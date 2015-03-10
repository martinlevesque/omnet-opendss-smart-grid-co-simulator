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

#include "EPON_mux_demux.h"
#include "PONUtil.h"
#include <string>
#include "MyUtil.h"
#include "ONUTable.h"
#include "RawStat.h"

using namespace std;

Define_Module(EPON_mux_demux);

void EPON_mux_demux::initialize()
{
    ports = gateSize("portpon");

    // Put the gates on instant transmission
    // (or suffer from collisions)
    gate("portin$i")->setDeliverOnReceptionStart(true);

    for (int i=0; i<ports; i++)
    {
    	gate("portpon$i",i)->setDeliverOnReceptionStart(true);
    }

    propagationDelay = par("propagationDelay");
    atONU = par("atONU").boolValue();

    // Display analogy
    EV << "EPON_mux_demux: INIT, 1 in port, " << ports << " pon ports " << endl;
}

void EPON_mux_demux::handleMessage(cMessage *msg)
{
	if (msg->isSelfMessage())
	{
		EV << "EPON_mux_demux: the propagation delay is finished, forwarding." << endl;

		// UPSTREAM, nothing complex, just forward
		send(msg,"portin$o");
		return;
	}

	cGate *ingate = msg->getArrivalGate();
	string inName = ingate->getName();

	EV << "EPON_mux_demux: Frame " << msg << " arrived on port name = " << inName << "...\n";

	// Here, we have to select the right channel
	EtherFrameWithLLID* opticalFrame = dynamic_cast<EtherFrameWithLLID*>(msg);

	int channel = (int)opticalFrame->getChannel();

	if (ingate->getId() ==  gate( "portin$i")->getId())
	{
		EV << "EPON_mux_demux: sending to pon\n";

		int port = channel;

		// AWG
		if (PONUtil::getPonVersion(this) == 2)
		{
			if (atONU)
			{
				// ALWAYS ONE PORT !
				port = 0; // skip EPON
			}
			else
			{
				++port;
			}
		}

		EV << "EPON_mux_demux::handleMessage port " << port << " inf o = " << gate("portpon$o", port)->getName() << endl;

		send(msg,"portpon$o", port);
	}
	else
	{
		EV << "EPON_mux_demux: sending IN OLT/ONU propagationDelay = " << propagationDelay << " msg = " << msg << endl;

		// TRANSMISSION DELAY:
		double duration = (double)check_and_cast<cPacket*>(msg)->getBitLength() / PONUtil::getCapacityOf(this, channel);

		// PROP delay
		double propDelay = propagationDelay;

		RawStat log = RawStat("EPONMUXDEMUXLOG", "event", 1);

		// OLT
		if (! atONU)
		{
			//EV << "ONUTable::channelSchedDelay[channel] = " << ONUTable::channelSchedDelay[channel] << endl;

			EV << "throughput at " << simTime().dbl() << " = " << ONUTable::channelUtil[channel][(int)simTime().dbl()] << endl;
			EV << "capacity " << (PONUtil::getCapacityOf(this, channel) * (simTime().dbl() - (double)((int)simTime().dbl()))) << endl;
			EV << "parttttt " << simTime().dbl() - (double)((int)simTime().dbl()) << endl;

			double ratioCurCapacity = (PONUtil::getCapacityOf(this, channel) * (simTime().dbl() - (double)((int)simTime().dbl())));

			double rho = 0;

			if (ratioCurCapacity > 0)
				rho = ONUTable::channelUtil[channel][(int)simTime().dbl()] / ratioCurCapacity;
			else
				rho = 0;

			if (rho >= 1.0)
				rho = 0.99;

			if (rho < 0)
				rho = 0;

			rho = (double)((int)(rho * 100.0)) / 100.0;

			EV << "rho = " << rho << endl;


			//log.log(simTime().dbl(), rho, "rho");
			//log.log(simTime().dbl(), ratioCurCapacity, "ratioCurCapacity");
			//log.log(simTime().dbl(), ONUTable::channelUtil[channel][(int)simTime().dbl()], "ONUTable::channelUtil[channel][(int)simTime().dbl()]");
			//log.log(simTime().dbl(), ONUTable::channelSchedDelay[channel], "ONUTable::channelSchedDelay[channel]");
			//log.log(simTime().dbl(), channel, "channel");

			// 3 times, because of report, gate and packet, for both version 1 and 2
			//propDelay = propagationDelay ; //+ ONUTable::channelSchedDelay[channel];

			double mainDelay = 2.0 * propagationDelay * ((2 - rho) / (1.0 - rho));

			propDelay = mainDelay;

			if (simTime().dbl() >= 2 && rho >= 0.1 && mainDelay > ONUTable::channelSchedDelay[channel])
			{
				propDelay -= ONUTable::channelSchedDelay[channel];
			}

			//log.log(simTime().dbl(), propDelay, "propDelay");
		}

		//log.log(simTime().dbl(), duration, "duration");

		scheduleAt(simTime() + propDelay + duration, msg);

		// UPSTREAM, nothing complex, just forward
		// send(msg,"portin$o");
	}
}

void EPON_mux_demux::finish ()
{

}
