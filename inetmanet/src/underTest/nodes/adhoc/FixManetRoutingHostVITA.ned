//
// Copyright (C) 2005 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//


package inet.underTest.nodes.adhoc;

import inet.applications.pingapp.PingApp;
import inet.applications.tcpapp.TCPApp;
import inet.applications.udpapp.UDPApp;
import inet.base.NotificationBoard;
import inet.mobility.NullMobility;
import inet.linklayer.ieee80211.Ieee80211NicAdhoc;
import inet.linklayer.ieee80211.Ieee80211aNicAdhoc;
import inet.linklayer.ieee80211.Ieee80211gNicAdhoc;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.ipv4.RoutingTable;
import inet.nodes.inet.NetworkLayerGlobalArp;
import inet.transport.tcp.TCP;
import inet.transport.udp.UDP;
import inet.networklayer.manetrouting.ManetRouting;


import inet.underTest.transport.aki.Aki;
import inet.underTest.linklayer.Ieee80216.Ieee80216NicBS;
//
// Models a mobile host with a wireless (802.11b) card. This module is
// a WirelessHost less the \PPP and Ethernet interfaces, plus mobility
// support (BasicMobility). It is intended mainly for mobile ad-hoc
// networks.
//
module FixManetRoutingHostVITA
{
    parameters:
        @node();
        int numTcpApps = default(0);
        string tcpAppType = default("");
        string routingFile = default("");
        @display("i=device/pocketpc_s");

    gates:
        input radioIn[2] @directIn;
       // input radioIn_WiMAX @directIn;

    submodules:
        notificationBoard: NotificationBoard {
            parameters:
                @display("p=60,70;i=block/control");
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=60,150;i=block/table");
        }
        routingTable: RoutingTable {
            parameters:
                //IPForward = IPForward,
                IPForward = true;
                routerId = "";
                routingFile = routingFile;
                @display("p=60,230;i=block/table");
        }
        tcpApp[numTcpApps]: <tcpAppType> like TCPApp {
            parameters:
                @display("p=153,45;i=block/app");
        }
        tcp: TCP {
            parameters:
                @display("p=178,136;i=block/wheelbarrow");
        }
        udp: UDP {
            parameters:
                @display("p=294,142;i=block/transport");
        }
        pingApp: PingApp {
            parameters:
                @display("i=block/app;p=353,200");
        }
        networkLayer: NetworkLayerGlobalArp {
            parameters:
                proxyARP = false;
                globalARP = true;
                @display("p=248,247;i=block/fork;q=queue");
            gates:
                ifIn[2];
                ifOut[2];
        }
        manetrouting: ManetRouting {
            parameters:
                @display("p=153,208;i=block/network2");
        }
        // the name of radio interface must have "wlan", valid examples wlan, wlan802, localwlan ....
        wlan: Ieee80211gNicAdhoc { // Nic80211 also works, it uses MF's 802.11 implementation
            parameters:
                @display("p=294,350;q=queue;i=block/ifcard");
        }
        
        mobility: NullMobility {
            parameters:
                @display("p=362,286;i=block/cogwheel");
        }
        aki: Aki {
            @display("p=333,45");
        }
        ieee80216NicBS: Ieee80216NicBS {
            @display("p=178,350");
        }
    connections allowunconnected:
        for i=0..numTcpApps-1 {
            tcpApp[i].tcpOut --> tcp.appIn++;
            tcpApp[i].tcpIn <-- tcp.appOut++;
        }

        tcp.ipOut --> networkLayer.tcpIn;
        tcp.ipIn <-- networkLayer.tcpOut;

        //for i=0..numUdpApps-1 {
        //}

        udp.ipOut --> networkLayer.udpIn;
        udp.ipIn <-- networkLayer.udpOut;

        networkLayer.pingOut --> pingApp.pingIn;
        networkLayer.pingIn <-- pingApp.pingOut;

        networkLayer.MANETOut --> manetrouting.from_ip;
        networkLayer.MANETIn <-- manetrouting.to_ip;

        // connections to network outside
        radioIn[0] --> wlan.radioIn;
        wlan.uppergateOut --> networkLayer.ifIn[0];
        wlan.uppergateIn <-- networkLayer.ifOut[0];


        udp.appOut++ --> aki.udpIn;
        aki.udpOut --> udp.appIn++;
        ieee80216NicBS.macUpperGateOut --> networkLayer.ifIn[1];
        radioIn[1] --> ieee80216NicBS.radioIn;
        networkLayer.ifOut[1] --> ieee80216NicBS.macUpperGateIn++;
}
