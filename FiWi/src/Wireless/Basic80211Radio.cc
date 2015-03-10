// Martin Lévesque <levesquem@emt.inrs.ca>
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
#include "EPON_CtrlInfo.h"
#include "Basic80211Radio.h"
#include "Lambda.h"
#include "PONUtil.h"
#include "MyUtil.h"
#include <string>
#include "EtherMAC.h"
#include "WirelessMacBase.h"
#include "IBasic80211Mac.h"
#include "FiWiTrafGen.h"
#include "Ieee80211Consts.h"
#include "EtherMacVlan.h"
#include <vector>
#include "RawStat.h"
#include "BasicIeee80211Mac.h"

using namespace std;

Define_Module(Basic80211Radio);

void Basic80211Radio::initialize(int stage)
{
	if (stage < 3)
	{
		return;
	}

    ports = gateSize("radios");
    propagationDelay = par("propagationDelay");
    BER = par("BER");
    selectiveBERAck = par("selectiveBERAck").boolValue();

    phyHeaderDuration = par("phyHeaderDuration").doubleValue();

    EV << "PROPAGATION RADIO -> " << propagationDelay << endl;
    EV << "BER -> " << BER << endl;
    EV << "selectiveBERAck -> " << selectiveBERAck << endl;

    //
    // Build routing table with adjacent nodes
    //

    cModule* tmpModule = MyUtil::findModuleUp(this, "wlan");

    if ( ! tmpModule)
    {
    	error("Basic80211Radio::initialize - prob 0");
    }

    IBasic80211Mac* curMac = dynamic_cast<IBasic80211Mac*>(MyUtil::findModuleUp(tmpModule, "mac"));

    if ( ! curMac)
    {
    	error("Basic80211Radio::initialize - prob 00");
    }

    // RADIO CONNECTIONS:

    for (int port = 0; port < ports; ++port)
    {
    	EV << "port = " << port << endl;

    	string nameGate = "radios$o";

    	cGate * g = ((cSimpleModule*)this)->gate(nameGate.c_str(), port);

    	if ( ! g)
    		error("Basic80211Radio::initialize - prob 1");

    	cGate* nextG = g->getNextGate();

    	if ( ! nextG)
    		error("Basic80211Radio::initialize - prob 2");

    	cModule* module = nextG->getOwnerModule();

    	if ( ! module)
    		error("Basic80211Radio::initialize - prob 3");

    	g = module->gate(nameGate.c_str(), port);

    	if ( ! g)
    		error("Basic80211Radio::initialize - prob 4");

    	nextG = g->getNextGate();

    	if ( ! nextG)
    		error("Basic80211Radio::initialize - prob 5");

    	module = nextG->getOwnerModule();

    	if ( ! module)
    		error("Basic80211Radio::initialize - prob 6");

    	g = module->gate(nameGate.c_str(), port);

    	if ( ! g)
    		error("Basic80211Radio::initialize - prob 7");

    	nextG = g->getNextGate();

    	if ( ! nextG)
    		error("Basic80211Radio::initialize - prob 8");

    	module = nextG->getOwnerModule();

    	if ( ! module)
    		error("Basic80211Radio::initialize - prob 9");


    	module = MyUtil::findModuleUp(module, "wlan");

    	if ( ! module)
    		error("Basic80211Radio::initialize - prob 10");

    	EV << "MOOOOOOOOOOOOODULE = " << module << endl;

    	IBasic80211Mac* mac = dynamic_cast<IBasic80211Mac*>(MyUtil::findModuleUp(module, "mac"));

    	if ( ! mac)
    	    error("Basic80211Radio::initialize - prob 11");

    	MyUtil::getRoutingTable(this)->addAdjacent(FiWiNode(mac->getMACAddress(), FIWI_NODE_TYPE_WIRELESS), FiWiNode(curMac->getMACAddress(), FIWI_NODE_TYPE_WIRELESS));
    	MyUtil::getRoutingTable(this)->print();
    }

    ///////////////////////////////////////
    // ethg connection ! It is also possible that ethg was used to connect to another network, e.g.: PON.
    cModule* module = MyUtil::findModuleUp(this, "wlan");

    module = MyUtil::getNeighbourOnGate(module, "ethg$o");

    if (module)
    {
    	module = MyUtil::getNeighbourOnGate(module, "ethg$o");

    	if (module)
    	{
			module = MyUtil::getNeighbourOnGate(module, "single$o");

			if (module)
			{
				EtherMacVlan* macVlan = dynamic_cast<EtherMacVlan*>(MyUtil::findModuleUp(module, "mac"));

				MyUtil::getRoutingTable(this)->addAdjacent(FiWiNode(curMac->getMACAddress(), FIWI_NODE_TYPE_WIRELESS), FiWiNode(macVlan->getMACAddress(), FIWI_NODE_TYPE_ANY));
				MyUtil::getRoutingTable(this)->print();
			}

    		//EV << "NEIGHBOR = " << module << endl;
    	}
    }

    // TODO ASSOC STA TO ZONE !!!
    EV << "whoopie" << this->getId() << " to " << curMac->getMACAddress() << endl;


    //
    // Get the space areas
    //

    this->portsRange = this->gateSize("rangeAreas");

    for (int range = 0; range < portsRange; ++range)
    {
		module = MyUtil::getNeighbourOnGate(this, "rangeAreas", range);

		if ( ! module)
		{
			error("Basic80211Radio::initialize - No space area 1");
		}

		module = MyUtil::getNeighbourOnGate(module, "rangeAreas", range);

		if ( ! module)
		{
			error("Basic80211Radio::initialize - No space area 2");
		}

		RangeArea* rangeArea = dynamic_cast<RangeArea*>(MyUtil::getNeighbourOnGate(module, "rangeAreas", range));

		MyUtil::getRoutingTable(this)->addNodeInWiZone(rangeArea->getId(), curMac->getMACAddress());

		ASSERT(dynamic_cast<BasicIeee80211Mac*>(curMac));
		MyUtil::getRoutingTable(this)->wiMacZones[rangeArea->getId()].push_back(dynamic_cast<BasicIeee80211Mac*>(curMac));

		if ( ! rangeArea)
		{
			error("Basic80211Radio::initialize - No space area 3");
		}

		rangeArea->setBER(BER);
		rangeArea->setSelectiveBERAck(selectiveBERAck);

		rangeAreas.push_back(rangeArea);

		EV << "Basic80211Radio::initialize - added range area " << rangeArea << endl;
    }
}

std::vector<RangeArea*> Basic80211Radio::getAreaRangesOf(cModule* thisModule)
{
	int portsRange = thisModule->gateSize("rangeAreas");
	std::vector<RangeArea*> areas;

	for (int range = 0; range < portsRange; ++range)
	{
		RangeArea* rangeArea = dynamic_cast<RangeArea*>(MyUtil::getNeighbourOnGate(thisModule, "rangeAreas", range));

		if ( ! rangeArea)
			continue;

		areas.push_back(rangeArea);
	}

	return areas;
}

void Basic80211Radio::handleMessage(cMessage *msg)
{
	if (msg->isSelfMessage())
	{
		EV << "Basic80211Radio::handleMessage rcv delayed packet " << msg << ", forwarding to mac" << " id = " << msg->getId() << endl;

		for (int range = 0; range < (int)rangeAreas.size(); ++range)
		{
			RangeArea* rangeArea = rangeAreas[range];

			if ( ! rangeArea)
				continue;

			rangeArea->erase(check_and_cast<Ieee80211Frame*>(msg));
		}

		// Send to MAC!
		send(msg, "uppergateOut");
		return;
	}

	// Handle frame sent down from the network entity: send out on every other port
	cGate *ingate = msg->getArrivalGate();
	EV << "Basic80211Radio::handleMessage: recv woo., pretty msg size = " << dynamic_cast<cPacket*>(msg)->getByteLength() << " in gate = " << ingate->getName() << "...\n";

	if (string(ingate->getName()) ==  string("radios$i"))
	{
		EV << "Basic80211Radio::handleMessage: inside..." << endl;

		Ieee80211Frame *frame = dynamic_cast<Ieee80211Frame *>(msg);

		if ( ! frame)
		{
			EV << " MSG = " << msg << endl;
			error("ERRRRRRRRRRRRRRRRR not 80211 frame !");
		}

		// RECEIVED, is there any collision or bit error ?

		// cModule* moduleIn = ingate->getNextGate()->getOwnerModule();

		cModule* moduleIn = MyUtil::getNeighbourOnGate(this, "radios$o", ingate->getIndex());

		if ( ! moduleIn)
		{
			error("Basic80211Radio::handleMessage - 1");
		}

		moduleIn = MyUtil::getNeighbourOnGate(moduleIn, "radios$o", ingate->getIndex());

		if ( ! moduleIn)
		{
			error("Basic80211Radio::handleMessage - 2");
		}

		moduleIn = MyUtil::getNeighbourOnGate(moduleIn, "radios$o", ingate->getIndex());

		if ( ! moduleIn)
		{
			error("Basic80211Radio::handleMessage - 3");
		}

		std::vector<RangeArea*> rangeAreasNeighbour = getAreaRangesOf(moduleIn);

		EV << "Basic80211Radio::handleMessage - moduleIn = " << moduleIn << " nb neighbour areas " << rangeAreasNeighbour.size() << endl;

		RangeArea* rangeArea = NULL;

		double delay = 0;

		// COLLISION:
		for (int rangeThis = 0; rangeThis < portsRange; ++rangeThis)
		{
			for (int rangeNeighbour = 0; rangeNeighbour < (int)rangeAreasNeighbour.size(); ++rangeNeighbour)
			{

				if (rangeAreasNeighbour[rangeNeighbour] != rangeAreas[rangeThis])
				{
					continue;
				}

				rangeArea = rangeAreas[rangeThis];

				// Transmission delay
				delay = rangeArea->delayOf(frame->getBitLength());

				EV << "RADIO, frame bit len = " << frame->getBitLength() << endl;

				// Phy header delay
				string name = frame->getName();

				if (name.find("-rts-") == string::npos && name.find("-cts-") == string::npos && name.find("-ack-") == string::npos)
				{
					delay += phyHeaderDuration;
					EV << "ok adding phy header..." << endl;
				}

				// Then, add PROPAGATION DELAY.
				delay += propagationDelay;

				EV << "HOURA transmission delay = " << rangeArea->delayOf(frame->getBitLength()) << ", phy header duration = " << phyHeaderDuration << " propagation del = " << propagationDelay << " total delay = " << delay << endl;

				vector<Ieee80211Frame*> framesInCollision = rangeArea->channelBusy(frame);

				if (framesInCollision.size() > 0)
				{
					static RawStat mystats = RawStat("80211RadioColl", "event", 1);
					mystats.log(simTime().dbl(), 0.0, string("frame generating coll = ") + string(frame->getFullName()));

					EV << "Basic80211Radio::handleMessage COOOOOOOOOOOOOOOOOLLISION ! frame = " << frame->getFullName() << endl;
					frame->setKind(COLLISION);

					// Also, all frames which collide must be marked as a collision:
					for (int i = 0; i < (int)framesInCollision.size(); ++i)
					{
						framesInCollision[i]->setKind(COLLISION);
						EV << "		Basic80211Radio::handleMessage marked as coll ! frame = " << framesInCollision[i]->getFullName() << " id = " << framesInCollision[i]->getId() << endl;
						mystats.log(simTime().dbl(), framesInCollision[i]->getArrivalTime().dbl(), string("  ") + string(framesInCollision[i]->getFullName()));
					}
				}
				else
				{
					// Consume bandwidth
					rangeArea->useChannel(frame, delay);
				}
			}
		}

		if ( ! rangeArea)
		{
			EV << "Basic80211Radio::handleMessage - dropping, no range area." << endl;
			delete msg;
			return;
		}



		// Delay the packet
		simtime_t sendAt = simTime() + delay;
		EV << "Basic80211Radio::handleMessage delaying until " << sendAt << endl;
		scheduleAt(sendAt, msg);
	}
	else
	{
		Ieee80211Frame* tmp = dynamic_cast<Ieee80211Frame *>(msg);

		EV << "Basic80211Radio::handleMessage: outside msg type = " << tmp->getType() << " rts type = " << ST_RTS << " !..." << endl;

		// We need to broadcast to all connected radio ports
		for (int i=0; i < ports; i++)
		{
			// if it is the last port to send, do NOT duplicate
			bool isLast = i == ports-1;

			// Check if we have 1 port to ONU...
			if (ports==1)
				isLast=true;

			Ieee80211Frame* msg2 = isLast ? tmp : tmp->dup();

			cGate* selectedGate = gate("radios$o", i);

			send(msg2, selectedGate);
		}
	}
}

void Basic80211Radio::finish ()
{
}
