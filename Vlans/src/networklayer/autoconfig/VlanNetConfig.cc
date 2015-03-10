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

#include "VlanNetConfig.h"

Define_Module(VlanNetConfig);

using namespace std;

void VlanNetConfig::initialize(int stage)
{
	FlatNetworkConfigurator::initialize(stage);
}


void VlanNetConfig::assignAddresses(cTopology& topo, NodeInfoVector& nodeInfo){
    // assign IP addresses
    uint32 networkAddress = IPAddress("10.0.0.0").getInt();
    uint32 netmask = IPAddress("255.255.255.0").getInt();
    int maxNodes = (~netmask)-1;  // 0 and ffff have special meaning and cannot be used
    //if (topo.getNumNodes()>maxNodes)
    //    error("netmask too large, not enough addresses for all %d nodes", topo.getNumNodes());

    int numIPNodes = 0;
    for (int i=0; i<topo.getNumNodes(); i++)
    {
        // skip bus types
        if (!nodeInfo[i].isIPNode)
            continue;

        uint32 addr = networkAddress | uint32(++numIPNodes);
        nodeInfo[i].address.set(addr);

        // find interface table and assign address to all (non-loopback) interfaces
        IInterfaceTable *ift = nodeInfo[i].ift;
        for (int k=0; k<ift->getNumInterfaces(); k++)
        {
            InterfaceEntry *ie = ift->getInterface(k);
            if (!ie->isLoopback())
            {
            	string ifname = ie->getFullName();
            	int subif = ifname.find('.');
            	int vlan=0;
            	if (subif!=string::npos){
            		vlan = atoi(ifname.substr(subif+1).c_str());
            	}

            	HostsPerVlan::iterator it = hosts_per_vlan.find(vlan);
            	if (it == hosts_per_vlan.end()){
            		hosts_per_vlan[vlan] = 1;
            	}else{
            		if (hosts_per_vlan[vlan] == 254)
            			error("netmask too large, not enough addresses for all nodes "
            					"for vlan %d", vlan);
            		hosts_per_vlan[vlan]++;
            	}
            	uint32_t vl = vlan&0x00FFL;
            	uint32_t vh = vlan>>8;
            	vh=vh<<8*2;
            	vl=vl<<8;
            	addr=networkAddress|vh|vl|hosts_per_vlan[vlan];

            	ie->ipv4Data()->setIPAddress(IPAddress(addr));
            	ie->ipv4Data()->setNetmask(netmask);
            	// full address must match for local delivery
            }
        }
    }
}

void VlanNetConfig::setDisplayString(cTopology& topo, NodeInfoVector& nodeInfo)
{
    int numIPNodes = 0;
    for (int i=0; i<topo.getNumNodes(); i++)
        if (nodeInfo[i].isIPNode)
            numIPNodes++;

    // update display string
    char buf[100];
    sprintf(buf, "%d IP nodes\nin %d vlans\n(%d non-IP nodes)", numIPNodes, (int)hosts_per_vlan.size(),topo.getNumNodes()-numIPNodes);
    getDisplayString().setTagArg("t",0,buf);
}


