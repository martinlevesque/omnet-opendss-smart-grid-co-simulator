//
// Copyright (C) 2008 Alfonso Ariza
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


package inet.experimental.nodes.adhoc;

import inet.base.MMapBoard;
import inet.base.NotificationBoard;
import inet.mobility.BasicMobility;
import inet.mobility.NullMobility;
import inet.experimental.linklayer.ieee80211.Ieee80211NicMesh;
import inet.experimental.linklayer.ieee80211.Ieee80211aNicMesh;
import inet.experimental.linklayer.ieee80211.Ieee80211gNicMesh;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.manetrouting.ManetRouting;
import inet.networklayer.ipv4.RoutingTable;
import inet.base.Sink;

module BasicNetLayer
{
    gates:
        input ifIn[1];
        output ifOut[1];
    submodules:
        sink: Sink {
            @display("p=100,193;i=block/sink");
            gates:
                in[1];
        }
    connections allowunconnected:
        ifIn[0] --> sink.in[0];
}

//
// Models a mobile host with a wireless (802.11b) card. This module is
// a WirelessHost less the \PPP and Ethernet interfaces, plus mobility
// support (BasicMobility). It is intended mainly for mobile ad-hoc
// networks.
//
module BasicMobileManetMesh
{
    parameters:
        @node();
        string mobilityType = default("NullMobility");
        @display("i=device/pocketpc_s");

    gates:
        input radioIn @directIn;

    submodules:
        routingTable: RoutingTable {
            parameters:
                IPForward = true;
                routerId = "";
                routingFile = "";
                @display("p=47,146");
        }

        notificationBoard: NotificationBoard {
            parameters:
                @display("p=146,148");
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=142,77");
        }
        sink: BasicNetLayer {
            @display("p=101,205;i=block/sink");
        }
        // the name of radio interface must have "wlan", valid examples wlan, wlan802, localwlan ....
        wlan: Ieee80211gNicMesh {
            parameters:
                @display("p=101,275;q=queue");
        }
        mobility: <mobilityType> like BasicMobility {
            parameters:
                @display("p=38,73");
        }

        mmapBoard: MMapBoard {
            parameters:
                @display("p=38,224;i=block/blackboard");
        }
    connections allowunconnected:

        // connections to network outside
        radioIn --> wlan.radioIn;
        wlan.uppergateOut --> sink.ifIn[0];
        wlan.uppergateIn <-- sink.ifOut[0];
}

module BasicFixManetMesh
{
    parameters:
        @node();
        string mobilityType = default("NullMobility");
        @display("i=device/pocketpc_s");

    gates:
        input radioIn @directIn;

    submodules:
        routingTable: RoutingTable {
            parameters:
                IPForward = true;
                routerId = "";
                routingFile = "";
                @display("p=47,146");
        }

        notificationBoard: NotificationBoard {
            parameters:
                @display("p=146,148");
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=142,77");
        }
        sink: BasicNetLayer {
            @display("p=101,205;i=block/sink");
        }
        // the name of radio interface must have "wlan", valid examples wlan, wlan802, localwlan ....
        wlan: Ieee80211gNicMesh {
            parameters:
                @display("p=101,275;q=queue");
        }
        mobility: NullMobility {
            parameters:
                @display("p=38,73");
        }

        mmapBoard: MMapBoard {
            parameters:
                @display("p=38,251;i=block/blackboard");
        }
    connections allowunconnected:

        // connections to network outside
        radioIn --> wlan.radioIn;
        wlan.uppergateOut --> sink.ifIn[0];
        wlan.uppergateIn <-- sink.ifOut[0];
}

