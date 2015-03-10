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

#include "ARP_Vlans.h"

Define_Module(ARP_Vlans);

void ARP_Vlans::sendPacketToNIC(cMessage *msg, InterfaceEntry *ie, const MACAddress& macAddress)
{
    // add control info with MAC address
    Ieee802VlanCtrl *controlInfo = new Ieee802VlanCtrl();
    controlInfo->setDest(macAddress);
    controlInfo->setIfName(ie->getFullName());
    msg->setControlInfo(controlInfo);
    // Ensure BC MAC
    if (dynamic_cast<ARPPacket *>(msg))
    	(dynamic_cast<ARPPacket *>(msg))->setDestMACAddress(MACAddress::BROADCAST_ADDRESS);

    // send out
    // send(msg, nicOutBaseGateId + ie->getNetworkLayerGateIndex());
    sendDirect(msg, getParentModule(), "ifOut",
                                  ie->getNetworkLayerGateIndex());
}



