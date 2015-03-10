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
module VeryBasicAdmissionControlNode
{
    parameters:
        @node();
        @labels(node,ethernet-node);
        @display("i=device/pc2");

        double mu = default(0);
        string fileInitTokens = default("");
        int bytesPerToken = default(0);
        int queueSizeInBytes = default(0);
        
    gates:
        inout inPort @labels(inputPort);
        inout outPort @labels(inputPort);
    submodules:
        fiWiAdmissionControl: FiWiAdmissionControl {
            @display("p=119,80");
            mu = mu;
            fileInitTokens = fileInitTokens;
            bytesPerToken = bytesPerToken;
            queueSizeInBytes = queueSizeInBytes;
        }
    connections:
        fiWiAdmissionControl.inPort <--> inPort;
        fiWiAdmissionControl.outPort <--> outPort;
}

