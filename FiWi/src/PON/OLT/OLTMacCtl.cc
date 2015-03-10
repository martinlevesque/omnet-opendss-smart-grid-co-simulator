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

#include "OLTMacCtl.h"
#include "EPON_mac.h"
#include "PONUtil.h"
#include "MyUtil.h"
#include <utility>
#include "FiWiTrafGen.h"
#include "SimpleQueue.h"
#include "MyAbstractQueue.h"
#include "FiWiFastAdmissionControl.h"
#include "MyWeightedFairQueue.h"

using namespace std;

Define_Module(OLTMacCtl);

OLTMacCtl::~OLTMacCtl()
{
}

double OLTMacCtl::maximumQueueDuration(int channel)
{
	return (double)(queueLimit * 8) / PONUtil::getCapacityOf(this, channel);
}

void OLTMacCtl::initialize(int stage)
{
	if (stage < 3)
		return;

    // Initialize the Q mgmt module
	queue_mod = dynamic_cast<IPassiveQueue *>(getNeighbourOnGate("upperLayerOut"));
	emac = dynamic_cast<EPON_mac *>(getNeighbourOnGate("lowerLayerOut"));

	admissionControlConfs = par("admissionControlConfs").stringValue();
	queuingStrategy = par("queuingStrategy").stringValue();

	if ( ! emac)
	{
		error("OLTMacCtl::initialize no epon_mac ?");
	}

	//oltBaseObj = dynamic_cast<OLTQPerLLiDBase *>(getNeighbourOnGate("upperLayerOut"));

	onutbl = dynamic_cast<ONUTable *>( MyUtil::findModuleUp(this, "onuTable"));

	// Set OLT addr, useful to build routing table
	onutbl->setOLTAddr(emac->getMACAddress());
	MyUtil::getRoutingTable(this)->setOltAddr(emac->getMACAddress());



	EV << "OLT ADDR = " << onutbl->getOLTAddr() << endl;

    MAX_NB_CHANNELS = 100;
    queueLimit = par("queueLimit");

    if (!queue_mod)
    	error("ONUMacCtl: A IPassiveQueue is needed above mac control");

    if ( ! onutbl)
    	error("OLTMacCtl - onutbl: null, problem.");

    // Init channels
    for (int i= 0; i < MAX_NB_CHANNELS; ++i)
    {
    	channels[i] = NULL;
    }
}

OLTChannelInfos* OLTMacCtl::getChannel(int ch)
{
	if (channels[ch] == NULL)
	{
		// Not allocated/init yet:
		channels[ch] = new OLTChannelInfos();

		channels[ch]->queue = NULL;

		if (queuingStrategy == "fifo")
		{
			channels[ch]->queue = new SimpleQueue();
		}
		else
		if (queuingStrategy == "admissionControl")
		{
			FiWiFastAdmissionControl* qAdmissionControl = new FiWiFastAdmissionControl();

			qAdmissionControl->setConfFile(admissionControlConfs);

			channels[ch]->queue = qAdmissionControl;
		}
		else
		if (queuingStrategy == "myWeightedFairQueue")
		{
			MyWeightedFairQueue* q = new MyWeightedFairQueue();


			channels[ch]->queue = q;
		}

		if (channels[ch]->queue == NULL)
		{
			error("ONUQPerLLiDBase::initialize: Unknown queue type!");
		}

		channels[ch]->queue->setLimit(queueLimit);
		channels[ch]->transmitState = TX_IDLE;
		channels[ch]->txEnd = new cMessage("TxEnd",TXENDMSG);
		channels[ch]->id = ch;
	}

	return channels[ch];
}

OLTChannelInfos* OLTMacCtl::findLeastBusyChannel(MACAddress dest)
{
	int minSizeBytes = 1000000000;
	OLTChannelInfos* chanMin = NULL;

	pair<bool, FiWiNode> resultNextHop = MyUtil::getRoutingTable(this)->nextHop(MyUtil::getRoutingTable(this)->getOltAddr(), dest, FIWI_NODE_TYPE_ANY);

	MACAddress onuAddr = resultNextHop.second.addr;

	vector<int> tmpChannels = onutbl->wavelengths();

	for (int i = 0; i < (int)tmpChannels.size(); ++i)
		getChannel(tmpChannels[i]);

	for (int i= 0; i < MAX_NB_CHANNELS; ++i)
	{
		if ( ! channels[i])
		{
			EV << "SKIPPING i = " << i << endl;
			continue;
		}

		EV << "OLTMacCtl::findLeastBusyChannel - channel = " << channels[i]->id << endl;

		int curQueueSize = channels[i]->queue->lengthInBytes();

		EV << "OLTMacCtl::findLeastBusyChannel - curQueueSize = " << curQueueSize << endl;

		// Check if the destination has this channel and WDM enabled
		ONUTableEntry* entry = onutbl->getEntry(onuAddr, channels[i]->id);

		EV << "OLTMacCtl::findLeastBusyChannel - resultNextHop = " << resultNextHop.first << endl;
		EV << "OLTMacCtl::findLeastBusyChannel - resultNextHop 3 = " << resultNextHop.second.addr << endl;
		EV << "OLTMacCtl::findLeastBusyChannel - resultNextHop 4 = " << resultNextHop.second.nodeType << endl;

		EV << "OLTMacCtl::findLeastBusyChannel - entry = " << entry << endl;
		EV << "OLTMacCtl::findLeastBusyChannel - dest = " << dest << endl;
		// EV << "OLTMacCtl::findLeastBusyChannel - entry->getWdmEnabled() = " << entry->getWdmEnabled() << endl;
		EV << "OLTMacCtl::findLeastBusyChannel - chanMin = " << chanMin << endl;

		if ((entry && curQueueSize <= minSizeBytes))
		{
			EV << "OK FOR " << i << endl;
			minSizeBytes = curQueueSize;
			chanMin = channels[i];
		}
	}

	return chanMin;
}

void OLTMacCtl::handleMessage(cMessage *msg)
{
	// Self Message
	if (msg->isSelfMessage())
	{
		EV << "Self-message " << msg << " received\n";

		for (int i = 0; i < MAX_NB_CHANNELS; ++i)
		{
			if ( ! channels[i])
			{
				continue;
			}

			if (msg == channels[i]->txEnd)
			{
				handleTxEnd(channels[i]);
			}
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
	else
	if (ingate->getId() ==  gate( "upperLayerIn")->getId())
	{
		processFrameFromHigherLayer(msg);
	}
	else
	{
		EV << "OLTMacCtl: Message came FROM THE WRONG DIRRECTION???? Dropping\n";
		delete msg;
	}
}

void OLTMacCtl::processFrameFromHigherLayer(cMessage *msg)
{
	EV << "OLTMacCtl: Incoming to PON area...\n";
	EtherFrame * frame = dynamic_cast<EtherFrame *>(msg);

	if ( ! frame)
	{
		error("OLTMacCtl::processFrameFromHigherLayer frame problem.");
	}

	EV << "PEV ici 1..." << endl;

	int channel = 0;

	// FROM OLT -> ONU delay recording!!!
	frame->setInternalTimestamp(simTime().raw());
	// End

	if (PONUtil::getPonVersion(this) == 1)
	{
		EV << "PEV ici 2..." << endl;
		EV << "OLTMacCtl::processFrameFromHigherLayer 1" << endl;

		if (dynamic_cast<EthernetIIFrame *>(msg) && dynamic_cast<EthernetIIFrame *>(msg)->getEtherType() == MPCP_TYPE)
		{
			EV << "OLTMacCtl::processFrameFromHigherLayer 2" << endl;
			// TDM, 0
			channel = 0;
		}
		else
		{
			EV << "OLTMacCtl::processFrameFromHigherLayer 3" << endl;
			// Take the least busy channel
			OLTChannelInfos* chan = findLeastBusyChannel(frame->getDest());
			EV << "OLTMacCtl::processFrameFromHigherLayer 4" << endl;

			if ( ! chan)
			{
				error("OLTMacCtl::processFrameFromHigherLayer - problem find least busy case pon version = 1, dest = %s", frame->getDest().str().c_str());
			}

			channel = chan->id;
			EV << "OLTMacCtl::processFrameFromHigherLayer 5" << endl;
		}
	}
	else
	if (PONUtil::getPonVersion(this) == 2)
	{
		if (dynamic_cast<EthernetIIFrame *>(msg) && dynamic_cast<EthernetIIFrame *>(msg)->getEtherType() == MPCP_TYPE)
		{
			channel = 0;
		}
		else
		{
			EV << "PEV ici 3..." << endl;
			pair<bool, FiWiNode> resultNextHop = MyUtil::getRoutingTable(this)->nextHop(MyUtil::getRoutingTable(this)->getOltAddr(),
					frame->getDest(), FIWI_NODE_TYPE_ANY);

			MACAddress onuAddr = resultNextHop.second.addr;

			ONUInfoWR o = ONUTable::getONUInfoWROf(onuAddr);

			channel = o.channel;
		}
	}
	else
	{
		error("OLTMacCtl::processFrameFromHigherLayer PON version problem");
	}

	if (dynamic_cast<EthernetIIFrame *>(msg) && dynamic_cast<EthernetIIFrame *>(msg)->getEtherType() == MPCP_TYPE)
	{
		MPCP * mpcp = check_and_cast<MPCP *>(msg);
		mpcp->setTs(MPCPTools::simTimeToNS16());
	}

	OLTChannelInfos* channelInfos = getChannel(channel);

	if ( ! channelInfos)
	{
		error("OLTMacCtl::processFrameFromHigherLayer - channel infos problem (selection)");
	}

	EV << "OLTMacCtl::Enqueue Frame " << msg << " in channel " << channel << endl;
	channelInfos->queue->enqueue(check_and_cast<cPacket*>(msg));

	// handle tx end will react like
	// a transmission just finished ==
	// check the Q module and keep on sending.
	if (channelInfos->transmitState == TX_IDLE)
	{
		EV << "OLTMacCtl::We where IDLE... starting transmission - msg = " << msg << endl;
		handleTxEnd(channelInfos);
	}
	else
	{
		EV << "OLTMacCtl:: NOT IDLE... msg = " << msg << endl;
	}
}

void OLTMacCtl::processFrameFromMAC(cMessage *msg)
{
	EV << "OLTMacCtl: Message from PON area, forwarding to higher layer\n";

	EtherFrame * frame = dynamic_cast<EtherFrame *>(msg);

	if ( ! frame)
	{
		error("Problem OLTMacCtl::processFrameFromMAC");
	}

	if (simTime().dbl() > 2.0)
	{
		// Record the upstream delay!
		simtime_t delay;
		delay.setRaw(simTime().raw() - frame->getInternalTimestamp());
		FiWiGeneralConfigs::updatePONUpstreamDelay(delay.dbl());
	}

	// Not Control
	send(msg,"upperLayerOut");
	numFramesFromLL++;
}

/**
 * Handle transmissions and queue.
 */
void OLTMacCtl::doTransmit(OLTChannelInfos* channelInfos, cMessage * msg)
{
	cPacket * packet = check_and_cast<cPacket *>(msg);

	if ( ! packet)
			error("Queue module returned NULL frame");

	// Calculate TX and remaining time
	// NOTE: Change here for more bandwidth

	uint32_t nextMsgSize =  packet->getByteLength();

	if (nextMsgSize == 0)
	{
		error("Message size 0");
	}

	uint32_t bytes= nextMsgSize;

	// TODO add overhead parameter
	// bytes += PREAMBLE_BYTES + SFD_BYTES;
	// bytes+=INTERFRAME_GAP_BITS / 8;

	// TODO: Add laser on/off delay
	simtime_t timereq= ((double)(bytes*8) / PONUtil::getCapacityOf(this, channelInfos->id));

	EV << "Bytes: "<<bytes<<" bits: "<<bytes*8<<" TX RATE: "<<PONUtil::getCapacityOf(this, channelInfos->id)<<endl;
	EV << "TX State: " << channelInfos->transmitState << endl;
	EV << "Scheduled after " << (double)(bytes*8) / PONUtil::getCapacityOf(this, channelInfos->id) << "s"<<endl;

	if (channelInfos->transmitState == TX_IDLE)
	{
		channelInfos->transmitState= TX_SENDING;

		// Calculate the tx time
		EV << "EndTx Scheduled after "<< timereq.raw() << " time_now: "<<simTime().raw()<<endl;
		scheduleAt(simTime() + timereq, channelInfos->txEnd);
		EV << " Sending... to " << channelInfos->id << endl;

		EPON_LLidCtrlInfo *nfo=dynamic_cast<EPON_LLidCtrlInfo *>(msg->getControlInfo());

		if (nfo)
		{
			nfo->channel = channelInfos->id;
		}

		send(msg, "lowerLayerOut");
	}
	else
	{
		error ("OLTMacCtl: Packet Arrived from higher layer"
				"while we where in transmission. This should never happen"
				"(normally).");
	}
}


void OLTMacCtl::handleTxEnd(OLTChannelInfos* channelInfos)
{
	channelInfos->transmitState = TX_IDLE;

	if (channelInfos->queue->isEmpty())
	{
		// Nothing to transmit !
		return;
	}

	cPacket* pkt = channelInfos->queue->front(100000);
	channelInfos->queue->remove(pkt);

	doTransmit(channelInfos, dynamic_cast<cMessage *>(pkt));
}

// TOOLS
cModule * OLTMacCtl::getNeighbourOnGate(const char * g)
{
	return gate(g)->getNextGate()->getOwnerModule();
}
