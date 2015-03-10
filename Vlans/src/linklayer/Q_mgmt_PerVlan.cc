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

#include "Q_mgmt_PerVlan.h"

Define_Module(Q_mgmt_PerVlan);

void Q_mgmt_PerVlan::initialize()
{
	// TODO - Generated method body
}

void Q_mgmt_PerVlan::handleMessage(cMessage *msg)
{
	// TODO - Generated method body
	// Self Message
	if (msg->isSelfMessage())
	{
		EV << "Self-message " << msg << " received\n";
		return;
	}


	// Network Message
	cGate *ingate = msg->getArrivalGate();
	EV << "Frame " << msg << " arrived on port " << ingate->getName() << "...\n";

	if (ingate->getId() ==  gate( "in")->getId()){
		send(msg,"out");
	}
	else if (ingate->getId() ==  gate( "out")->getId()){
		send(msg,"in");
	}else{
		EV << "Q_mgmt_PerVlan: Message came FROM THE UnKnown DIRRECTION???? Dropping\n";
		delete msg;
	}
}


void Q_mgmt_PerVlan::requestPacket(){}

bool Q_mgmt_PerVlan::isEmpty(){
	return false;
}

