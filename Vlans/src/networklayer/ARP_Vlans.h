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

#ifndef __ARP_VLANS_H__
#define __ARP_VLANS_H__

#include <omnetpp.h>
#include "InterfaceEntry.h"
#include "MACAddress.h"
#include "ARP.h"
#include "Ieee802VlanCtrl_m.h"
#include "IPAddress.h"
#include "IPv4InterfaceData.h"

/**
 * Extended Class to add interface name in the controlInfo
 */
class ARP_Vlans : public ARP
{
  protected:
    virtual void sendPacketToNIC(cMessage *msg, InterfaceEntry *ie, const MACAddress& macAddress);
    /**
     * One line only added to keep the original incoming interface name... GRRR
     * IP Layer must change....
     */
    //virtual void processARPPacket(ARPPacket *arp);
};

#endif
