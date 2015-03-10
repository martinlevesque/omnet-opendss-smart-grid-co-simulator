//
// Copyright (C) 2012 Martin Lévesque 
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



package FiWi.common;

import inet.applications.ethernet.EtherAppCli;
import inet.applications.ethernet.EtherAppSrv;
import inet.linklayer.ethernet.EtherLLC;
import inet.linklayer.ethernet.EtherMAC;


//
// An example host with one Ethernet port and traffic generators that
// generate Ethernet traffic directly. This host model does not contain
// higher layer protocols (\IP, \TCP).
//
module VeryBasicNode
{
    parameters:
        @node();
        @labels(node,ethernet-node);
        @display("i=device/pc2");

        int packetSizeInBytes = default(0);
        int lambda = default(0);
        string hostId = default("");
        string packetType = default("");
    gates:
        inout inoutPort @labels(inoutPort);
    submodules:
        veryBasicApp: VeryBasicApp {
            @display("p=136,88");
            packetSizeInBytes = packetSizeInBytes;
            lambda = lambda;
            hostId = hostId;
            packetType = packetType;
        }
    connections:
        inoutPort <--> veryBasicApp.inoutPort;
}

