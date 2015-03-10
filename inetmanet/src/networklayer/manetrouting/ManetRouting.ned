//
//   Copyright (C) 2009 by Alfonso Ariza
//   Malaga University 
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

package inet.networklayer.manetrouting;

//
// MANET router 
//
// This router module should be attached to the network layer of the node that 
// participates in MANET routing. It contains a ManetManeger which instantiates the
// requested routing protocol. You should configure the routing protcol to be the same
// across the whole network. Place the following in your INI file:
//
// **.manetrouting.manetmanager.routingProtocol="DSR"
// 
// along with parameters for the choosen routing algorithm.
// At the moment the following MANET routing algorithms are supported:
// OLSR, DYMOFAU, DYMO, DSR, DSDV, AODV
// OLSR_ETX is not yet supported by the ManetManager
// 
// NOTE:
//  - A node participating in MANET routing must have a single network interface with "wlan" 
//    in its name somewhere e.g: wlan0 or localwlan
// 
module ManetRouting
{
    parameters:
        @display("bgb=160,88");
    gates:
        input from_ip;
        output to_ip;
    submodules:
        // the name of manet routing manager module must be "manetmanager", the routing protocols is ManetRouting.ned
        manetmanager: ManetManager {
            parameters:
                // Manager parameters
                // eliminate in the routing table all the initial reference to wlan interfaces
                manetActive = true;
               // manetPurgeRoutingTables = true;
                @display("p=90,48;b=40,24");
        }
    connections allowunconnected:
        manetmanager.from_ip <-- from_ip;
        manetmanager.to_ip --> to_ip;
}






