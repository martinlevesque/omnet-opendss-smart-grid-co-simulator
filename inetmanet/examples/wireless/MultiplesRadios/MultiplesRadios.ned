//
// Copyright (C) 2010 Alfonso Ariza
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

package inet.examples.wireless.MultiplesRadios;

import inet.world.ChannelControlExtended;
import inet.nodes.wireless.WirelessRouter;
import inet.nodes.wireless.WirelessHost;
import inet.nodes.wireless.WirelessHostMultiRadio;
import inet.nodes.inet.StandardHost;
//
// TODO auto-generated module
//
network MultiplesRadios
{
    @display("bgb=728,467");
    submodules:
        channelControl: ChannelControlExtended {
            @display("p=67,39");
        }
        ap1: WirelessRouter {
            @display("p=169,186");
            routingFile = "wirelessRouter1.rt";
            numUdpApps = 1;
            udpAppType = "DHCPServer";
            mgmtType = default("Ieee80211MgmtAP");
        }
        ap2: WirelessRouter {
            @display("p=509,186");
            routingFile = "wirelessRouter2.rt";
            numUdpApps = 1;
            udpAppType = "DHCPServer";
            mgmtType = default("Ieee80211MgmtAP");
        }
        wirelessHost: WirelessHostMultiRadio {
            @display("p=342,361");
            numRadios = 2;
            numUdpApps = 2;
            udpAppType = "DHCPClient";
        }
    connections:
        ap1.ethg++ <--> ap2.ethg++;
}

