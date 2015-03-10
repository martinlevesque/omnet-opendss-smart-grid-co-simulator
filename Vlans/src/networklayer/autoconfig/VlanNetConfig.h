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

#ifndef __VLANNETCONFIG_H__
#define __VLANNETCONFIG_H__

#include <map>
#include <string>
#include <stdlib.h>
#include <omnetpp.h>
#include "FlatNetworkConfigurator.h"
#include "InterfaceEntry.h"
#include "InterfaceTable.h"
#include "IPv4InterfaceData.h"

/**
 * A NetworkConfigurator to handle vlans on initialization.
 * Not many parameters available... What it actually does is
 * to take the 10.x.x.0/24 subnet for each vlan. 10.0.0.0/24
 * is always going to be the real device. the x.x is replaced
 * by the vlan ID. DrawBack: This means that each vlan is
 * allowed 254 machines(hosts+network devices).
 */
class VlanNetConfig : public FlatNetworkConfigurator
{
  protected:
	typedef std::map<uint16_t, uint8_t> HostsPerVlan;
	HostsPerVlan hosts_per_vlan;
    virtual void initialize(int stage);

    virtual void assignAddresses(cTopology& topo, NodeInfoVector& nodeInfo);
    virtual void setDisplayString(cTopology& topo, NodeInfoVector& nodeInfo);


};

#endif
