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

#include "EPON_CtrlInfo.h"
#include "PON_Splitter.h"
#include "Lambda.h"
#include "PONUtil.h"
#include <string>

using namespace std;

Define_Module(PON_Splitter);

void PON_Splitter::initialize()
{
    numMessages = 0;
    previousMsg = 0;
    WATCH(numMessages);

    ports = gateSize("portg");

    // Put the gates on instant transmission
    // (or suffer from collisions)
    gate("portu$i")->setDeliverOnReceptionStart(true);

    for (int i=0; i<ports; i++)
    	gate("portg$i",i)->setDeliverOnReceptionStart(true);
}

void PON_Splitter::handleMessage(cMessage *msg)
{
	// Handle frame sent down from the network entity: send out on every other port
	cGate *ingate = msg->getArrivalGate();
	string inGateName = ingate->getName();
	EV << "PON_Splitter: Frame " << msg << " arrived on port " << ingate->getName() << "...\n";

	EtherFrameWithLLID* opticalFrame = dynamic_cast<EtherFrameWithLLID*>(msg);

	int channel = (int)opticalFrame->getChannel();

	EV << "THE CHANNEL IS " << channel << endl;

	if (inGateName == string("portu$i") || inGateName == string("portwdmolt$i"))
	{
		EV << "PON_Splitter: sending to ONUs" << endl;
		// DownStream
		if (ports==0)
		{
			delete msg;
			return;
		}

		// check downstream busy
		PONUtil::checkDownstreamBusy(this, channel);
		PONUtil::sendDownstream(this, check_and_cast<cPacket*>(msg)->getBitLength(), channel);

		EV << "NB BITS FRAME -> " << check_and_cast<cPacket*>(msg)->getBitLength() << endl;

		for (int i=0; i<ports; i++)
		{
			// if it is the last port to send, do NOT duplicate
			bool isLast = (ingate->getIndex() ==ports-1) ? (i==ports-2) : (i==ports-1);
			// Check if we have 1 port to ONU...
			if (ports==1)
				isLast=true;

			cMessage *msg2 = isLast ? msg : (cMessage*) msg->dup();

			// We have W * ports gates ! We need to select the good one.
			// The ports here represent the ONUs

			cGate* selectedGate = gate("portg$o", i);

			if (channel > 0)
			{
				// we do * i because we have W * ports gates, W channels for all ONUs
				selectedGate = gate("portwdmonu$o", (channel-1) * ports + i);
			}

			send(msg2, selectedGate);
		}
	}
	else
	{
		// UPSTREAM: TDM -> 0, WDM -> 1..W

		cGate* selectedGate = gate("portu$o"); // default, TDM

		if (channel > 0)
		{
			selectedGate = gate("portwdmolt$o", channel - 1);
		}

		EV << "UPSTREAM PASSING, msg = " << msg << " on channel = " << channel << endl;

		// check upstream busy
		PONUtil::checkUpstreamBusy(this, channel);
		PONUtil::sendUpstream(this, check_and_cast<cPacket*>(msg)->getBitLength(), channel);



		send(msg, selectedGate);
	}


	// Here only if message sent
	previousMsg=msg;
	numMessages++;

}

void PON_Splitter::finish ()
{
    simtime_t t = simTime();
    recordScalar("simulated time", t);
    recordScalar("messages handled", numMessages);

    if (t>0)
    {
        recordScalar("messages/sec", numMessages/t);
    }
}
