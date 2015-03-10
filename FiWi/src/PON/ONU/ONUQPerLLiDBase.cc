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

#include "ONUQPerLLiDBase.h"
#include "StatsCollector.h"
#include "PONUtil.h"
#include "FiWiTrafGen.h"
#include "SimpleQueue.h"
#include "MyAbstractQueue.h"
#include "FiWiFastAdmissionControl.h"
#include "MyWeightedFairQueue.h"
#include "MyUtil.h"
#include "ONUTable.h"

ONUQPerLLiDBase::ONUQPerLLiDBase()
{
	regTOMsg = new cMessage("regTOMsg", REGTOMSG);
	regSendMsg = new cMessage("regSendMsg", REGSENDMSG);
}

ONUQPerLLiDBase::~ONUQPerLLiDBase(){
	cancelAndDelete(regSendMsg);
	cancelAndDelete(regTOMsg);
}

void ONUQPerLLiDBase::initialize(int stage)
{
	if (stage != 3)
		return;

	static int onuID = -1;
	++onuID;

	cModule* module = MyUtil::findModuleUp(this, "epon_mac");
	EPON_mac* mac = dynamic_cast<EPON_mac*>(module);

	int wavelength = par("forceWavelength");

	if ( ! mac)
	{
		error("ONUQPERLLIDBASE prob mac address");
	}

	ONUInfoWR onuInfos;
	onuInfos.addr = mac->getMACAddress();
	onuInfos.id = onuID;
	onuInfos.channel = wavelength;
	ONUTable::WRNodeInfos.push_back(onuInfos);

	// Parameters
	regTimeOut = par("regTimeOut");
	regTimeOut/=1000;
	queueLimit = par("queueLimit");
	wdmEnabled = par("wdmEnabled").boolValue();
	nbWDMChannels = par("nbWDMChannels");
	queuingStrategy = par("queuingStrategy").stringValue();
	admissionControlConfs = par("admissionControlConfs").stringValue();

	queue = NULL;

	if (queuingStrategy == "fifo")
	{
		queue = new SimpleQueue();
	}
	else
	if (queuingStrategy == "admissionControl")
	{
		FiWiFastAdmissionControl* qAdmissionControl = new FiWiFastAdmissionControl();

		qAdmissionControl->setConfFile(admissionControlConfs);

		queue = qAdmissionControl;
	}
	else
	if (queuingStrategy == "myWeightedFairQueue")
	{
		MyWeightedFairQueue* q = new MyWeightedFairQueue();

		queue = q;
	}

	if (queue == NULL)
	{
		error("ONUQPerLLiDBase::initialize: Unknown queue type!");
	}

	queue->setLimit(queueLimit);

	queue->initialize(this);

	nbPacketsReceivedFromHigherLayer = 0;

	dedicatedChannel = -1;
}

void ONUQPerLLiDBase::handleMessage(cMessage *msg)
{
	// Self Message
	if (msg->isSelfMessage())
	{
		EV << "Self-message " << msg << " received\n";

		if (msg == regTOMsg)
		{
			EV << "*** ONUMacCtl: Registration FAILED"<<endl;
		}
		else
		if (msg == regSendMsg)
			sendMPCPReg();
		else
		{
			error("ONUQPerLLiDBase::handleMessage err unknown message");
		}

		return;
	}

	// Network Message
	cGate *ingate = msg->getArrivalGate();
	EV << "Frame " << msg << " arrived on port " << ingate->getName() << "...\n";

	if (ingate->getId() ==  gate( "lowerLayerIn")->getId()){
		processFrameFromLowerLayer(msg);
	}
	else if (ingate->getId() ==  gate( "upperLayerIn")->getId()){
		// Add the frame to the proper Q
		processFrameFromHigherLayer(msg);
	}else{
		EV << "ONUMacCtl: Message came FROM THE WRONG DIRRECTION???? Dropping\n";
		delete msg;
	}
}

void ONUQPerLLiDBase::processFrameFromHigherLayer(cMessage *msg)
{
	cPacket *pkt = dynamic_cast<cPacket *>(msg);

	EV << "ONUQPerLLiDBase::processFrameFromHigherLayer - QUEUE LENGTH IN BYTES..." << queue->lengthInBytes() << " nb packets = " << queue->length() << endl;

	if (queue->isFull())
	{
		EV << "ONUQPerLLiDBase::processFrameFromHigherLayer - queue is full... dropping !" << endl;

		EtherFrame* frame = dynamic_cast<EtherFrame*>(pkt);

		if ( ! frame)
		{
			error("INVALID 2! processFrameFromHigherLayer");
		}

		if (frame->getPktType() == FIWI_TRAF_PKT_TYPE_BEST_EFFORT)
		{
			FiWiTrafGen::BestEffortDropRatio.addStat(simTime().dbl(), 0, 1);
		}
		else
		if (frame->getPktType() == FIWI_TRAF_PKT_TYPE_VIDEO)
		{
			FiWiTrafGen::VideoStreamsDropRatio.addStat(simTime().dbl(), 0, 1);
		}

		delete msg;
	}
	else
	{
		// Reset for upstream delay recording!!
		EtherFrame* f = dynamic_cast<EtherFrame*>(msg);

		if ( ! f)
		{
			error("ONUQPerLLiDBase::processFrameFromHigherLayer - Problem not etherframe ?");
		}

		f->setInternalTimestamp(simTime().raw());

		queue->enqueue(pkt);
	}
}

void ONUQPerLLiDBase::processFrameFromLowerLayer(cMessage *msg){
	EV << "ONUQPerLLiDBase: Incoming from lower layer...\n";

	EthernetIIFrame * frame = dynamic_cast<EthernetIIFrame *>(msg);

	if (frame && frame->getEtherType() == MPCP_TYPE)
	{
		processMPCP(frame );
		return;
	}
	else
	{

	}

	send(msg,"upperLayerOut");
}

void ONUQPerLLiDBase::processMPCP(EthernetIIFrame *frame ){
	EV << "ONUMacCtl: MPCP Frame processing\n";
	MPCP * mpcp = check_and_cast<MPCP *>(frame);


	switch (mpcp->getOpcode())
	{
		case MPCP_REGISTER:
		{
			MPCPRegAck *ack = new MPCPRegAck();
			MPCPRegister * reg = check_and_cast<MPCPRegister *>(frame);

			EV << "ONUMacCtl: Type is MPCP_REGISTER\n";
			cancelEvent(regTOMsg);
			EV << "ONUMacCtl: Canceling RegTOMsg\n";

			ack->setOpcode(MPCP_REGACK);
			ack->setName("MPCPAck");
			ack->setEtherType(MPCP_TYPE);
			ack->setDest(mpcp->getSrc());
			ack->setByteLength(MPCP_HEADER_LEN);

			if (reg->getLLIDsArraySize() == 1)
			{
				llid = reg->getLLIDs(0);
			}

			// ack->set

			send(ack,"lowerLayerOut");
			// Send the frame on top layer that manages LLIDs
			send(frame->dup(),"upperLayerOut");

			break;
		}
		case MPCP_GATE:
		{
			MPCPGate * gate = check_and_cast<MPCPGate *>(frame);
			EV << "ONUQPerLLiDBase: Type is MPCP_GATE\n";

			// Register Grant
			if (gate->getDest().isBroadcast())
			{
				EV << "ONUMacCtl: MPCP REGISTER GRANT (DOOUUU)" <<endl;
				// Process here number of llids...

				startMPCPReg();
				break;
			}

			break;
		}
		default:
			EV << "ONUMacCtl: Unrecognized MPCP OpCode!!\n";
			return;
	};

	delete frame;
}


void ONUQPerLLiDBase::sendMPCPReg()
{
	MPCPRegReq *regreq = new MPCPRegReq();
	regreq->setDest(MACAddress("FF:FF:FF:FF:FF:FF"));
	regreq->setEtherType(MPCP_TYPE);
	regreq->setName("MPCPRegReq");
	regreq->setOpcode(MPCP_REGREQ);
	regreq->setPtpNumReq(1);
	regreq->setWdmEnabled(this->wdmEnabled);

	regreq->setByteLength(MPCP_HEADER_LEN + MPCP_LIST_LEN);

	// Send REG REQ and Schedule a timeout
	send(regreq, "lowerLayerOut");
	scheduleAt(simTime() +regTimeOut, regTOMsg );
}

void ONUQPerLLiDBase::startMPCPReg()
{
	uint32_t rndBackOff = dblrand() * 100000000 / 16;
	EV << "ONUQPerLLiDBase: Sending MPCP REGREQ in " << rndBackOff << endl;
	simtime_t t;
	t.setRaw(simTime().raw() + MPCPTools::ns16ToSimTime(rndBackOff));

	EV << "WILL REGISTER AT " << t << endl;

	scheduleAt(t, regSendMsg );
}

cModule * ONUQPerLLiDBase::findModuleUp(const char * name)
{
	cModule *mod = NULL;

	for (cModule *curmod=this; !mod && curmod; curmod=curmod->getParentModule())
	     mod = curmod->getSubmodule(name);

	return mod;
}

void ONUQPerLLiDBase::finish()
{
	simtime_t t = simTime();

}

bool ONUQPerLLiDBase::isEmpty()
{
	return queue->isEmpty();
}

