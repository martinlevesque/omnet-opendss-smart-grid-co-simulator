/*
 * Copyright (C) 2013 Martin Lévesque <levesque.martin@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include <omnetpp.h>
#include "LinklessMAC.h"
#include "EtherMAC.h"
#include "Ieee802Ctrl_m.h"
#include "IPassiveQueue.h"

using namespace std;

Define_Module (LinklessMAC);

LinklessMAC::LinklessMAC()
{
}
LinklessMAC::~LinklessMAC()
{
	for (int i=0; i<txQueue.length(); i++)
		delete txQueue.pop();
}

void LinklessMAC::initialize()
{
	//paramTxrate = par("txRatePerChannel");

	//txrate = 1000000000;

    EtherMACBase::initialize();

    duplexMode = true;
    calculateParameters();

    beginSendFrames();

    withOverhead = false;
}

void LinklessMAC::initializeTxrate()
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
    	txrate = 10000000000;
    }
}

// TODO ML, doit avoir plusieurs queue, 1 par channel, modifier handle end tx period !!
void LinklessMAC::handleMessage(cMessage *msg)
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
void LinklessMAC::startFrameTransmission()
{
	/*
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
    */

}

void LinklessMAC::scheduleEndTxPeriod(cPacket *frame)
{
	EtherMACBase::scheduleEndTxPeriod(frame);
}

void LinklessMAC::processFrameFromUpperLayer(EtherFrame *frame)
{
	EV << "Received frame from upper layer: " << frame << endl;

	if (frame->getDest().equals(address))
	{
		error("logic error: frame %s from higher layer has local MAC address as dest (%s)",
			  frame->getFullName(), frame->getDest().str().c_str());
	}

	EV << "EPON from UP src addr = " << frame->getSrc() << " dest = " << frame->getDest() << endl;

	send(frame, physOutGate);

}

void LinklessMAC::processMsgFromNetwork(cPacket *msg)
{
    //EtherMACBase::processMsgFromNetwork(msg);
	// the original ^ checks for frame type and for cable length...
	// both not applicable here...

	 EtherFrame *ethframe = dynamic_cast<EtherFrame *>(msg);

	 if ( ! ethframe)
	 {
		 return;
	 }

	send(ethframe, "upperLayerOut");

}

void LinklessMAC::handleEndIFGPeriod()
{
    EtherMACBase::handleEndIFGPeriod();

    startFrameTransmission();
}

void LinklessMAC::handleEndTxPeriod()
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

void LinklessMAC::updateHasSubcribers()
{
    hasSubscribers = nb->hasSubscribers(NF_PP_TX_BEGIN) ||
                     nb->hasSubscribers(NF_PP_TX_END) ||
                     nb->hasSubscribers(NF_PP_RX_END);
}

void LinklessMAC::finish()
{
}

void LinklessMAC::finalize()
{
}


