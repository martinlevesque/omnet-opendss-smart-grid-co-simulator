package inet.examples.adhoc.multiradio;
//
// Copyright (C) 2010 Alfonso Ariza
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//




//package inet.nodes.wireless;

import inet.applications.pingapp.PingApp;
import inet.applications.tcpapp.TCPApp;
import inet.applications.udpapp.UDPApp;
import inet.base.NotificationBoard;
import inet.linklayer.ethernet.EthernetInterface;
import inet.linklayer.ieee80211.Ieee80211NicSTA;
import inet.linklayer.ieee80211.Ieee80211gNicAdhoc;
import inet.linklayer.ppp.PPPInterface;
import inet.mobility.BasicMobility;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.ipv4.RoutingTable;
import inet.nodes.inet.NetworkLayerGlobalArp;
import inet.transport.tcp.TCP;
import inet.transport.udp.UDP;
import inet.networklayer.manetrouting.ManetRouting;
import inet.networklayer.autorouting.HostAutoConfigurator;

//
// Models a host with several wireless (802.11g) cards in adhoc mode.
// Unnecessary interfaces like ppp and eth are removed.
//
module MultiRadioHost
{
    parameters:
        @node();
        int numRadios = default(1);
        int numTcpApps = default(0);
        int numUdpApps = default(0);
        string tcpAppType = default("");
        string udpAppType = default("");
        bool IPForward = default(true);
        string routingFile = default("");
        string mobilityType = default("NullMobility");

        @display("i=device/pocketpc_s");
    gates:
        input radioIn[numRadios] @directIn;
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
                IPForward = IPForward;
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
        udpApp[numUdpApps]: <udpAppType> like UDPApp {
            parameters:
                @display("p=272,67");
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
                ifIn[(sizeof(pppg)+sizeof(ethg))+numRadios];
                ifOut[(sizeof(pppg)+sizeof(ethg))+numRadios];
        }
        manetrouting: ManetRouting {
            parameters:
                @display("p=153,247;i=block/network2");
        }
        wlan[numRadios]: Ieee80211gNicAdhoc {
            parameters:
                @display("p=99,357,row,90;q=queue");
        }
        mobility: <mobilityType> like BasicMobility {
            parameters:
                @display("p=58,301");
        }
    connections allowunconnected:
        for i=0..numTcpApps-1 {
            tcpApp[i].tcpOut --> tcp.appIn++;
            tcpApp[i].tcpIn <-- tcp.appOut++;
        }

        tcp.ipOut --> networkLayer.tcpIn;
        tcp.ipIn <-- networkLayer.tcpOut;

        for i=0..numUdpApps-1 {
            udpApp[i].udpOut --> udp.appIn++;
            udpApp[i].udpIn <-- udp.appOut++;
        }

        udp.ipOut --> networkLayer.udpIn;
        udp.ipIn <-- networkLayer.udpOut;

        networkLayer.pingOut --> pingApp.pingIn;
        networkLayer.pingIn <-- pingApp.pingOut;

        networkLayer.MANETOut --> manetrouting.from_ip;
        networkLayer.MANETIn <-- manetrouting.to_ip;

        for i=0..numRadios-1 {
            radioIn[i] --> wlan[i].radioIn;
            wlan[i].uppergateOut --> networkLayer.ifIn[sizeof(pppg)+sizeof(ethg)+i];
            wlan[i].uppergateIn <-- networkLayer.ifOut[sizeof(pppg)+sizeof(ethg)+i];
        }

}

