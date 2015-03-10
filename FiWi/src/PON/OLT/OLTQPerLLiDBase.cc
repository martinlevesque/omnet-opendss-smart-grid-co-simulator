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

#include "OLTQPerLLiDBase.h"
#include "EPON_mac.h"
#include "ONUMacCtl.h"
#include <vector>
#include "PONUtil.h"
#include "MyUtil.h"
#include <utility>
#include "OLTMacCtl.h"

using namespace std;

OLTQPerLLiDBase::OLTQPerLLiDBase(){}

OLTQPerLLiDBase::~OLTQPerLLiDBase()
{
	for (uint32_t i=0; i<pendingAck.size(); i++)
	{
		cancelAndDelete((cMessage *)pendingAck[i]);
	}
}

void OLTQPerLLiDBase::initialize(int stage)
{
	if (stage != 6)
		return;

	EV << "OLTQPerLLiDBase::initialize OK." << endl;

	// ONU table
	onutbl = NULL;
	onutbl = dynamic_cast<ONUTable *>( findModuleUp("onuTable"));

	if (!onutbl)
		error("Shit... no ONU table found ?!?!");

	oltMac = dynamic_cast<OLTMacCtl *>( findModuleUp("oltMacCtl"));

	if ( ! oltMac)
	{
		error("God damn, no olt mac");
	}

	// Parameters
	regAckTimeOut = par("regAckTimeOut");
	regAckTimeOut/=1000;

	// Convert to ns16 time
	// (to be added in the MPCP frames)
	regTimeInt = par("regTimeInt");
	regTimeInt*=(double)1000 / 16;
	queueLimit = par("queueLimit");

	nbWDMChannels = par("nbWDMChannels");
	nbONUs = par("nbONUs");

	// TO CLEAN, parameter.
	propagationDelay = par("propagationDelay");

	maximumGrantSize = par("maximumGrantSize");

	if (maximumGrantSize == 0)
	{
		// Infinite
		maximumGrantSize = 1000000000 / 8;
	}

	EV << "PEV hmm 2" << endl;
	// Ask to register at the beginning
	sendMPCPGateReg();

	maximumQueueDurationControl = oltMac->maximumQueueDuration(0);

	EV << "OLTQPerLLiDBase::initialize maximumQueueDurationControl = " << maximumQueueDurationControl << endl;
}

void OLTQPerLLiDBase::handleMessage(cMessage *msg)
{

	// Self Message
	if (msg->isSelfMessage())
	{
		EV << "Self-message " << msg << " received\n";

		if (msg->getKind() == ONUPENACK)
			handleRegTimeOut(msg);
		else
			EV << "UnKnown Self Message\n";

		return;
	}

	// Network Message
	cGate *ingate = msg->getArrivalGate();
	EV << "Frame " << msg << " arrived on port " << ingate->getName() << "...\n";

	if (ingate->getId() ==  gate( "lowerLayerIn")->getId())
	{
		processFrameFromLowerLayer(msg);
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

void OLTQPerLLiDBase::processFrameFromHigherLayer(cMessage *msg)
{
	EV << "OLT_Q_mgmt_PerLLiD: Incoming from higher layer...\n";

	// Should proccess via a queue
	send(msg, "lowerLayerOut");
}

void OLTQPerLLiDBase::processFrameFromLowerLayer(cMessage *msg)
{
	EV << "OLT_Q_mgmt_PerLLiD: Incoming from lower layer" << endl;

	EthernetIIFrame * frame = dynamic_cast<EthernetIIFrame *>(msg);

	if (frame && frame->getEtherType() == MPCP_TYPE)
	{
		processMPCP(frame );
		return;
	}

	EtherFrame* dataFrame = dynamic_cast<EtherFrame *>(msg);

	if ( ! dataFrame)
	{
		error("OLTQPerLLiDBase::processFrameFromLowerLayer invalid frame ? ");
	}

	// Here we need to check WHERE to forward the packet.
	if (dataFrame && (dataFrame->getDest() == MyUtil::getRoutingTable(this)->getOltAddr() ||
			dataFrame->getDest() == MyUtil::getRoutingTable(this)->getDMSAddr()))
	{
		send(msg,"upperLayerOut");
	}
	else
	{
		send(msg, "lowerLayerOut");
	}
}

/**
 * Do MPCP stuff
 */
void OLTQPerLLiDBase::processMPCP(EthernetIIFrame *frame )
{
	EV << "OLTQPerLLiDBase: MPCP Frame processing" << endl;
	MPCP * mpcp = check_and_cast<MPCP *>(frame);

	switch (mpcp->getOpcode())
	{
		case MPCP_REGREQ:
		{
			MPCPRegReq * req = check_and_cast<MPCPRegReq *>(frame);
			MPCPRegister *rspframe = new MPCPRegister();

			rspframe->setOpcode(MPCP_REGISTER);
			rspframe->setName("MPCPRegister");
			rspframe->setEtherType(MPCP_TYPE);
			rspframe->setDest(req->getSrc());
			rspframe->setPtpNumReg(req->getPtpNumReq());

			EV << "OLTQPerLLiDBase: Log the LLIDs temporarly and add a TO timer\n";

			ONUTableEntry te;
			te.setId(req->getSrc());
			te.setWdmEnabled(req->getWdmEnabled());

			EPON_LLidCtrlInfo* nfo = dynamic_cast<EPON_LLidCtrlInfo *>(frame->getControlInfo());

			rspframe->setControlInfo(new EPON_LLidCtrlInfo(LLID_EPON_BC, 0));

			// Parse and set LLIDS
			rspframe->setLLIDsArraySize(req->getPtpNumReq());

			for (int i=0; i<req->getPtpNumReq(); i++)
			{
				// 0xFFF = 4095 intrand -> [0-4095)
				uint32_t llid_tmp=(uint32_t)intrand(4095);

				while (te.addLLID(llid_tmp) <0 )
				{
					EV << "*** RANDOM GEN FAILED!!!!???\n";
					llid_tmp=(uint32_t)intrand(4095);
				}

				rspframe->setLLIDs(i,llid_tmp);
			}

			rspframe->setByteLength(MPCP_HEADER_LEN+MPCP_LIST_LEN+req->getPtpNumReq()*MPCP_LLID_LEN);

			//temptbl.addONU(te);
			temptbl.push_back(te);

			// NOTE: do NOT send... enqueue
			//send(rspframe,"lowerLayerOut");
			processFrameFromHigherLayer(rspframe);

			// Create and add TO self message
			cMessage *tmpmsg=new cMessage("OnuTOMsg", ONUPENACK);
			pendingAck.push_back(tmpmsg);
			scheduleAt(simTime()+regAckTimeOut, tmpmsg);

			break;
		}
		case MPCP_REGACK:
		{
			EV << "OLTQPerLLiDBase: ML received ack at olt." << endl;
			doOnuRegistration(mpcp->getSrc());

			break;
		}
		case MPCP_REPORT :
		{
			EV << "OLTQPerLLiDBase: received REPORT" << endl;

			error("OLTQPerLLiDBase::processMPCP - MPCP_REPORT: it should not happen anymore.");

			MPCPReport* req = check_and_cast<MPCPReport *>(frame);

			if ( ! req)
			{
				error("OLTQPerLLiDBase::processMPCP: unrecognized report packet");
			}

			EV << "CHANGING PARAMETERS OLTQPerLLiDBase::processMPCP - src = " << mpcp->getSrc() << ", length slot = " << req->getLengthSlot() << endl;

			DoUpstreamDBA(mpcp->getSrc(), req->getLengthSlot());

			break;
		}
		default:
			EV << "Unrecognized MPCP OpCode";
	};

	delete frame;
}


void OLTQPerLLiDBase::doOnuRegistration(MACAddress mac)
{
	EV << "OLTQPerLLiDBase::doOnuRegistration 1" << endl;

	// Find Entry Index....
	for (uint32_t i=0; i<temptbl.size(); i++)
	{
		if (temptbl.at(i).getId() == mac)
		{
			EV << "OLTMacCtl: ONU (MAC: "<<mac<<") request "<< i << " Registered"<<endl;
			// Clear message and move tb entry to the global
			cancelAndDelete(pendingAck[i]);
			pendingAck.erase(pendingAck.begin()+i);

			// Copy
			ONUTableEntry en=temptbl.at(i);

			// For all WDM
			for (uint32_t ch = 0; ch <= (uint32_t)nbWDMChannels; ++ch)
			{
				if (ch > 0 && ! en.getWdmEnabled())
				{
					break;
				}

				en.setChannel(ch);
				onutbl->addONU(en);
				onutbl->previousONUQueueLengths[mac] = 1500;
			}

			// Remove from tmp
			//temptbl.removeONU(i);
			temptbl.erase(temptbl.begin()+i);

			// OLT <-> ONU
			MyUtil::getRoutingTable(this)->addAdjacent(FiWiNode(onutbl->getOLTAddr(), FIWI_NODE_TYPE_PON),
					FiWiNode(mac, FIWI_NODE_TYPE_PON));

			MyUtil::getRoutingTable(this)->print();

			break;
		}
	}
}

void OLTQPerLLiDBase::handleRegTimeOut(cMessage *msg)
{
	// Find Message Index....
	for (uint32_t i=0; i<pendingAck.size(); i++)
	{
		if (pendingAck[i] == msg)
		{
			EV << "OLTMacCtl: ONU request "<< i << " TimeOut"<<endl;
			// Clear message and tb entry
			cancelAndDelete((cMessage *)pendingAck[i]);
			pendingAck.erase(pendingAck.begin()+i);
			//temptbl.removeONU(i);
			temptbl.erase(temptbl.begin()+i);
		}
	}
}

std::vector<ONUReservation> OLTQPerLLiDBase::DoUpstreamDBA(MACAddress onuAddr, int newSlotLengthInBytes)
{
	std::vector<ONUReservation> reservationsBurst;
	ONUReservation reservationReport;

	EV << "OLTQPerLLiDBase::DoUpstreamDBA - initialization" << endl;
	onutbl->print();

	EV << "reservations before clean:" << endl;
	onutbl->printReservations();

	onutbl->cleanReservations();
	EV << "reservations after clean:" << endl;
	onutbl->printReservations();

	std::vector<uint8_t> noChannelToAvoid;

	EV << "newSlotLengthInBytes = " << newSlotLengthInBytes << endl;

	onutbl->previousONUQueueLengths[onuAddr] = newSlotLengthInBytes;

	if (newSlotLengthInBytes <= 0)
	{
		vector<ONUReservation> res;

		return res;
	}

	//////////////
	// BURST
	int wdmEnabled = onutbl->getEntry(onuAddr, 0)->getWdmEnabled();
	int nbMaxReservations = (wdmEnabled && nbWDMChannels > 0) ? (nbWDMChannels + 1) : 1;

	std::vector<uint8_t> channelsToAvoid;

	if (PONUtil::getPonVersion(this) == 2)
	{
		nbMaxReservations = 1;

		// With AWG-based PON, we can only send using a given lambda!
		ONUInfoWR o = ONUTable::getONUInfoWROf(onuAddr);

		vector<int> wavelengths = onutbl->wavelengths();

		for (int i = 0; i < (int)wavelengths.size(); ++i)
		{
			if (wavelengths[i] != o.channel)
			{
				channelsToAvoid.push_back(wavelengths[i]);
			}
		}
	}

	onutbl->previousONUQueueLengths[onuAddr] = newSlotLengthInBytes;

	if (newSlotLengthInBytes > maximumGrantSize * nbMaxReservations)
		newSlotLengthInBytes = maximumGrantSize * nbMaxReservations;

	int shouldAllocate = onutbl->calculatesAllocationTo(onuAddr, maximumGrantSize * nbMaxReservations);

	onutbl->previousONUQueueLengths[onuAddr] = shouldAllocate;

	EV << "UPSTREAM DBA " << onuAddr << " should allocate -> " << shouldAllocate << endl;

	// Allocate for the burst
	ONUReservation r = onutbl->reserve((simTime()).raw(), shouldAllocate, false, onuAddr, channelsToAvoid, true);

	reservationsBurst.push_back(r);

	EV << "OLTQPerLLiDBase::DoUpstreamDBA - initialization" << endl;
	onutbl->print();

	EV << "reservations at end:" << endl;
	onutbl->printReservations();

	return reservationsBurst;
}

MPCPGate * OLTQPerLLiDBase::makeRegPacket()
{
	MPCPGate *gt = new MPCPGate();
	gt->setEtherType(MPCP_TYPE);
	gt->setOpcode(MPCP_GATE);
	gt->setName("MPCPGate(Reg)");

	gt->setDurationReport(regTimeInt);
	gt->setDest(MACAddress::BROADCAST_ADDRESS);


	// Header + (start report + Len report) + (start burst + Len burst)
	gt->setByteLength(sizeGateMsgInBytes(0));

	return gt;
}

int OLTQPerLLiDBase::sizeGateMsgInBytes(int nbSlots)
{
	/*
	int64 startTimeReport;
    uint8_t durationReport;
    uint8_t nbSlots;
    int64 startTimeBurst[];
    uint16_t durationBurst[];
    uint8_t channelBurst[];
	    */
	return MPCP_HEADER_LEN + (64 + 8 + 8 + nbSlots * (64 + 16 + 8)) / 8.0;
}

void OLTQPerLLiDBase::sendMPCPGateReg()
{
	MPCPGate *gt = makeRegPacket();

	gt->setControlInfo(new EPON_LLidCtrlInfo(LLID_EPON_BC, 0) );

	EV << "PEV OK set channel to 0" << endl;

	send(gt, "lowerLayerOut");
}

cModule * OLTQPerLLiDBase::findModuleUp(const char * name)
{
	cModule* mod = NULL;

	for (cModule *curmod=this; !mod && curmod; curmod=curmod->getParentModule())
	     mod = curmod->getSubmodule(name);

	return mod;
}

void OLTQPerLLiDBase::finish()
{

}
