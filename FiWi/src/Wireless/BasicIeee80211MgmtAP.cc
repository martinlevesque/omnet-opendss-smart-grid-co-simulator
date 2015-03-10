//
// Copyright (C) 2006 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//


#include "BasicIeee80211MgmtAP.h"
#include "Ieee802Ctrl_m.h"
#include "EtherFrame_m.h"
#include "NotifierConsts.h"
#include "RadioState.h"
#include "MyUtil.h"
#include <utility>
#include "EtherMAC.h"
#include "StatsCollector.h"
#include <iostream>
#include <fstream>
#include "Ieee80211Consts.h"
#include "BasicIeee80211Mac.h"
#include <list>
#include "FiWiTrafGen.h"
#include "FiWiGeneralConfigs.h"
#include <utility>

using namespace std;

#include "IP.h"

Define_Module(BasicIeee80211MgmtAP);

long BasicIeee80211MgmtAP::cntPackets = 0;

static std::ostream& operator<< (std::ostream& os, const BasicIeee80211MgmtAP::STAInfo& sta)
{
    os << "state:" << sta.status;
    return os;
}

BasicIeee80211MgmtAP::~BasicIeee80211MgmtAP()
{
	for (std::map<MACAddress, std::map<short, std::list<pair<double, EtherFrame* > > > >::iterator itDest = queues.begin();
			itDest != queues.end(); ++itDest)
	{
		for (std::map<short, std::list<pair<double, EtherFrame* > > >::iterator itKind = itDest->second.begin();
				itKind != itDest->second.end(); ++itKind)
		{
			// for (int i = 0; i < (int)itKind->second.size(); ++i)
			for (std::list<pair<double, EtherFrame* > >::iterator it = itKind->second.begin(); it != itKind->second.end(); ++it)
			{
				delete it->second;
			}
		}
	}
}

void BasicIeee80211MgmtAP::initialize(int stage)
{
    Ieee80211MgmtAPBase::initialize(stage);

    if (stage==0)
    {
    	nbPacketsRequested = 0;

    	cntPackets = 0;

        // read params and init vars
        ssid = par("ssid").stringValue();
        beaconInterval = par("beaconInterval");

        withBeacon = par("withBeacon").boolValue();

        wiType = par("wiType").stringValue();

        EV << "wi type = " << wiType << endl;

        EV << "WITH SUPER BEACON ?" << withBeacon << endl;

        numAuthSteps = par("numAuthSteps");
        if (numAuthSteps!=2 && numAuthSteps!=4)
            error("parameter 'numAuthSteps' (number of frames exchanged during authentication) must be 2 or 4, not %d", numAuthSteps);
        channelNumber = -1;  // value will arrive from physical layer in receiveChangeNotification()
        WATCH(ssid);
        WATCH(channelNumber);
        WATCH(beaconInterval);
        WATCH(numAuthSteps);
        WATCH_MAP(staList);
        isConnected = gate("uppergateOut")->getPathEndGate()->isConnected();

        aggregationTresholdBytes = par("aggregationTresholdBytes");
        maximumAggregationDuration = par("maximumAggregationDuration").doubleValue();
        queueLimit = par("queueLimit").doubleValue();

        FiWiGeneralConfigs::aggregationTresholdBytes = aggregationTresholdBytes;

        EV << "aggregationTresholdBytes = " << aggregationTresholdBytes << endl;
        EV << "maximumAggregationDuration = " << maximumAggregationDuration << endl;

        //TBD fill in supportedRates

        // subscribe for notifications
        NotificationBoard *nb = NotificationBoardAccess().get();
        nb->subscribe(this, NF_RADIO_CHANNEL_CHANGED);

        // start beacon timer (randomize startup time)

        beaconTimer = new cMessage("beaconTimer");

        if (withBeacon)
        	scheduleAt(simTime()+uniform(0,beaconInterval), beaconTimer);

        cModule *moduleRadio = getParentModule()->getSubmodule("radio");
        radioId = moduleRadio->getId();


    }
    else
    if (stage == 3)
    {
    	// addrTrafInterface
		cModule* module = MyUtil::getNeighbourOnGate(this, "ethg$o");
		module = MyUtil::getNeighbourOnGate(module, "ethg$o");
		module = MyUtil::getNeighbourOnGate(module, "ethg$o");

		if (module)
		{
			// connected via bridge
			int nbOut = module->gateSize("multiple$o");

			for (int i= 0; i < nbOut; ++i)
			{
				cModule* h = MyUtil::getNeighbourOnGate(module, "multiple$o", i);

				if (h)
				{
					EtherMAC* mac = dynamic_cast<EtherMAC*>(MyUtil::findModuleUp(h, "mac"));

					if (mac)
					{
						EV << "BasicIeee80211MgmtAP::initialize = ethg 1 = " << module << ", MAC = " << mac->getMACAddress() << endl;
						addrTrafInterface = mac->getMACAddress();
					}
				}
			}
		}

		cModule* tmpModule = MyUtil::findModuleUp(this, "wlan");

		if ( ! tmpModule)
		{
			error("Basic80211Radio::initialize - prob 0");
		}

		IBasic80211Mac* curMac = dynamic_cast<IBasic80211Mac*>(MyUtil::findModuleUp(tmpModule, "mac"));

		int collocatedPorts = gateSize("collocatedRadios");

		for (int port = 0; port < collocatedPorts; ++port)
		{
			string nameGate = "collocatedRadios$o";

			cGate * g = ((cSimpleModule*)this)->gate(nameGate.c_str(), port);

			if ( ! g)
				error("BasicIeee80211MgmtAP::initialize - prob 1");

			cGate* nextG = g->getNextGate();

			if ( ! nextG)
				error("BasicIeee80211MgmtAP::initialize - prob 2");

			cModule* module = nextG->getOwnerModule();

			if ( ! module)
				error("BasicIeee80211MgmtAP::initialize - prob 3");

			g = module->gate(nameGate.c_str(), port);

			if ( ! g)
				error("BasicIeee80211MgmtAP::initialize - prob 4");

			nextG = g->getNextGate();

			if ( ! nextG)
				error("BasicIeee80211MgmtAP::initialize - prob 5");

			module = nextG->getOwnerModule();

			if ( ! module)
				error("BasicIeee80211MgmtAP::initialize - prob 6");

			g = module->gate(nameGate.c_str(), port);

			if ( ! g)
				error("BasicIeee80211MgmtAP::initialize - prob 7");

			nextG = g->getNextGate();

			if ( ! nextG)
				error("BasicIeee80211MgmtAP::initialize - prob 8");

			module = nextG->getOwnerModule();

			if ( ! module)
				error("BasicIeee80211MgmtAP::initialize - prob 9");


			module = MyUtil::findModuleUp(module, "wlan");

			if ( ! module)
				error("BasicIeee80211MgmtAP::initialize - prob 10");

			EV << "MOOOOOOOOOOOOODULE = " << module << endl;

			IBasic80211Mac* mac = dynamic_cast<IBasic80211Mac*>(MyUtil::findModuleUp(module, "mac"));

			if ( ! mac)
				error("BasicIeee80211MgmtAP::initialize - prob 11");

			EV << "MGMT adding collocated radios " << mac->getMACAddress() << " <-> " << curMac->getMACAddress() << endl;

			MyUtil::getRoutingTable(this)->addAdjacent(FiWiNode(mac->getMACAddress(), FIWI_NODE_TYPE_WIRELESS),
					FiWiNode(curMac->getMACAddress(), FIWI_NODE_TYPE_WIRELESS));
			MyUtil::getRoutingTable(this)->setAreWirelessCollocated(mac->getMACAddress(), curMac->getMACAddress());
			MyUtil::getRoutingTable(this)->print();
		}
    }
}

void BasicIeee80211MgmtAP::handleTimer(cMessage *msg)
{
    if (msg == beaconTimer)
    {
    	if (withBeacon)
    	{
			sendBeacon();
			scheduleAt(simTime()+beaconInterval, beaconTimer);
    	}
    }
    else
    {
        error("internal error: unrecognized timer '%s'", msg->getName());
    }
}

void BasicIeee80211MgmtAP::handleCommand(int msgkind, cPolymorphic *ctrl)
{
    error("handleCommand(): no commands supported");
}

void BasicIeee80211MgmtAP::receiveChangeNotification(int category, const cPolymorphic *details)
{
	// dropManagementFrame(frame);
}

BasicIeee80211MgmtAP::STAInfo *BasicIeee80211MgmtAP::lookupSenderSTA(Ieee80211ManagementFrame *frame)
{
	dropManagementFrame(frame);

	return NULL;
}

void BasicIeee80211MgmtAP::sendManagementFrame(Ieee80211ManagementFrame *frame, const MACAddress& destAddr)
{
	dropManagementFrame(frame);
}

void BasicIeee80211MgmtAP::sendBeacon()
{
    EV << "Sending beacon\n";
    Ieee80211BeaconFrame *frame = new Ieee80211BeaconFrame("Beacon");
    Ieee80211BeaconFrameBody& body = frame->getBody();
    body.setSSID(ssid.c_str());
    body.setSupportedRates(supportedRates);
    body.setBeaconInterval(beaconInterval);
    body.setChannelNumber(channelNumber);

    frame->setReceiverAddress(MACAddress::BROADCAST_ADDRESS);
    frame->setFromDS(true);

    sendOrEnqueue(frame);
}

void BasicIeee80211MgmtAP::frameReceivedDest(cPacket* packet)
{
	EV << "BasicIeee80211MgmtAP::handleDataFrame FRAME RECEIVED !" << endl;

	if (simTime() > 2)
	{

	}

	// Send to application
	send(packet, "uppergateOut");
}

void BasicIeee80211MgmtAP::finish()
{
}

int BasicIeee80211MgmtAP::sizeOfQueueInBytes(MACAddress addr, short kind)
{
	int bytes = 0;

	// for (int i = 0; i < (int)queues[addr][kind].size(); ++i)
	for (std::list<pair<double, EtherFrame* > >::iterator it = queues[addr][kind].begin(); it != queues[addr][kind].end(); ++it)
	{
		bytes += it->second->getByteLength();
	}

	return bytes;
}

// TODO: no good.. should not check creation time !! but the enqued time
double BasicIeee80211MgmtAP::aggregationDuration(MACAddress addr, short kind)
{
	double maxDuration = 0.0;

	for (std::list<pair<double, EtherFrame* > >::iterator it = queues[addr][kind].begin(); it != queues[addr][kind].end(); ++it)
	{
		// Cur time - enqueud time
		double curTime = simTime().dbl() - it->first;

		if (curTime > maxDuration)
			maxDuration = curTime;
	}

	return maxDuration;
}

void BasicIeee80211MgmtAP::handleUpperMessage(cPacket *msg)
{
	EV << "BasicIeee80211MgmtAP::handleUpperMessage OK " << msg << " arrived !" << endl;

	// JcM Add: we assume the packet comes in raw way.

	EV << "BasicIeee80211MgmtAP::handleUpperMessage - Handling upper message from Network Layer" << endl;

	cGate *ingate = msg->getArrivalGate();

	if (ingate)
	{
		EV << "BasicIeee80211MgmtAP::handleDataFrame -> IN GATE = " << ingate->getFullPath() << endl;
		if (string(ingate->getName()) ==  string("collocatedRadios$i"))
		{
			Ieee80211DataFrame* frame = dynamic_cast<Ieee80211DataFrame*>(msg);

			if (frame)
			{
				pair<bool, FiWiNode> routing =
						MyUtil::getRoutingTable(this)->nextHop(frame->getTransmitterAddress(), frame->getReceiverAddress(), frame->getFiwiType());

				if (routing.first)
				{
					if (routing.second.addr == myAddress)
					{
						// route it
						if ( ! routeFrame(frame))
						{
							delete msg;
							return;
						}
					}
					else
					{
						delete msg;
						return;
					}
				}
			}
			else
			{
				delete msg;
				return;
			}
		}
	}

	EtherFrame* tmpFrame = dynamic_cast<EtherFrame*>(msg);

	if ( ! tmpFrame)
	{
		// error("BasicIeee80211MgmtAP::handleUpperMessage - err not ether frame");
		delete tmpFrame;
		return;
	}

	tmpFrame->setInternalTimestamp(simTime().raw());

	if (string(tmpFrame->getNextAction()) == string("dropDueToConnectedToHost"))
	{
		delete msg;
		return;
	}

	if (tmpFrame && (tmpFrame->getPktType() == FIWI_TRAF_PKT_TYPE_VIDEO || tmpFrame->getPktType() == FIWI_TRAF_PKT_TYPE_BEST_EFFORT ||
			tmpFrame->getPktType() == FIWI_TRAF_PKT_TYPE_VOICE))
	{
		static RawStat throughput = RawStat("DownstreamTriplePlayThroughput80211Handle", "event", 1);
		throughput.addStat(simTime().dbl(), tmpFrame->getBitLength(), 0);
		//delete f;
		//return;
	}

	MACAddress dest = tmpFrame->getDest();

	if (dest == myAddress)
	{
		frameReceivedDest(msg);
		//delete msg;

		//delete msg;
		return;
	}

	if (dest != MACAddress::BROADCAST_ADDRESS)
	{
		if ( ! MyUtil::getRoutingTable(this)->nodeExists(dest))
		{
			EV << "dont know." << endl;

			EV << "STA with MAC address " << dest << " not associated with this AP, dropping frame\n";
			delete msg; // XXX count drops?
			return;
		}
	}

	if ( ! addrTrafInterface.isUnspecified() && addrTrafInterface == dest && string(msg->getArrivalGate()->getFullName()) == string("ethg$i"))
	{
		EV << "BasicIeee80211MgmtAP::handleUpperMessage - " << ", discarding. msg->getArrivalGate() = " << msg->getArrivalGate()->getFullName() << endl;
		delete msg;
		return;
	}

	short kind = (tmpFrame->getKind() >= 100) ? (tmpFrame->getKind() - 100) : tmpFrame->getKind();

	EV << "BasicIeee80211MgmtAP::handleUpperMessage - KIND = " << kind << " CUR len = " << lengthOverallQueueSize() << " queue limit = " << queueLimit << endl;

	if (lengthOverallQueueSize() >= queueLimit)
	{
		// Drop the packet, queue full.
		EV << "BasicIeee80211MgmtAP QUEUE FULL" << endl;
		delete msg;
		return;
	}
	else
	{
		if (tmpFrame->getKind() >= 100) // Fast retransmit
		{
			EV << "BasicIeee80211MgmtAP::handleUpperMessage - casssse1" << endl;
			queues[dest][kind].push_front(make_pair(simTime().dbl() - 1, tmpFrame));
			tmpFrame->setKind(tmpFrame->getKind() - 100);
		}
		else
		{
			EV << "BasicIeee80211MgmtAP::handleUpperMessage - casssse2" << endl;
			queues[dest][kind].push_back(make_pair(simTime().dbl(), tmpFrame));
		}

		if (nbPacketsRequested > 0)
		{
			generateAndSendPacket();
		}
	}
}

double BasicIeee80211MgmtAP::lengthOverallQueueSize()
{
	double size = 0;

	for (std::map<MACAddress, std::map<short, std::list<pair<double, EtherFrame* > > > >::iterator itDest = queues.begin(); itDest != queues.end(); ++itDest)
	{
		for (std::map<short, std::list<pair<double, EtherFrame* > > >::iterator itKind = itDest->second.begin(); itKind != itDest->second.end(); ++itKind)
		{
			// for (int i = 0; i < (int)itKind->second.size(); ++i)
			for (std::list<pair<double, EtherFrame* > >::iterator it = itKind->second.begin(); it != itKind->second.end(); ++it)
			{
				size += it->second->getByteLength();
			}
		}
	}

	return size;
}

void BasicIeee80211MgmtAP::generateAndSendPacket()
{
	// std::map<MACAddress, std::map<short, std::vector<EtherFrame*> > > queues;
	EV << "BasicIeee80211MgmtAP::generateAndSendPacket - ok 1" << endl;

	MACAddress destOldest;
	short kindOldest = -1;
	double durationOldest = -1;

	for (std::map<MACAddress, std::map<short, std::list<pair<double, EtherFrame* > > > >::iterator itDest = queues.begin(); itDest != queues.end(); ++itDest)
	{
		EV << "BasicIeee80211MgmtAP::generateAndSendPacket - ok 2" << endl;
		for (std::map<short, std::list<pair<double, EtherFrame* > > >::iterator itKind = itDest->second.begin(); itKind != itDest->second.end(); ++itKind)
		{
			EV << "BasicIeee80211MgmtAP::generateAndSendPacket - ok 3" << endl;
			MACAddress dest = itDest->first;
			short kind = itKind->first;

			int curQueueSize = sizeOfQueueInBytes(dest, kind);

			double curAggregationDuration = aggregationDuration(dest, kind);



			if (curQueueSize > 0 && (curQueueSize >= aggregationTresholdBytes || curAggregationDuration  >= maximumAggregationDuration))
			{
				EV << "		OK size = " << curQueueSize << " nb packets = " << queues[dest][kind].size() << endl;

				EV << "   cur queue size = " << curQueueSize << " th size = " << aggregationTresholdBytes << " curAggregationDuration = " << curAggregationDuration << " maximumAggregationDuration = " << maximumAggregationDuration << endl;

				// FIND OLDEST !!!!
				if (curAggregationDuration > durationOldest)
				{
					destOldest = dest;
					kindOldest = kind;
					durationOldest = curAggregationDuration;
				}
			}
		}
	}

	if (durationOldest >= 0)
	{
		++cntPackets;
		char name[300];
		sprintf(name, "wireless-frame-%ld", cntPackets);

		ListEtherFrames* frames = new ListEtherFrames(name);

		// Fill the list of frames
		int aggregatedPacketSize = 0;
		EV << "		test 1" << endl;

		vector<EtherFrame> framesToCopy;

		for (list<pair<double, EtherFrame* > >::iterator it = queues[destOldest][kindOldest].begin(); it != queues[destOldest][kindOldest].end(); )
		{
			EV << "test 2" << endl;
			if (aggregatedPacketSize >= aggregationTresholdBytes && aggregatedPacketSize > 0)
				break;

			EV << "test 3" << endl;

			if ( ! it->second)
			{
				error("BasicIeee80211MgmtAP::generateAndSendPacket - err 1");
			}

			framesToCopy.push_back(EtherFrame(*(it->second)));

			aggregatedPacketSize += it->second->getByteLength();

			EV << "test aggregatedPacketSize = " << aggregatedPacketSize << endl;

			delete it->second;
			it = queues[destOldest][kindOldest].erase(it);

			EV << "test 4 = " << endl;
		}

		frames->setFramesArraySize(framesToCopy.size());
		EV << "		framesToCopy.size() = " << framesToCopy.size() << endl;

		if (framesToCopy.size() == 0)
		{
			error("BasicIeee80211MgmtAP::generateAndSendPacket - err 2");
		}

		for (int i= 0; i < (int)framesToCopy.size(); ++i)
		{
			frames->setFrames(i, framesToCopy[i]);
		}

		frames->setByteLength(aggregatedPacketSize);

		EV << "aggregated packet size = " << aggregatedPacketSize << endl;

		// Then empty the list
		// queues[dest].clear();

		Ieee80211DataFrame* frame = new Ieee80211DataFrame(name);
		frame->setFromDS(true);
		frame->setFiwiType(kindOldest);

		// copy addresses from ethernet frame (transmitter addr will be set to our addr by MAC)
		frame->setReceiverAddress(destOldest);
		frame->setAddress3(myAddress);
		frame->setTransmitterAddress(myAddress);

		// encapsulate list of ether frames
		if (wiType == "MSDU")
		{
			// PAPER:
			frame->setByteLength(36 + 4 + (16 * (int)framesToCopy.size())); // MAC header + FCS + Header for each frames in the frame aggregate
		}
		else
		{
			frame->setByteLength((24 * (int)framesToCopy.size())); // No MAC header neither FCS + Header for each frames in the frame aggregate
		}

		EV << "BasicIeee80211MgmtAP::generateAndSendPacket - frame size before encap = " << frame->getBitLength() << endl;

		frame->encapsulate(frames);
		EV << "BasicIeee80211MgmtAP::generateAndSendPacket - frame size after encap = " << frame->getBitLength() << endl;
		sendOrEnqueue(frame);

		--nbPacketsRequested;
		return;
	}
}

void BasicIeee80211MgmtAP::removeSubPacketsWithErrors(Ieee80211Frame* frame)
{
	ListEtherFrames* listFrames = dynamic_cast<ListEtherFrames*>(frame->getEncapsulatedPacket());

	if (listFrames)
	{
		// If it's ==, then no need to recopy since it must entirely be resent
		listFrames = dynamic_cast<ListEtherFrames*>(frame->decapsulate());

		EV << "BasicIeee80211MgmtAP::removeSubPacketsWithErrors - size = " << listFrames->getByteLength() << endl;

		vector<EtherFrame> packetsToRecopy;

		for (int j = 0; j < (int)listFrames->getFramesArraySize(); ++j)
		{
			if (listFrames->getFrames(j).getKind() != BITERROR)
			{
				EV << "  good, got id., index = " << j << endl;
				packetsToRecopy.push_back(EtherFrame(listFrames->getFrames(j)));
			}
		}

		ListEtherFrames* listFrames2 = new ListEtherFrames(listFrames->getName());

		delete listFrames;

		listFrames2->setByteLength(sizeInBytes(packetsToRecopy));
		listFrames2->setFramesArraySize(packetsToRecopy.size());

		for (int i = 0; i < (int)packetsToRecopy.size(); ++i)
		{
			listFrames2->setFrames(i, EtherFrame(packetsToRecopy[i]));
		}

		EV << "BasicIeee80211MgmtAP::removeSubPacketsWithErrors - size = " << listFrames2->getByteLength() << endl;

		frame->encapsulate(listFrames2);
	}
}

int BasicIeee80211MgmtAP::sizeInBytes(const vector<EtherFrame>& frames)
{
	int bytes = 0;

	for (int i = 0; i < (int)frames.size(); ++i)
	{
		bytes += frames[i].getByteLength();
	}

	return bytes;
}

void BasicIeee80211MgmtAP::sendOrEnqueue(cPacket *frame)
{
	send(frame, "macOut");
}

bool BasicIeee80211MgmtAP::routeFrame(Ieee80211DataFrame *frame)
{
	if (frame->getKind() == IEEE80211MAC_REQUEST_FOR_PACKET)
	{
		++this->nbPacketsRequested;
		generateAndSendPacket();

		delete frame;
		return true;
	}
	else
	{
		removeSubPacketsWithErrors(frame);
	}

	if (frame->getReceiverAddress() == myAddress)
	{
		std::vector<EtherFrame*> frames = convertToEtherFrames(frame);

		for (int i = 0; i < (int)frames.size(); ++i)
		{
			frameReceivedDest(frames[i]);
		}

		EV << "is my addresss!" << endl;

		return true;
	}

	EV << "BasicIeee80211MgmtAP::routeFrame my addr = " << myAddress << " transmitter addr -> " << frame->getTransmitterAddress() << " dest = " << frame->getReceiverAddress() << endl;

	pair<bool, FiWiNode> res2 = MyUtil::getRoutingTable(this)->nextHop(myAddress, frame->getReceiverAddress(), frame->getFiwiType());

	EV << "bla 2 res2.first =  " << res2.first << endl;

	// EV << "OK -> next hop = " << res.second.toString() << endl;
	// return res.second.addr == address;
	if (res2.first)
	{

		FiWiNodeType nodeType = MyUtil::getRoutingTable(this)->nodeTypeOf(res2.second.addr);

		EV << "NODE TYPE OF PACKET = " << nodeType << endl;

		if (nodeType == FIWI_NODE_TYPE_WIRELESS)
		{
			EV << "OKKKKKKKKKK, wireless" << endl;
			if (MyUtil::getRoutingTable(this)->areWirelessCollocated(myAddress, res2.second.addr))
			{
				EV << "ARE COLLOCATED!!" << endl;

				// Should send dirently via collocation
				frame->setTransmitterAddress(myAddress);

				// It is to a collocated.. need to broadcast!
				int collocatedPorts = gateSize("collocatedRadios");

				for (int port = 0; port < collocatedPorts; ++port)
				{
					string nameGate = "collocatedRadios$o";

					cGate * g = ((cSimpleModule*)this)->gate(nameGate.c_str(), port);

					// Need to make a cpy
					Ieee80211DataFrame* f = (port == collocatedPorts - 1) ? frame : frame->dup();

					send(f, g);
				}
			}
			else
			{	// Send to wireless
				EV << "SEND TO WIRELESSSSSSSSSSSSSSSSS" << endl;
				frame->setTransmitterAddress(myAddress);

				sendOrEnqueue(frame);
			}

			return true;
		}
		else
		if (nodeType == FIWI_NODE_TYPE_ANY || nodeType == FIWI_NODE_TYPE_PON)
		{
			// Need to forward to ethg, remove Ieee80211 stuff before forwarding !
			MACAddress src = frame->getTransmitterAddress();
			MACAddress dest = frame->getReceiverAddress();

			std::vector<EtherFrame*> frames = convertToEtherFrames(frame);

			EV << "rawr yep nb frames = " << frames.size() << endl;

			for (int i = 0; i < (int)frames.size(); ++i)
			{
				frames[i]->setSrc(src);
				frames[i]->setDest(dest);

				if (simTime().dbl() > 2.0)
				{
					// Save the WMN delay!!
					simtime_t delay;
					delay.setRaw(simTime().raw() - frames[i]->getInternalTimestamp());


					if (delay.dbl() > 0)
					{
						FiWiGeneralConfigs::updateWMNAverageDelay(delay.dbl());
					}
				}

				// Also, reset the timestamp
				frames[i]->setInternalTimestamp(simTime().raw());

				send(frames[i], "ethg$o");
			}

			return true;
		}
	}

	return false;
}

void BasicIeee80211MgmtAP::handleDataFrame(Ieee80211DataFrame *frame)
{
	EV << "BasicIeee80211MgmtAP::handleDataFrame 1" << endl;

	// This case is when an ack is received, to re-enqueue
	if (frame->getKind() == BITERROR)
	{
		EV << "BasicIeee80211MgmtAP::handleDataFrame 2" << endl;
		EV << "BLUB dropping for now..." << endl;

		ListEtherFrames* listFrames = dynamic_cast<ListEtherFrames*>(frame->getEncapsulatedPacket());

		if (listFrames)
		{
			for (int i = 0; i < (int)listFrames->getFramesArraySize(); ++i)
			{
				if (listFrames->getFrames(i).getKind() == BITERROR)
				{
					EtherFrame* f = listFrames->getFrames(i).dup();
					f->setKind(frame->getFiwiType() + 100);

					handleUpperMessage(f);
				}
			}
		}

		delete frame;
		return;
	}
	else
	if (routeFrame(frame))
	{
		EV << "BasicIeee80211MgmtAP::handleDataFrame 3" << endl;
	}
	else
	{
		// Drop
		EV << "BasicIeee80211MgmtAP::handleDataFrame - Frame is not for us -- discarding\n";
		delete frame;
		return;
	}

}

void BasicIeee80211MgmtAP::handleAuthenticationFrame(Ieee80211AuthenticationFrame *frame)
{
	dropManagementFrame(frame);
}

void BasicIeee80211MgmtAP::handleDeauthenticationFrame(Ieee80211DeauthenticationFrame *frame)
{
	dropManagementFrame(frame);
}

void BasicIeee80211MgmtAP::handleAssociationRequestFrame(Ieee80211AssociationRequestFrame *frame)
{
	dropManagementFrame(frame);
}

void BasicIeee80211MgmtAP::handleAssociationResponseFrame(Ieee80211AssociationResponseFrame *frame)
{
    dropManagementFrame(frame);
}

void BasicIeee80211MgmtAP::handleReassociationRequestFrame(Ieee80211ReassociationRequestFrame *frame)
{
	dropManagementFrame(frame);
}

void BasicIeee80211MgmtAP::handleReassociationResponseFrame(Ieee80211ReassociationResponseFrame *frame)
{
    dropManagementFrame(frame);
}

void BasicIeee80211MgmtAP::handleDisassociationFrame(Ieee80211DisassociationFrame *frame)
{
	dropManagementFrame(frame);
}

void BasicIeee80211MgmtAP::handleBeaconFrame(Ieee80211BeaconFrame *frame)
{
    dropManagementFrame(frame);
}

void BasicIeee80211MgmtAP::handleProbeRequestFrame(Ieee80211ProbeRequestFrame *frame)
{
    dropManagementFrame(frame);
}

void BasicIeee80211MgmtAP::handleProbeResponseFrame(Ieee80211ProbeResponseFrame *frame)
{
    dropManagementFrame(frame);
}

std::vector<EtherFrame*> BasicIeee80211MgmtAP::convertToEtherFrames(Ieee80211DataFrame *frame)
{
    // create a matching ethernet frame
    // EtherFrame *ethframe = new EthernetIIFrame(frame->getName()); //TODO option to use EtherFrameWithSNAP instead

    //XXX set ethertype

    // encapsulate the payload in there
    ListEtherFrames* frames = dynamic_cast<ListEtherFrames*>(frame->decapsulate());

    ASSERT(frames != NULL);

    std::vector<EtherFrame*> listFrames;
    std::vector<EtherFrame*> framesToDelete;

    EV << "rawr - nb frames -> " << frames->getFramesArraySize() << endl;

    if (frames->getFramesArraySize() == 0)
    {
    	error("BasicIeee80211MgmtAP::convertToEtherFrames err 1 .. name = %s", frames->getName());
    }

    for (int i = 0; i < (int)frames->getFramesArraySize(); ++i)
    {
    	string name = frames->getFrames(i).getName();

    	EV << "rawr .. name extracted = " << name << " frames->getFrames(i).getByteLength() = " << frames->getFrames(i).getByteLength() << endl;

    	EtherFrame* f = frames->getFrames(i).dup();

    	if ( ! f)
    	{
    		error("BasicIeee80211MgmtAP::convertToEtherFrames err 2");
    	}

    	f->setDest(frame->getReceiverAddress());
    	f->setSrc(frame->getTransmitterAddress());
    	f->setKind(frame->getFiwiType());
    	//f->setByteLength(frames->getFrames(i).getByteLength());

		// ethframe->encapsulate(payload);
    	listFrames.push_back(f);
    }

    delete frames;

    delete frame;

    // done
    return listFrames;
}

Ieee80211DataFrame * BasicIeee80211MgmtAP::convertFromEtherFrame(EtherFrame *ethframe)
{
    // create new frame
    Ieee80211DataFrame *frame = new Ieee80211DataFrame(ethframe->getName());
    frame->setFromDS(true);
    frame->setFiwiType(ethframe->getKind());

    // copy addresses from ethernet frame (transmitter addr will be set to our addr by MAC)
    frame->setReceiverAddress(ethframe->getDest());
    frame->setAddress3(ethframe->getSrc());

    // encapsulate payload
    cPacket *payload = ethframe->decapsulate();
    if (!payload)
        error("received empty EtherFrame from upper layer");
    frame->encapsulate(payload);
    delete ethframe;

    // done
    return frame;
}


