// Author: Martin Lévesque <levesquem@emt.inrs.ca>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include <omnetpp.h>
#include "FiWiAdmissionControl.h"
#include <fstream>
#include <string>
#include <vector>
#include "FiWiMessages_m.h"
#include <algorithm>
#include <fstream>
#include <utility>

using namespace std;

Define_Module(FiWiAdmissionControl);

void FiWiAdmissionControl::initialize(int stage)
{
	if (stage != 5)
		return;

	cfgFile = par("fileInitTokens").stringValue();
	bytesPerToken = par("bytesPerToken").longValue();
	this->mu = par("mu").doubleValue();
	queueSizeInBytes = par("queueSizeInBytes").longValue();
	fairnessThreshold = par("fairnessThreshold").doubleValue();
	fairnessAlphaGreen = par("fairnessAlphaGreen").doubleValue();
	fairnessAlphaRed = par("fairnessAlphaRed").doubleValue();

	statusService = ADMISSION_CONTROL_STATUS_IDLE;
	transmissionMsg = new cMessage("admissionControlTransmissionMessage", 101);

	ifstream file;

	file.open(cfgFile.c_str());

	if (file.is_open())
	{
		while ( ! file.eof())
		{
			string host;
			string pktType;
			int nbTokens;
			int capacity;

			file >> host;
			file >> pktType;
			file >> nbTokens;
			file >> capacity;

			if (host == "")
			{
				break;
			}

			BucketInfos bucket;
			bucket.lambda = nbTokens;

			tokensForPktType[pktType].nbTokens += bucket.lambda;

			bucket.capacity = capacity;
			bucket.nbBytesAvailable = 0;
			bucket.fairnessAlphaGreen = fairnessAlphaGreen;
			bucket.fairnessAlphaRed = fairnessAlphaRed;
			bucket.nextTokenMsg = new cMessage("nextTokenAdmissionControl", 101);

			EV << " TOKEN INFO host = " << host << " pkt type = " << pktType << " index min = " << bucket.indexMin << " index max = " << bucket.indexMax << endl;

			EV << " 	tokensForPktType[pktType].nbTokens =  " << tokensForPktType[pktType].nbTokens << endl;

			if (bucket.lambda > 0)
			{
				scheduleAt(simTime() + 1.0 / (double)bucket.lambda, bucket.nextTokenMsg);
			}

			configs[host][pktType] = bucket;
			hosts.insert(host);
			pktTypes.insert(pktType);

		}

		file.close();
	}
	else
	{
		error("FiWiAdmissionControl::initialize: can't read admission control config file.");
	}

	int curIndex = 0;

	// Fill the index stuff for the queues:
	for (std::map<std::string, TokenForPktTypeObj>::iterator it = this->tokensForPktType.begin(); it != this->tokensForPktType.end(); ++it)
	{
		it->second.indexMin = curIndex;
		it->second.indexMax = curIndex + it->second.nbTokens - 1;
		EV << " index token for " << it->first << " min = " << it->second.indexMin << " max = " << it->second.indexMax << endl;
		curIndex += it->second.nbTokens;
	}

	/*

	AdmissionControlPacket p1;
	p1.color = ADMISSION_CONTROL_GREEN;
	p1.priority = 0.10;
	p1.pkt = new cPacket();

	AdmissionControlPacket p2;
	p2.color = ADMISSION_CONTROL_GREEN;
	p2.priority = 0.20;
	p2.pkt = new cPacket();

	AdmissionControlPacket p3;
	p3.color = ADMISSION_CONTROL_GREEN;
	p3.priority = 0.10;
	p3.pkt = new cPacket();
	p3.arrivalTime = 4;

	AdmissionControlPacket p4;
	p4.color = ADMISSION_CONTROL_RED;
	p4.priority = 0.10;
	p4.pkt = new cPacket();

	AdmissionControlPacket p5;
	p5.color = ADMISSION_CONTROL_RED;
	p5.priority = 0.20;
	p5.pkt = new cPacket();

	AdmissionControlPacket p6;
	p6.color = ADMISSION_CONTROL_RED;
	p6.priority = 0.30;
	p6.pkt = new cPacket();

	AdmissionControlPacket p4;
	p4.color = ADMISSION_CONTROL_GREEN;
	p4.priority = 0.10;
	p4.arrivalTime = 3;
	p4.pkt = new cPacket();

	queues["ptx"].push_back(p2);
	queues["ptx"].push_back(p3);
	queues["ptx"].push_back(p4);
	queues["ptx"].push_back(p1);

	sort(queues["ptx"].begin(), queues["ptx"].end(), CompareAdmissionControlPacket);

	for (int i = 0; i < (int)queues["ptx"].size(); ++i)
	{
		EV << " TEST queue color = " << queues["ptx"][i].color << " priority = " << queues["ptx"][i].priority << " arrival time = " << queues["ptx"][i].arrivalTime << endl;
	}
	*/
}

void BucketInfos::updateFairnessStatus(int color)
{
	double alpha = (color == ADMISSION_CONTROL_GREEN) ? fairnessAlphaGreen : fairnessAlphaRed;

	fairnessStatus = min(fairnessStatus + alpha, 1.0);
}

bool FiWiAdmissionControl::queueFullWith(int nbBytesPkt, const std::string& pktType)
{
	int nbBytes = 0;

	for (int i= 0; i < (int)queues[pktType].size(); ++i)
	{
		nbBytes += queues[pktType][i].pkt->getByteLength();
	}

	return nbBytes + nbBytesPkt >= this->queueSizeInBytes;
}

bool FiWiAdmissionControl::queuesEmpty()
{
	for (std::map<std::string, TokenForPktTypeObj>::iterator it = this->tokensForPktType.begin(); it != this->tokensForPktType.end(); ++it)
	{
		string packetType = it->first;

		if ( ! queues[packetType].empty())
			return false;
	}

	return true;
}

void FiWiAdmissionControl::updateNbInSystem()
{
	if (statusService == ADMISSION_CONTROL_STATUS_TRANSMITTING)
	{
		FiWiMessage* fiwiMsg = check_and_cast<FiWiMessage*>(packetToSend.pkt->getEncapsulatedPacket());
		string host = fiwiMsg->getHost();
		string pktType = fiwiMsg->getPktType();

		stats[host][pktType][packetToSend.color].totalNbInSystem += 1;
	}


	for (set<string>::iterator it = pktTypes.begin(); it != pktTypes.end(); ++it)
	{
		for (int i = 0; i < (int)queues[*it].size(); ++i)
		{
			FiWiMessage* fiTmp = check_and_cast<FiWiMessage*>(queues[*it][i].pkt->getEncapsulatedPacket());

			stats[fiTmp->getHost()][*it][queues[*it][i].color].totalNbInSystem += 1;
		}
	}
}

void FiWiAdmissionControl::processAnotherPacket()
{
	EV << "FiWiAdmissionControl::processAnotherPacket 1 " << endl;
	if (queuesEmpty())
		return;

	EV << "FiWiAdmissionControl::processAnotherPacket 2 " << endl;

	// At least one packet somewhere, let's get the next packet randomly:
	bool foundPacket = false;


	do
	{
		int indexRand = (int)uniform(0, mu);

		EV << "FiWiAdmissionControl::processAnotherPacket 3 random index " << indexRand << endl;

		for (std::map<std::string, TokenForPktTypeObj>::iterator it = this->tokensForPktType.begin(); it != this->tokensForPktType.end(); ++it)
		{
			string packetType = it->first;

			// Not empty and random matching queue indexes
			if ( ! queues[packetType].empty() && indexRand >= it->second.indexMin && indexRand <= it->second.indexMax)
			{
				foundPacket = true;
				packetToSend = queues[packetType][0];
				queues[packetType].erase(queues[packetType].begin()); //.pop();
				sort(queues[packetType].begin(), queues[packetType].end(), CompareAdmissionControlPacket);
				break;
			}
		}
	}
	while( ! foundPacket);

	EV << "FiWiAdmissionControl::processAnotherPacket 4 SHOOT ! " << endl;

	// Then we need to wait a little.
	double transmissionTime = 1 / mu;

	EV << "FiWiAdmissionControl::processAnotherPacket 5 transmission time " << transmissionTime << endl;

	statusService = ADMISSION_CONTROL_STATUS_TRANSMITTING;
	scheduleAt(simTime() + transmissionTime, transmissionMsg);
}

void FiWiAdmissionControl::handleMessage(cMessage *msg)
{
	if (msg->isSelfMessage())
	{
		if (msg == transmissionMsg)
		{
			// Shoot it out!
			send(packetToSend.pkt, "outPort$o");

			updateNbInSystem();

			// Update stats
			FiWiMessage* fiwiMsg = check_and_cast<FiWiMessage*>(packetToSend.pkt->getEncapsulatedPacket());
			string host = fiwiMsg->getHost();
			string pktType = fiwiMsg->getPktType();
			stats[host][pktType][packetToSend.color].nbPktsReceived += 1;
			stats[host][pktType][packetToSend.color].sumDelays += (simTime() - packetToSend.pkt->getCreationTime()).dbl();

			// Change transmission status:
			statusService = ADMISSION_CONTROL_STATUS_IDLE;

			packetToSend.pkt = NULL;

			// No time to wait, any packet to process ?
			processAnotherPacket();
		}

		bool msgProcessed = false;

		for (set<string>::iterator itHost = hosts.begin(); itHost != hosts.end() && ! msgProcessed; ++itHost)
		{
			for (set<string>::iterator itPktType = pktTypes.begin(); itPktType != pktTypes.end() && ! msgProcessed; ++itPktType)
			{
				BucketInfos& bucket = configs[*itHost][*itPktType];

				// Token event
				if (bucket.nextTokenMsg == msg)
				{
					// Can we put some tokens in the leaky bucket ?
					if (bucket.nbBytesAvailable + this->bytesPerToken <= bucket.capacity * this->bytesPerToken)
					{
						bucket.nbBytesAvailable += this->bytesPerToken;
					}

					msgProcessed = true;

					// Sched next token insertion
					scheduleAt(simTime() + 1.0 / (double)bucket.lambda, bucket.nextTokenMsg);
				}
			}
		}
	}
	else
	{
		cGate *ingate = msg->getArrivalGate();

		if (string(ingate->getName()) == string("inPort$i"))
		{
			cPacket* pkt = dynamic_cast<cPacket*>(msg);
			FiWiMessage* fiwiMsg = check_and_cast<FiWiMessage*>(pkt->getEncapsulatedPacket());

			string packetType = fiwiMsg->getPktType();
			string host = fiwiMsg->getHost();



			double priority = configs[host][packetType].lambda / this->mu;

			int color = ADMISSION_CONTROL_GREEN;

			// Flag a packet RED, not enough token
			if (configs[host][packetType].nbBytesAvailable < this->bytesPerToken)
			{
				EV << "host " << host << " pkt type " << packetType << " marked red !!" << endl;
				color = ADMISSION_CONTROL_RED;

				configs[host][packetType].updateFairnessStatus(ADMISSION_CONTROL_RED);
			}
			else
			{ // GREEN !

				// Use tokens:
				configs[host][packetType].nbBytesAvailable -= this->bytesPerToken;

				// Update fairness:
				configs[host][packetType].updateFairnessStatus(ADMISSION_CONTROL_GREEN);
			}

			stats[host][packetType][color].nbPktsArrived += 1;

			AdmissionControlPacket admPkt;
			admPkt.pkt = pkt;
			admPkt.color = color;
			admPkt.priority = priority;
			admPkt.arrivalTime = simTime().dbl();

			// EARLY DROP POLICING:
			if (configs[host][packetType].fairnessStatus < this->fairnessThreshold && color == ADMISSION_CONTROL_RED)
			{
				// Let's drop it, master.
				stats[host][packetType][color].nbPktsDropped += 1;
				stats[host][packetType][color].nbPktsEarlyDropped += 1;

				delete admPkt.pkt;
			}
			else
			{
				// Queue full ?
				if (queueFullWith(pkt->getByteLength(), packetType))
				{
					// We insert it and remove the less prioritized packet
					queues[packetType].push_back(admPkt);
					sort(queues[packetType].begin(), queues[packetType].end(), CompareAdmissionControlPacket);

					AdmissionControlPacket p = queues[packetType].back();
					queues[packetType].pop_back();

					FiWiMessage* pDropped = check_and_cast<FiWiMessage*>(p.pkt->getEncapsulatedPacket());

					stats[pDropped->getHost()][pDropped->getPktType()][p.color].nbPktsDropped += 1;

					delete p.pkt;
				}
				else
				{
					queues[packetType].push_back(admPkt);
					sort(queues["ptx"].begin(), queues["ptx"].end(), CompareAdmissionControlPacket);
				}
			}

			EV << "received " << fiwiMsg->getHost() << " pkt type = " << fiwiMsg->getPktType() << endl;

			// Alright we are not transmitting, ask for a packet:
			if (statusService == ADMISSION_CONTROL_STATUS_IDLE)
			{
				// Ask to process another packet.
				processAnotherPacket();
			}
		}
		else
		{
			error("FiWiAdmissionControl::handleMessage: wrong PORT (%s).", ingate->getName());
		}

		//delete msg;
	}
}

void FiWiAdmissionControl::finish ()
{
		std::ofstream myfile;
		myfile.open ("resultsAdmissionControl.txt", std::ios::app);
		vector<int> colors;
		colors.push_back(ADMISSION_CONTROL_RED);
		colors.push_back(ADMISSION_CONTROL_GREEN);

		for (set<string>::iterator itHost = hosts.begin(); itHost != hosts.end(); ++itHost)
		{
			for (set<string>::iterator itPktType = pktTypes.begin(); itPktType != pktTypes.end(); ++itPktType)
			{
				for (int i = 0; i < (int)colors.size(); ++i)
				{
					int color = colors[i];
					int nbPacketsArrived = stats[*itHost][*itPktType][color].nbPktsArrived;
					double blockingProb = (nbPacketsArrived > 0) ?stats[*itHost][*itPktType][color].nbPktsDropped / nbPacketsArrived : 0;

					int nbPacketsReceived = stats[*itHost][*itPktType][color].nbPktsReceived;
					double delay = (nbPacketsReceived > 0) ? stats[*itHost][*itPktType][color].sumDelays / nbPacketsReceived : 0;

					double nbInSystem = 1.0 / this->mu * (stats[*itHost][*itPktType][color].totalNbInSystem / simTime().dbl());

					myfile << *itHost << " " << *itPktType << " " << colors[i] << " " << stats[*itHost][*itPktType][color].nbPktsArrived << " " << stats[*itHost][*itPktType][color].nbPktsDropped << " " << stats[*itHost][*itPktType][color].nbPktsEarlyDropped << " " << blockingProb << " " << delay << " " << nbInSystem << endl;
				}
			}
		}

		myfile.close();
}
