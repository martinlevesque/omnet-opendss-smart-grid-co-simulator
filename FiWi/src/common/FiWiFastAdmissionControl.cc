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

#include <fstream>
#include <string>
#include <vector>
#include "FiWiMessages_m.h"
#include <algorithm>
#include <fstream>
#include <utility>

#include "FiWiFastAdmissionControl.h"
#include "FiWiTrafGen.h"
#include "MyUtil.h"

using namespace std;

struct myclass {
    bool operator()(const FastAdmissionControlPacket& p1, const FastAdmissionControlPacket& p2)
    {
    	// Color priority
    	if (p1.color > p2.color)
    		return true;

    	// Host priority
    	if (p1.color == p2.color && p1.priority > p2.priority)
    	    return true;

    	// Arrival time priority, oldest first
    	if (p1.color == p2.color && p1.priority == p2.priority && p1.arrivalTime < p2.arrivalTime)
    		return true;

    	return false;
    }
} FiWiFastCompareAdmissionControl;


void FiWiFastAdmissionControl::initialize(cModule* thisModule)
{
	bytesPerToken = 1;
	queueSizeInBytes = this->limit;

	fairnessThreshold = 0.2; // par("fairnessThreshold").doubleValue();
	fairnessAlphaGreen = 0.001; // par("fairnessAlphaGreen").doubleValue();
	fairnessAlphaRed = -0.01; // par("fairnessAlphaRed").doubleValue();

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

			MACAddress addr = MyUtil::resolveDestMACAddress(thisModule, host);
			EV << "my super addr = " << addr << endl;

			FastBucketInfos bucket;
			bucket.lambda = nbTokens;

			bucket.capacity = capacity;
			bucket.nbBytesAvailable = 0;
			bucket.fairnessAlphaGreen = fairnessAlphaGreen;
			bucket.fairnessAlphaRed = fairnessAlphaRed;

			int iPktType = FIWI_TRAF_PKT_TYPE_BEST_EFFORT;

			if (pktType == "bestEffort")
			{
				iPktType = FIWI_TRAF_PKT_TYPE_BEST_EFFORT;
			}
			else
			if (pktType == "video")
			{
				iPktType = FIWI_TRAF_PKT_TYPE_VIDEO;
			}
			else
			if (pktType == "voice")
			{
				iPktType = FIWI_TRAF_PKT_TYPE_VOICE;
			}
			else
			if (pktType == "smartGridControl")
			{
				iPktType = FIWI_TRAF_PKT_TYPE_SMART_GRID_CONTROL;
			}
			else
			if (pktType == "smartGridNotification")
			{
				iPktType = FIWI_TRAF_PKT_TYPE_SMART_GRID_NOTIFICATION;
			}

			EV << " TOKEN INFO host = " << host << " pkt type = " << pktType << " index min = " << bucket.indexMin << " index max = " << bucket.indexMax << endl;

			bucket.lastTokenUpdate = 0;

			configs[addr.str()][iPktType] = bucket;
			hosts.insert(addr.str());
			pktTypes.insert(iPktType);

		}

		file.close();
	}
	else
	{
		return;
	}
}

FiWiFastAdmissionControl::FiWiFastAdmissionControl()
{
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_SMART_GRID_CONTROL] = FastTokenForPktTypeObj(0.4);
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_SMART_GRID_CONTROL].indexMin = 0;
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_SMART_GRID_CONTROL].indexMax = 40;
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_SMART_GRID_NOTIFICATION] = FastTokenForPktTypeObj(0.3);
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_SMART_GRID_NOTIFICATION].indexMin = 40;
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_SMART_GRID_NOTIFICATION].indexMax = 70;
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_BEST_EFFORT] = FastTokenForPktTypeObj(0.02);
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_BEST_EFFORT].indexMin = 70;
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_BEST_EFFORT].indexMax = 71;
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_VIDEO] = FastTokenForPktTypeObj(0.14);
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_VIDEO].indexMin = 71;
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_VIDEO].indexMax = 86;
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_VOICE] = FastTokenForPktTypeObj(0.14);
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_VOICE].indexMin = 86;
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_VOICE].indexMax = 100;

	sizeInBytes = 0;
}

void FastBucketInfos::updateFairnessStatus(int color)
{
	double alpha = (color == FAST_ADMISSION_CONTROL_GREEN) ? fairnessAlphaGreen : fairnessAlphaRed;

	fairnessStatus = max(0.0, min(fairnessStatus + alpha, 1.0));
}

void FastBucketInfos::updateTokens(int bytesPerToken)
{
	double nbSeconds = simTime().dbl() - this->lastTokenUpdate;
	double nbTokens = nbSeconds * (double)this->lambda;

	nbBytesAvailable += nbTokens * (double)bytesPerToken;

	if (nbBytesAvailable > (double)capacity * (double)bytesPerToken)
	{
		nbBytesAvailable = (double)capacity * (double)bytesPerToken;
	}

	lastTokenUpdate = simTime().dbl();
}

bool FiWiFastAdmissionControl::queueFullWith(int nbBytesPkt, int pktType)
{
	int nbBytes = 0;

	for (int i= 0; i < (int)queues[pktType].size(); ++i)
	{
		nbBytes += queues[pktType][i].pkt->getByteLength();
	}

	double sizeQueue = queueSizeInBytes / (double)pktTypePriorities.size();

	return nbBytes + nbBytesPkt > sizeQueue;
}

bool FiWiFastAdmissionControl::queuesEmpty()
{
	for (std::map<int, FastTokenForPktTypeObj>::iterator it = this->pktTypePriorities.begin(); it != this->pktTypePriorities.end(); ++it)
	{
		int packetType = it->first;

		if ( ! queues[packetType].empty())
			return false;
	}

	return true;
}

void FiWiFastAdmissionControl::remove(cPacket* pkt)
{
	for (std::map<int, FastTokenForPktTypeObj>::iterator it = this->pktTypePriorities.begin(); it != this->pktTypePriorities.end(); ++it)
	{
		int packetType = it->first;

		if ( ! queues[packetType].empty() && queues[packetType].begin()->pkt == pkt)
		{
			queues[packetType].erase(queues[packetType].begin()); //.pop();
			sort(queues[packetType].begin(), queues[packetType].end(), FiWiFastCompareAdmissionControl);

			sizeInBytes -= pkt->getByteLength();

			return;
		}
	}
}

cPacket* FiWiFastAdmissionControl::front(int maxPacketSize)
{
	EV << "FiWiAdmissionControl::front 1 " << endl;
	if (queuesEmpty())
		return NULL;

	EV << "FiWiAdmissionControl::front 2 " << endl;

	bool atLeastOnePacketMin = false;

	for (std::map<int, FastTokenForPktTypeObj>::iterator it = this->pktTypePriorities.begin(); it != this->pktTypePriorities.end(); ++it)
	{
		int packetType = it->first;

		if ( ! queues[packetType].empty() && queues[packetType][0].pkt->getByteLength() <= maxPacketSize)
		{
			atLeastOnePacketMin = true;
			break;
		}
	}

	if ( ! atLeastOnePacketMin)
	{
		return NULL;
	}

	// At least one packet somewhere, let's get the next packet randomly:
	bool foundPacket = false;

	do
	{
		int indexRand = (int)uniform(0, 100);

		EV << "FiWiAdmissionControl::front 3 random index " << indexRand << endl;

		for (std::map<int, FastTokenForPktTypeObj>::iterator it = this->pktTypePriorities.begin(); it != this->pktTypePriorities.end(); ++it)
		{
			int packetType = it->first;

			// Not empty and random matching queue indexes
			if ( ! queues[packetType].empty() && indexRand >= it->second.indexMin && indexRand < it->second.indexMax &&
					queues[packetType][0].pkt->getByteLength() <= maxPacketSize)
			{
				foundPacket = true;
				FastAdmissionControlPacket pktToSend = queues[packetType][0];
				queues[packetType].erase(queues[packetType].begin()); //.pop();
				sort(queues[packetType].begin(), queues[packetType].end(), FiWiFastCompareAdmissionControl);

				return pktToSend.pkt;
			}
		}
	}
	while( ! foundPacket);

	return NULL;
}

uint32_t FiWiFastAdmissionControl::length()
{
	uint32_t cnt = 0;

	for (std::map<int, FastTokenForPktTypeObj>::iterator it = this->pktTypePriorities.begin(); it != this->pktTypePriorities.end(); ++it)
	{
		//for (int i = 0; i < (int)queues[it->first].size(); ++i)
		cnt += queues[it->first].size();
	}

	return cnt;
}

uint32_t FiWiFastAdmissionControl::lengthInBytes()
{
	return sizeInBytes;
}

void FiWiFastAdmissionControl::enqueue(cPacket *msg)
{
	EtherFrame* pkt = dynamic_cast<EtherFrame*>(msg);

	int packetType = pkt->getPktType();
	string host = pkt->getHost();

	// Check for new tokens:
	configs[host][packetType].updateTokens(this->bytesPerToken);

	double priority = 0; // should be adaptive normally

	int color = FAST_ADMISSION_CONTROL_GREEN;

	// Flag a packet RED, not enough token
	if (configs[host][packetType].nbBytesAvailable < pkt->getByteLength())
	{
		EV << "host " << host << " pkt type " << packetType << " marked red !!" << " nb avail = " << configs[host][packetType].nbBytesAvailable << " bytesPerToken = " << bytesPerToken << endl;
		color = FAST_ADMISSION_CONTROL_RED;
	}
	else
	{ // GREEN !

		// Use tokens:
		configs[host][packetType].nbBytesAvailable -= pkt->getByteLength();
	}

	// Update fairness:
	configs[host][packetType].updateFairnessStatus(color);

	//static RawStat s1 = RawStat("color", "event", 1);

	/*
	if (packetType == FIWI_TRAF_PKT_TYPE_SMART_GRID_CONTROL || packetType == FIWI_TRAF_PKT_TYPE_SMART_GRID_NOTIFICATION)
		s1.log(simTime().dbl(), color, "");

	static RawStat s2 = RawStat("queueLength", "event", 1);
	s2.log(simTime().dbl(), lengthInBytes(), "");
	*/

	FastAdmissionControlPacket admPkt;
	admPkt.pkt = pkt;
	admPkt.color = color;
	admPkt.priority = priority;
	admPkt.arrivalTime = simTime().dbl();

	// EARLY DROP POLICING:
	if (configs[host][packetType].fairnessStatus < this->fairnessThreshold && color == FAST_ADMISSION_CONTROL_RED)
	{
		static RawStat s = RawStat("earlyDrop", "event", 1);

		if (packetType == FIWI_TRAF_PKT_TYPE_SMART_GRID_CONTROL || packetType == FIWI_TRAF_PKT_TYPE_SMART_GRID_NOTIFICATION)
			s.log(simTime().dbl(), configs[host][packetType].fairnessStatus, "");

		delete admPkt.pkt;
	}
	else
	{
		// Queue full ?
		if (queueFullWith(pkt->getByteLength(), packetType))
		{
			// We insert it and remove the less prioritized packet
			queues[packetType].push_back(admPkt);
			sort(queues[packetType].begin(), queues[packetType].end(), FiWiFastCompareAdmissionControl);
			sizeInBytes += admPkt.pkt->getByteLength();

			FastAdmissionControlPacket p = queues[packetType].back();
			queues[packetType].pop_back();
			sizeInBytes -= p.pkt->getByteLength();

			delete p.pkt;
		}
		else
		{
			queues[packetType].push_back(admPkt);
			sort(queues[packetType].begin(), queues[packetType].end(), FiWiFastCompareAdmissionControl);
			sizeInBytes += admPkt.pkt->getByteLength();
		}
	}

	EV << "received " << pkt->getHost() << " pkt type = " << pkt->getPktType() << endl;
}
