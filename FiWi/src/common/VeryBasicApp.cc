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
#include "VeryBasicApp.h"
#include "StatsCollector.h"
#include <fstream>
#include "FiWiMessages_m.h"

using namespace std;

Define_Module(VeryBasicApp);

void VeryBasicApp::initialize(int stage)
{
	if (stage != 5)
		return;

	packetSizeInBytes = par("packetSizeInBytes").longValue();
	lambda = par("lambda").longValue();
	hostId = par("hostId").stringValue();
	packetType = par("packetType").stringValue();

	msgNextPacket = new cMessage("nextPacketVeryBasicApp", 101);

	EV << "VERY BASIC APP INIT" << endl;
	EV << "		packetSizeInBytes = " << packetSizeInBytes << endl;
	EV << "		lambda = " << lambda << endl;
	EV << "		hostId = " << hostId << endl;
	EV << "		packetType = " << packetType << endl;

	cnt = 0;

	if (lambda > 0)
	{

		double nextP = 1.0 / exponential((double)lambda);

		EV << "lambda = " << lambda << " nextP = " << nextP << endl;

		scheduleAt(simTime() + nextP, msgNextPacket);
	}
}

double VeryBasicApp::randomInter()
{
	double rand = (uniform(0,30) - 15) / 100.0;
	double randNumber = (1.0 + rand) * (double)lambda;

	double n = 0;


	while ( (n = poisson((double)lambda+0.5)) <= 0)
	{

	}

	// n = uniform((double)lambda * 0.95, (double)lambda * 1.05);

	return 1 / n;
}

void VeryBasicApp::handleMessage(cMessage *msg)
{
	if (msg->isSelfMessage())
	{
		if (msg == msgNextPacket)
		{
			scheduleAt(simTime() + randomInter(), msgNextPacket);

			++cnt;


			cPacket* packet = new cPacket();
			packet->setName("veryBasicAppPacket");
			packet->setByteLength(packetSizeInBytes);

			FiWiMessage* encapMsg = new FiWiMessage();
			encapMsg->setHost(this->hostId.c_str());
			encapMsg->setPktType(this->packetType.c_str());
			packet->encapsulate(encapMsg);

			send(packet, "inoutPort$o");
		}
	}
	else
	{
		cGate *ingate = msg->getArrivalGate();
		EV << "VeryBasicNode::handleMessage: Frame " << msg << " arrived on port " << ingate->getName() << "...\n";

		cPacket* pkt = dynamic_cast<cPacket*>(msg);

		double delay = (simTime() - pkt->getCreationTime()).dbl();

		EV << "pkt creation = " << delay << endl;

		StatsCollector::Instance().addStat("delay", STAT_TYPE_MEAN, simTime().dbl(), delay);

		delete msg;
	}
}

void VeryBasicApp::finish ()
{
	EV << "CNT of lambda " << lambda << " = " << cnt << endl;
}
