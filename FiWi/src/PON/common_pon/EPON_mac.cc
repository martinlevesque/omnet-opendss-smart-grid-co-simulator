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

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "EPON_mac.h"
#include "IPassiveQueue.h"
#include "NotificationBoard.h"
#include "NotifierConsts.h"
#include "EPON_CtrlInfo.h"

Define_Module(EPON_mac);

EPON_mac::EPON_mac()
{
}
EPON_mac::~EPON_mac()
{
	for (int i=0; i<txQueue.length(); i++)
		delete txQueue.pop();
}

void EPON_mac::initialize()
{
	paramTxrate = par("txRatePerChannel");

    EtherMACBase::initialize();

    duplexMode = true;
    calculateParameters();

    beginSendFrames();

    withOverhead = false;
}

void EPON_mac::initializeTxrate()
{
    // if we're connected, find the gate with transmission rate
    txrate = 0;

    if (connected)
    {
        // obtain txrate from channel. As a side effect, this also asserts
        // that the other end is an EPON_mac, since normal EtherMAC
        // insists that the connection has *no* datarate set.
        // if we're connected, get the gate with transmission rate
    	EV << "EPON_mac: connected.. before data chan check 1" << endl;
    	/* BEFORE
        cChannel *datarateChannel = physOutGate->getTransmissionChannel();
        txrate = datarateChannel->par("datarate").doubleValue();
        */

    	// new
    	txrate = paramTxrate;
    }
}

// TODO ML, doit avoir plusieurs queue, 1 par channel, modifier handle end tx period !!
void EPON_mac::handleMessage(cMessage *msg)
{
    if (!connected)
        processMessageWhenNotConnected(msg);
    else if (disabled)
        processMessageWhenDisabled(msg);
    else if (msg->isSelfMessage())
    {
        EV << "Self-message " << msg << " received\n";

        if (msg == endTxMsg)
            handleEndTxPeriod();
        else if (msg == endIFGMsg)
            handleEndIFGPeriod();
        else if (msg == endPauseMsg)
            handleEndPausePeriod();
        else
            error("Unknown self message received!");
    }
    else
    {
        if (msg->getArrivalGate() == gate("upperLayerIn"))
            processFrameFromUpperLayer(check_and_cast<EtherFrame *>(msg));
        else if (msg->getArrivalGate() == gate("phys$i")){
        	processMsgFromNetwork(check_and_cast<cPacket *>(msg));
        }else
            error("Message received from unknown gate!");
    }

    if (ev.isGUI())  updateDisplayString();
}

/**
 * When Tx starts we have already added Logical
 * Link Identifier.
 */
void EPON_mac::startFrameTransmission()
{
	EtherFrameWithLLID *origFrame = (EtherFrameWithLLID *)txQueue.front();
    EV << "Transmitting a copy of frame " << origFrame << endl;

    EtherFrameWithLLID *frame = (EtherFrameWithLLID *) origFrame->dup();

    // TODO
    frame->setChannel(0);

    if (hasSubscribers)
    {
        // fire notification
        notifDetails.setPacket(frame);
        nb->fireChangeNotification(NF_PP_TX_BEGIN, &notifDetails);
    }

    // fill in src address if not set
//    if (frame->getSrc().isUnspecified())
//        frame->setSrc(address);

    // send
    EV << "Starting transmission of " << frame << endl;
    send(frame, physOutGate);
    scheduleEndTxPeriod(frame);

    // update burst variables
    if (frameBursting)
    {
        bytesSentInBurst = frame->getByteLength();
        framesSentInBurst++;
    }

}

void EPON_mac::scheduleEndTxPeriod(cPacket *frame)
{
	EtherMACBase::scheduleEndTxPeriod(frame);
}

void EPON_mac::processFrameFromUpperLayer(EtherFrame *frame)
{
	EV << "Received frame from upper layer: " << frame << endl;

	if (frame->getDest().equals(address))
	{
		error("logic error: frame %s from higher layer has local MAC address as dest (%s)",
			  frame->getFullName(), frame->getDest().str().c_str());
	}

	EV << "EPON from UP src addr = " << frame->getSrc() << " dest = " << frame->getDest() << endl;

	if (frame->getByteLength() > MAX_ETHERNET_FRAME_D1Q)
		error("packet from higher layer (%d bytes) exceeds maximum Ethernet frame size (%d)", frame->getByteLength(), MAX_ETHERNET_FRAME_D1Q);

	// must be EtherFrame (or EtherPauseFrame) from upper layer
	bool isPauseFrame = (dynamic_cast<EtherPauseFrame*>(frame)!=NULL);
	if (!isPauseFrame)
	{
		numFramesFromHL++;

		if (txQueueLimit && txQueue.length()>txQueueLimit)
			error("txQueue length exceeds %d -- this is probably due to "
				  "a bogus app model generating excessive traffic "
				  "(or if this is normal, increase txQueueLimit!)",
				  txQueueLimit);

		// fill in src address if not set
		if (frame->getSrc().isUnspecified())
			frame->setSrc(address);


		 // Note: take the control from the origial frame. Dup() does not clone it.
		EPON_LLidCtrlInfo *nfo=dynamic_cast<EPON_LLidCtrlInfo *>(frame->getControlInfo());
		EtherFrameWithLLID * llid_eth = new EtherFrameWithLLID();
		llid_eth->setName(frame->getName());

		// test

		if (nfo != NULL){
			EV << "Control Info FOUND"<<endl;
			EV << "pretty ok nfo channel ! " << nfo->channel << endl;

			llid_eth->setChannel(nfo->channel);

			// Convert to EtherFrameWithLLID

			uint16_t tmpLlid = nfo->llid;
			// Relay didn't knew the llid...
			if (tmpLlid == -1)
				tmpLlid=LLID_EPON_BC;
			llid_eth->setLlid(tmpLlid);
		}else{
	    	// MPCP Frames may not have LLIDs
	    	// Encapsulate BC
			uint16_t tmpLlid = LLID_EPON_BC;
			llid_eth->setLlid(tmpLlid);
	    }


		/*
		 * YOU HAVE TO MANUALLY SET THE BYTE LENGTH...
		 */

		// #define PREAMBLE_BYTES               7
		// #define EPON_PREAMBLE_BYTES (PREAMBLE_BYTES - 2)

		if (withOverhead)
		{
			llid_eth->addByteLength(2); 	// add for the 2 bytes in the preamble
			frame->addByteLength(EPON_PREAMBLE_BYTES+SFD_BYTES); // add the rest
		}

		llid_eth->encapsulate(frame);
		//frame=NULL;

		// store frame and possibly begin transmitting
		EV << "Packet " << llid_eth << " arrived from higher layers\n";
		EV << "After enc Size " << llid_eth->getByteLength()<< " \n";
		EV << "Orig. Size " << frame->getByteLength()<< " \n";


		// CHANGED ML
		// txQueue.insert(llid_eth);
		send(llid_eth, physOutGate);

		// TODO
/*
		if (frameBursting)
		    {
		        bytesSentInBurst = frame->getByteLength();
		        framesSentInBurst++;
		    }
*/

	}
	else
	{
		EV << "PAUSE received from higher layer\n";

		// PAUSE frames enjoy priority -- they're transmitted before all other frames queued up
		if (!txQueue.empty())
			txQueue.insertBefore(txQueue.front(), frame);  // front() frame is probably being transmitted
		else
			txQueue.insert(frame);
	}


	// In any case, wait for IFG before transmitting
	/*
    if (transmitState == TX_IDLE_STATE)
    	scheduleEndIFGPeriod();
	*/
}

void EPON_mac::processMsgFromNetwork(cPacket *msg)
{
    //EtherMACBase::processMsgFromNetwork(msg);
	// the original ^ checks for frame type and for cable length...
	// both not applicable here...

	 EtherFrame *ethframe = dynamic_cast<EtherFrame *>(msg);
	 EtherFrameWithLLID * ethllid= dynamic_cast<EtherFrameWithLLID *>(msg);

	if ( ethframe== NULL && ethllid== NULL)
	{
		error("EPON_mac: Unrecognized Frame... Shit happens");
	}

	// IF we have EPON frame with LLID de-capsulate
	if (ethllid)
	{
		ethframe = check_and_cast<EtherFrame *>(ethllid->decapsulate());
		delete ethllid->decapsulate();
		// Add Control Information to it
		EV << "IT HAD LLID INFO ... : "<<ethllid->getLlid() << endl;


		if (ethframe)
			EV << "EPON from NET src addr = " << ethframe->getSrc() << " dest = " << ethframe->getDest() << endl;

		// if (ethframe->getControlInfo()==NULL)
		// ethframe->setControlInfo(new EPON_LLidCtrlInfo(ethllid->getLlid(), 0) ); // TODO copy channel
		// Add original pre-amble.. to be used from the base class...

		if (withOverhead)
		{
			ethframe->addByteLength(-EPON_PREAMBLE_BYTES);
			ethframe->addByteLength(PREAMBLE_BYTES);
		}

		// Get rid of the original
		delete msg;
	}

	if ( ! withOverhead)
	{
		send(ethframe, "upperLayerOut");
		return;
	}

    if (hasSubscribers)
    {
        // fire notification
        notifDetails.setPacket(ethframe);
        nb->fireChangeNotification(NF_PP_RX_END, &notifDetails);
    }

    if (checkDestinationAddress(ethframe))
        frameReceptionComplete(ethframe);
}

void EPON_mac::handleEndIFGPeriod()
{
    EtherMACBase::handleEndIFGPeriod();

    startFrameTransmission();
}

void EPON_mac::handleEndTxPeriod()
{
    if (hasSubscribers)
    {
        // fire notification
        notifDetails.setPacket((cPacket *)txQueue.front());
        nb->fireChangeNotification(NF_PP_TX_END, &notifDetails);
    }

    if (checkAndScheduleEndPausePeriod())
        return;


    // Deletes the frame... and log stats
    EtherMACBase::handleEndTxPeriod();


    beginSendFrames();
}

void EPON_mac::updateHasSubcribers()
{
    hasSubscribers = nb->hasSubscribers(NF_PP_TX_BEGIN) ||
                     nb->hasSubscribers(NF_PP_TX_END) ||
                     nb->hasSubscribers(NF_PP_RX_END);
}

void EPON_mac::finish()
{
}

void EPON_mac::finalize()
{
}






