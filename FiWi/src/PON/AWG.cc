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
#include "AWG.h"
#include "Lambda.h"
#include "PONUtil.h"
#include <string>
#include "ONUTable.h"
#include "MPCP_codes.h"

using namespace std;

Define_Module(AWG);

void AWG::initialize()
{
    numMessages = 0;
    previousMsg = 0;
    WATCH(numMessages);

}

void AWG::handleMessage(cMessage *msg)
{
	// Handle frame sent down from the network entity: send out on every other port
	string inName = msg->getArrivalGate()->getName();
	EV << "AWG: Frame " << msg << " arrived on port name =  " << inName << "...\n";

	EtherFrameWithLLID* opticalFrame = dynamic_cast<EtherFrameWithLLID*>(msg);

	int channel = (int)opticalFrame->getChannel();

	if (inName == "portwdmolt$i")
	{
		EV << "AWG: sending to ONU" << endl;
		// DownStream

		// check downstream busy
		PONUtil::checkDownstreamBusy(this, channel);
		PONUtil::sendDownstream(this, check_and_cast<cPacket*>(msg)->getBitLength(), channel);

		if (dynamic_cast<EthernetIIFrame *>(opticalFrame->getEncapsulatedMsg()) && dynamic_cast<EthernetIIFrame *>(opticalFrame->getEncapsulatedMsg())->getEtherType() == MPCP_TYPE)
		{
			// BROADCAST!
			for (int i = 0; i < (int)ONUTable::WRNodeInfos.size(); ++i)
			{
				cGate* selectedGate = gate("portwdmonu$o", ONUTable::WRNodeInfos[i].id);

				if ( ! selectedGate)
				{
					error("AWG::handleMessage prob 3");
				}

				send(msg->dup(), selectedGate);
			}
		}
		else
		{
			// Broadcast to the ONUs in this given sector!
			for (int i = 0; i < (int)ONUTable::WRNodeInfos.size(); ++i)
			{
				if (ONUTable::WRNodeInfos[i].channel == channel)
				{
					cGate* selectedGate = gate("portwdmonu$o", ONUTable::WRNodeInfos[i].id);

					if ( ! selectedGate)
					{
						error("AWG::handleMessage prob 1");
					}

					send(msg->dup(), selectedGate);
				}
			}
		}

		delete msg; // We duplicated!
	}
	else
	{
		// UPSTREAM
		cGate* selectedGate = gate("portwdmolt$o", channel);

		// check upstream busy
		PONUtil::checkUpstreamBusy(this, channel);
		PONUtil::sendUpstream(this, check_and_cast<cPacket*>(msg)->getBitLength(), channel);

		if ( ! selectedGate)
			error("AWG::handleMessage prob 2");

		send(msg, selectedGate);
	}

}

void AWG::finish ()
{
}
