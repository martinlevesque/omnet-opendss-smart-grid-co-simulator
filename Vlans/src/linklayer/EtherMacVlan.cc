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

#include "EtherMacVlan.h"

Define_Module(EtherMacVlan);

using namespace std;

void EtherMacVlan::processMsgFromNetwork(cPacket *msg)
{
	EV << "EtherMacVlan::processMsgFromNetwork frame size " << msg->getByteLength() << endl;

	// shoot it upper, damnit!
	send(msg, "upperLayerOut");
}

// ONLY TO CHANGE THE MAX ALLOWED FRAME
void EtherMacVlan::processFrameFromUpperLayer(EtherFrame *frame)
{
	send(frame, "phys$o");
	return;

	// Copied from EtherMACBase.cc ----------------------------------------------------------------------------
    EV << "Received frame from upper layer: " << frame << endl;

    if (frame->getDest().equals(address))
    {
        error("logic error: frame %s from higher layer has local MAC address as dest (%s)",
              frame->getFullName(), frame->getDest().str().c_str());
    }

    if (frame->getByteLength() > MAX_ETHERNET_FRAME_D1Q)
        error("packet from higher layer (%d bytes) exceeds maximum Ethernet frame size (%d)", (int)(frame->getByteLength()), MAX_ETHERNET_FRAME_D1Q);

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

        // store frame and possibly begin transmitting
        EV << "Packet " << frame << " arrived from higher layers, enqueueing\n";
        txQueue.insert(frame);
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


    // Copied from EtherMAC.cc ----------------------------------------------------------------------------
    if (!autoconfigInProgress && (duplexMode || receiveState==RX_IDLE_STATE) && transmitState==TX_IDLE_STATE)
	{
		EV << "No incoming carrier signals detected, frame clear to send, wait IFG first\n";
		scheduleEndIFGPeriod();
	}

}

/**
 * Override to clone packet name to the duplicated frame
 */
void EtherMacVlan::startFrameTransmission()
{
    cPacket *origFrame = (cPacket *)txQueue.front();
    EV << "Transmitting a copy of frame " << origFrame << endl;
    cPacket *frame = origFrame->dup();
    frame->setName(origFrame->getName());

    // add preamble and SFD (Starting Frame Delimiter), then send out
    frame->addByteLength(PREAMBLE_BYTES+SFD_BYTES);
    if (ev.isGUI())  updateConnectionColor(TRANSMITTING_STATE);
    send(frame, physOutGate);

    // update burst variables
    if (frameBursting)
    {
        bytesSentInBurst = frame->getByteLength();
        framesSentInBurst++;
    }

    // check for collisions (there might be an ongoing reception which we don't know about, see below)
    if (!duplexMode && receiveState!=RX_IDLE_STATE)
    {
        // During the IFG period the hardware cannot listen to the channel,
        // so it might happen that receptions have begun during the IFG,
        // and even collisions might be in progress.
        //
        // But we don't know of any ongoing transmission so we blindly
        // start transmitting, immediately collide and send a jam signal.
        //
        sendJamSignal();
        // numConcurrentTransmissions stays the same: +1 transmission, -1 jam

        if (receiveState==RECEIVING_STATE)
        {
            delete frameBeingReceived;
            frameBeingReceived = NULL;

            numCollisions++;
            numCollisionsVector.record(numCollisions);
        }
        // go to collision state
        receiveState = RX_COLLISION_STATE;
    }
    else
    {
        // no collision
        scheduleEndTxPeriod(frame);

        // only count transmissions in totalSuccessfulRxTxTime if channel is half-duplex
        if (!duplexMode)
            channelBusySince = simTime();
    }
}


