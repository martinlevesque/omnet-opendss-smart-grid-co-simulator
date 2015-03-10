package inet.underTest.nodes.adhoc;

//
// Copyright (C) 2006 Andras Varga
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
import inet.mobility.BasicMobility;
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
import inet.mobility.ANSimMobility;
//
// Models a mobile host with a wireless (802.11b) card in ad-hoc mode.
// This model contains the new IEEE 802.11 implementation, Ieee80211Nic,
// and IP, TCP and UDP protocols. The mobility model can be
// dynamically specified with the mobilityType parameter.
//
// @see MFMobileHost, WirelessHost
//
module MobileManetRoutingHostAki
{
    parameters:
        @node();
        int numTcpApps = default(0);
        int numUdpApps = default(0);
        string tcpAppType = default("");
        //string udpAppType = default("");
        string routingFile = default("");
        //string mobilityType = default("NullMobility");
        string mobilityType = default("RandomWPMobility");
        @display("i=device/pocketpc_s");
    gates:
        input radioIn @directIn;
    submodules:
        notificationBoard: NotificationBoard {
            parameters:
                @display("p=60,70");
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=60,150");
        }
        routingTable: RoutingTable {
            parameters:
                IPForward = true;
                routerId = "";
                routingFile = routingFile;
                @display("p=60,230");
        }
        tcpApp[numTcpApps]: <tcpAppType> like TCPApp {
            parameters:
                @display("p=163,67");
        }
        tcp: TCP {
            parameters:
                @display("p=163,154");
        }
        udp: UDP {
            parameters:
                @display("p=272,154");
        }
        pingApp: PingApp {
            parameters:
                @display("p=343,200");
        }
        networkLayer: NetworkLayerGlobalArp {
            parameters:
                proxyARP = false;
                globalARP = true;
                @display("p=248,247;q=queue");
            gates:
                ifIn[1];
                ifOut[1];
        }
        manetrouting: ManetRouting {
            @display("p=153,247");
        }
        wlan: Ieee80211gNicAdhoc {
            parameters:
                @display("p=248,349;q=queue");
        }
        mobility: <mobilityType> like BasicMobility { 
            parameters:
                @display("p=149,307");
        }
        aki: Aki {
            @display("p=333,63");
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
        radioIn --> wlan.radioIn;
        wlan.uppergateOut --> networkLayer.ifIn[0];
        wlan.uppergateIn <-- networkLayer.ifOut[0];

        udp.appOut++ --> aki.udpIn;
        aki.udpOut --> udp.appIn++;
}

