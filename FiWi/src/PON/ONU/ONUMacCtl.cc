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

#include "ONUMacCtl.h"
#include "StatsCollector.h"
#include "PONUtil.h"
#include "MyUtil.h"
#include "ONUQPerLLiDBase.h"

#include <iostream>
#include <fstream>
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include "RawStat.h"
#include "ONUTable.h"

using namespace std;

Define_Module(ONUMacCtl);

ONUMacCtl::ONUMacCtl()
{
	/*
	startTxMsg= 0;
	stopTxMsg= 0;
	changeParametersMsg = 0;
	*/
}

ONUMacCtl::~ONUMacCtl()
{
}

void ONUMacCtl::initialize(int stage)
{
	if (stage != 6)
		return;

	numGates = 0;
	wdmEnabled = par("wdmEnabled").boolValue();
	nbWDMChannels = par("nbWDMChannels");

	state = ONU_STATE_IDLE;

	// Link to MAC layer
	emac = dynamic_cast<EPON_mac *>(getNeighbourOnGate("lowerLayerOut"));

	EV << "ONU ADDR = " << emac->getMACAddress() << endl;

	if (!emac)
		error("No MAC layer found connected under ONU MAC Control Layer");

	// Init Stats
	numFramesFromHL=0;
	numFramesFromLL=0;
	numFramesFromHLDropped=0;
	fragmentedTime=0;

	WATCH(numFramesFromHL);
	WATCH(numFramesFromLL);
	WATCH(numFramesFromHLDropped);
	WATCH(fragmentedTime);

	// Initialize the Q mgmt module
	queue_mod = dynamic_cast<IExtPassiveQueue *>(getNeighbourOnGate("upperLayerOut"));

	if ( ! queue_mod)
		error("ONUMacCtl: An IEponQueueMgmt is needed above mac control");

	int nbChannels = 1;

	if (wdmEnabled)
	{
		nbChannels += nbWDMChannels;
	}

	eponPollingMsg = new cMessage("eponPollingCycleTime", EPON_POLLING_TIME_MSG);

	if (PONUtil::getPonVersion(this) == 1)
	{
		// Create the channels
		for (int i= 0; i < nbChannels; ++i)
		{
			ChannelInfos c;
			c.id = i;
			channels.push_back(c);
		}
	}
	else
	if (PONUtil::getPonVersion(this) == 2)
	{
		// Only ONE wavelength
		ONUInfoWR o = ONUTable::getONUInfoWROf(emac->getMACAddress());

		ChannelInfos c;
		c.id = o.channel;
		channels.push_back(c);
	}

	EV << "ONUMacCtl::initialize: wdm enabled = " << wdmEnabled << " nb = " << channels.size() << endl;

	// Initialization of the channels:
	for (int i= 0; i < (int)channels.size(); ++i)
	{
		channels[i].startBurst= 0;
		channels[i].lenBurst= 0;
	}

	///////////
	// Polling, to speed up simulation, it is *emulated*.
	scheduleAt(1 + FiWiGeneralConfigs::eponPollingCycleTime, eponPollingMsg);
	EV << "INIT 2 FiWiGeneralConfigs::eponPollingCycleTime .. " << FiWiGeneralConfigs::eponPollingCycleTime << endl;

	////////
	// Grab the OLT mgmt object
	oltMgmtObj = NULL;
	cModule* oltTmp = MyUtil::findModuleUp(this, "epon_olt");

	if (oltTmp)
	{
		oltTmp = MyUtil::findModuleUp(oltTmp, "olt_if");

		if (oltTmp)
		{
			oltMgmtObj = dynamic_cast<IOLTMgmt*>(MyUtil::findModuleUp(oltTmp, "olt_Q_mgmt"));

			if ( ! oltMgmtObj)
			{
				error("ONUMacCtl::initialize: can't find epon_olt 1 ");
			}
		}
		else
		{
			error("ONUMacCtl::initialize: can't find epon_olt 2");
		}
	}
	else
	{
		error("ONUMacCtl::initialize: can't find epon_olt 3");
	}
}

void ONUMacCtl::handleMessage(cMessage *msg)
{
	// Self Message
	if (msg->isSelfMessage())
	{
		EV << "Self-message " << msg << " received\n";

		bool destFound = false;

		if (msg == eponPollingMsg)
		{
			EV << "FiWiGeneralConfigs::eponPollingCycleTime .. " << simTime() << endl;
			destFound = true;
			scheduleAt(simTime() + FiWiGeneralConfigs::eponPollingCycleTime, eponPollingMsg);
			reserveAndPrepareEPONBurst();
		}

		if ( ! destFound)
		{
			EPON_LLidCtrlInfo* nfo = dynamic_cast<EPON_LLidCtrlInfo *>(check_and_cast<cPacket*>(msg)->getControlInfo());

			EV << "SEND PACKET - sending packet" << msg << " size = " << calcPacketSizeOnMedium(check_and_cast<cPacket*>(msg)->getByteLength()) << " channel = " << nfo->channel << endl;
			// It's a DATA packet previously scheduled, need to forward lower
			sendToLowerLayer(msg);
		}

		return;
	}

	// Network Message
	cGate *ingate = msg->getArrivalGate();
	EV << "Frame " << msg << " arrived on port " << ingate->getName() << "...\n";

	if (ingate->getId() ==  gate( "lowerLayerIn")->getId())
	{
		processFrameFromMAC(msg);
	}
	else if (ingate->getId() ==  gate( "upperLayerIn")->getId())
	{
		string msgName = msg->getName();

		processFrameFromHigherLayer(msg);
	}
	else
	{
		EV << "ONUMacCtl: Message came FROM THE WRONG DIRRECTION???? Dropping\n";
		delete msg;
	}
}

void ONUMacCtl::sendToLowerLayer(cMessage* msg)
{
	EtherFrame * frame = dynamic_cast<EtherFrame *>(msg);

	if ( ! frame)
	{
		error("Problem ONUMacCtl::sendToLowerLower");
	}



	state = ONU_STATE_TX;
	send(frame, "lowerLayerOut");
}

void ONUMacCtl::reserveAndPrepareEPONBurst()
{
	// CALL OLT, reserve.

	uint32_t sizeQueue = (uint32_t)queue_mod->queueSizeInBytes();



	EV << "ONUMacCtl::reserveAndPrepareEPONBurst 1 " << endl;

	std::vector<ONUReservation> reservations = oltMgmtObj->DoUpstreamDBA(emac->getMACAddress(), sizeQueue);

	// After reservation, because the OLT needs to be aware there is nothing!!
	if (sizeQueue == 0)
	{
		// Nothing to schedule !
		return;
	}

	EV << "ONUMacCtl::reserveAndPrepareEPONBurst 2 nb =  " << reservations.size() << endl;

	simtime_t t;

	int totalLength = 0;

	for (int i= 0; i < (int)reservations.size(); ++i)
	{
		t.setRaw(reservations[i].getStart());



		EV << "reservation delay = " << (t - simTime()).dbl() << endl;

		int ch = reservations[i].getChannel();

		int indexChannelCh = 0;

		// Can be unordered!!!
		for (int j = 0; j < (int)channels.size(); ++j)
			if (channels[j].id == ch)
				indexChannelCh = j;

		EV << "BURST start at " << t << " on channel " << ch << endl;

		channels[indexChannelCh].startBurst = t;
		channels[indexChannelCh].lenBurst = reservations[i].getSlotSizeInBytes();

		ONUTable::channelUtil[channels[indexChannelCh].id][(int)simTime().dbl()] += reservations[i].getSlotSizeInBytes() * 8;

		ONUTable::channelSchedDelay[channels[indexChannelCh].id] = (t - simTime()).dbl();

		totalLength += channels[indexChannelCh].lenBurst;
	}

	for (int i= 0; i < (int)channels.size(); ++i)
	{
		dumpReg(channels[i]);
	}

	// Extract the packets in the burst to be sent:
	burst.clear();
	EV << "clear burst at " << simTime().dbl() << " .... " << this->getFullPath() << endl;
	nbPacketsInBurst = 0;
	state = ONU_STATE_IDLE;

	do
	{
		int curPacketSize = queue_mod->requestPacket(totalLength);

		totalLength -= curPacketSize;

		if (curPacketSize == 0)
		{
			break;
		}

		++nbPacketsInBurst;
	}
	while(totalLength > 0);

}

void ONUMacCtl::handleEPONDataPacket(cMessage* msg)
{
	burst.insert(check_and_cast<cPacket*>(msg));

	EV << "SEND PACKET - nb packets = " << burst.size() << " nbPacketsInBurst = " << nbPacketsInBurst << endl;

	if (nbPacketsInBurst == (int)burst.size())
	{
		EV << "SEND PACKET - FINISHED RECEIVED PACKETS." << endl;

		double sumBytesUsed = 0;
		double sumBandwidthAllocated = 0;

		for (int i= 0; i < (int)channels.size(); ++i)
		{
			// Schedule packets in this channel
			simtime_t curTime = channels[i].startBurst;
			simtime_t endAllocation = channels[i].endSimTimeAllocation(this);

			// We need to desactive this schedule !!!!!!!!!!!!!
			channels[i].startBurst = 0;

			if (endAllocation <= simTime())
			{
				EV << "SEND PACKET skipping channel " << channels[i].id << " cuz end alloc = " << endAllocation << " start = " << curTime <<  endl;
				continue;
			}

			sumBandwidthAllocated += channels[i].lenBurst;

			EV << "SEND PACKET burst start at " << curTime << endl;
			EV << "SEND PACKET endAllocation = " << endAllocation << endl;
			EV << "SEND PACKET lenburst = " << channels[i].lenBurst << endl;

			while (curTime < endAllocation)
			{
				EV << "SEND PACKET loop curTime = " << curTime << " endAllocation = " << endAllocation << endl;

				bool onePacketAllocated = false;

				for (std::set<cPacket*, lessThanPacket>::iterator it = burst.begin(); it != burst.end(); )
				{
					cPacket* p = *it;

					// Does it fit ?
					simtime_t beginPacket = curTime;
					EV << "SEND PACKET size = " << p->getByteLength() << " txrate = " << PONUtil::getCapacityOf(this, channels[i].id) << endl;
					simtime_t endPacket = beginPacket + (double)(calcPacketSizeOnMedium(p->getByteLength()) * 8) / (double)PONUtil::getCapacityOf(this, channels[i].id);

					if (endPacket <= endAllocation)
					{
						// Mark the packet to the right channel !
						EPON_LLidCtrlInfo* nfo = dynamic_cast<EPON_LLidCtrlInfo *>(p->getControlInfo());

						if ( ! nfo)
						{
							error("ONUMacCtl::processFrameFromHigherLayer unknown packet, problem.");
						}

						nfo->channel = channels[i].id;

						EV << "SEND PACKET - scheduling packet " << p << " of size " << (calcPacketSizeOnMedium(p->getByteLength()) * 8) << " bits on channel " << nfo->channel << endl;

						sumBytesUsed += calcPacketSizeOnMedium(p->getByteLength());

						scheduleAt(beginPacket, p);

						// Don't send it twice
						std::set<cPacket*, lessThanPacket>::const_iterator duplicate = it;
						++it;

						burst.erase(duplicate);

						// A new packet has been scheduled, add the cur time
						curTime = endPacket;
						onePacketAllocated = true;
					}
					else
					{
						++it;
					}
				}

				if ( ! onePacketAllocated)
					break;
			}
		}

		double waste = sumBandwidthAllocated - sumBytesUsed;
		StatsCollector::Instance().addStat("wasted_bandwidth", STAT_TYPE_PER_SECOND, (int)simTime().dbl(), waste * 8);
	}
}

void ONUMacCtl::processFrameFromHigherLayer(cMessage *msg)
{
	EV << "ONUMacCtl: Outgoing message, forwarding...\n";
	numFramesFromHL++;

	// Pre-Configuration ...
	EV << "\tPacket " << msg << " arrived from higher layers, sending\n";

	if (simTime() < 1)
	{

		EV<<"Frame arrived in pre-conf stage "<<endl;
		send(msg, "lowerLayerOut");
		return;
	}
	else
	{
		handleEPONDataPacket(msg);
	}
}

void ONUMacCtl::processFrameFromMAC(cMessage *msg)
{
	EV << "ONUMacCtl: Incoming message from PON\n";
	EtherFrame * frame = dynamic_cast<EtherFrame *>(msg);

	if (dynamic_cast<EthernetIIFrame *>(msg) && dynamic_cast<EthernetIIFrame *>(msg)->getEtherType() == MPCP_TYPE)
	{
		// Check that the frame is for us...
		if (frame->getDest() != emac->getMACAddress() && !frame->getDest().isBroadcast())
		{
			EV << "MPCP not for us... dropping\n";
			delete frame;
			return;
		}

		processMPCP(frame );
	}
	else
	{
		// Should we forward or drop ??
		// Check the next hop from the OLT to the destination
		pair<bool, FiWiNode> res = MyUtil::getRoutingTable(this)->nextHop(MyUtil::getRoutingTable(this)->getOltAddr(), frame->getDest(), FIWI_NODE_TYPE_ANY);

		if ( ! res.first || res.second.addr != emac->getMACAddress())
		{
			EV << "DROPPING AT ONU, addr = " << emac->getMACAddress() << " res first = " << res.first << " res second = " << res.second.addr << endl;
			delete msg;
			return;
		}

		// DATA PACKET. Let's record the downstream delay!!!
		if (simTime().dbl() >= 2.0)
		{
			simtime_t delay;
			delay.setRaw(simTime().raw() - frame->getInternalTimestamp());
			FiWiGeneralConfigs::updatePONDownstreamDelay(delay.dbl());
		}
	}

	send(msg,"upperLayerOut");
	numFramesFromLL++;
}

void ONUMacCtl::processMPCP(EtherFrame *frame )
{
	EV << "ONUMacCtl: MPCP Frame processing\n";
	MPCP * mpcp = check_and_cast<MPCP *>(frame);

	switch (mpcp->getOpcode())
	{
		case MPCP_GATE:
		{
			MPCPGate * gate = check_and_cast<MPCPGate *>(frame);
			EV << "ONUMacCtl: Type is MPCP_GATE\n";

			/**
			 *  NOTE: Announced time IS NOT ALWAYS ON THE FUTURE
			 *  IF we want the announced times to be on the future
			 *  then the DBA algorithms MUST CONSIDER RTT. For simplicity
			 *  we do accept to loose some slots...
			 */
			if ( numGates == 0)
			{
				EV << "ONUMacCtl::processMPCP: initial" << endl;

				EV << "ONUMacCtl: MPCP_GATE arrived, IT IS INITIAL - MAC: "<<frame->getDest()<<endl;
				// Cancel ALL an scheduled TX

			}

			numGates++;
			break;
		}
		default:
			break;
	};
}

uint32_t ONUMacCtl::calcPacketSizeOnMedium(uint32_t packetSizeInBytes)
{
	// TODO add parameter OVERHEAD
	// if (packetSizeInBytes < 64)
	//	packetSizeInBytes= 64;

	uint32_t bytes= packetSizeInBytes;

	// TODO add parameter OVERHEAD
	// bytes+= PREAMBLE_BYTES + SFD_BYTES;
	// bytes+= INTERFRAME_GAP_BITS/8;

	return bytes;
}

void ONUMacCtl::dumpReg(ChannelInfos& ch)
{
	EV << "Sim(ns16): "<<MPCPTools::simTimeToNS16()<<endl;
	EV << "startBurst: " << ch.startBurst << endl;
	EV << "lenBurst: " << ch.lenBurst << endl;
}

void ONUMacCtl::finish ()
{
    simtime_t t = simTime();
    recordScalar("simulated time", t);
    recordScalar("messages handled", numFramesFromHL+numFramesFromLL);
    recordScalar("Dropped Frames From HL", numFramesFromHLDropped);

    double fragBits = fragmentedTime.dbl()/(1.0/(double)txrate);
    recordScalar("FragmentedBits", fragBits);

    if (t > 0)
    {
        recordScalar("frames/sec", (numFramesFromHL+numFramesFromLL)/t);
        recordScalar("drops/sec", numFramesFromHLDropped/t);
        recordScalar("FragmentedBits/sec", fragBits/t);
    }
}

cModule * ONUMacCtl::getNeighbourOnGate(const char * g)
{
	return gate(g)->getNextGate()->getOwnerModule();
}

