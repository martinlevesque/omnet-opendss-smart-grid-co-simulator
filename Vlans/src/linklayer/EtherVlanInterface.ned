package Vlans.linklayer;
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

import inet.linklayer.ethernet.EtherMAC;
import inet.linklayer.INetworkInterface;
import inet.networklayer.queue.OutputQueue;
import Vlans.linklayer.EtherMacVlan;

//
// TODO auto-generated module
//
module EtherVlanInterface like INetworkInterface
{
    parameters:
        @display("bgb=296,121;i=block/ifcard");
        string QMgmtUnit = default("Q_mgmt_PerVlan");
    gates:
        input netwIn;
        output netwOut;
        inout phys;

    submodules:
        mac: EtherMacVlan {
            @display("p=54,58");
            promiscuous = default(true);
        }
        encap_vlans: EtherEncapVlans {
            @display("p=148,58");
        }
        queue: <QMgmtUnit> like OutputQueue  {
            @display("p=245,58");
        }

    connections:
        netwIn --> queue.in;
        queue.out --> encap_vlans.upperLayerIn;
        netwOut <-- { @display("m=n"); } <-- encap_vlans.upperLayerOut;
        encap_vlans.lowerLayerOut --> mac.upperLayerIn;
        encap_vlans.lowerLayerIn <-- mac.upperLayerOut;
        mac.phys <--> phys;

}
