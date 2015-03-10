/*
 * Copyright (C) 2003 Andras Varga; CTIE, Monash University, Australia
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
#include "NoDelaySwitch.h"

using namespace std;

Define_Module (NoDelaySwitch);

void NoDelaySwitch::initialize()
{
	nbMultiplePorts = this->gateSize("multiple");
}

void NoDelaySwitch::handleMessage(cMessage *msg)
{
	cGate *ingate = msg->getArrivalGate();
	EV << "NoDelaySwitch::handleMessage: Frame " << msg << " arrived on port " << ingate->getName() << "...\n";

	// From single input
	if (ingate->getId() ==  gate( "single$i")->getId())
	{
		EV << "NoDelaySwitch: sending to multiple outputs" << endl;

		// DownStream
		if (nbMultiplePorts == 0)
		{
			delete msg;
			return;
		}

		for (int i=0; i < nbMultiplePorts; i++)
		{
			// if it is the last port to send, do NOT duplicate
			bool isLast = i == nbMultiplePorts - 1;

			if (nbMultiplePorts == 1)
				isLast = true;

			cMessage *msg2 = isLast ? msg : (cMessage*) msg->dup();

			cGate* selectedGate = gate("multiple$o", i);

			send(msg2, selectedGate);
		}
	}
	else
	{
		// TO single output
		cGate* selectedGate = gate("single$o");

		send(msg, selectedGate);
	}
}



void NoDelaySwitch::finish()
{
}

