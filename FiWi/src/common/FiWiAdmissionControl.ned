//
// Copyright (C) 2003 Andras Varga; CTIE, Monash University, Australia
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
simple FiWiAdmissionControl
{
    parameters:
        @labels(node,ethernet-node);
        @display("i=block/queue");

        double mu = default(0);
        string fileInitTokens = default("");
        int bytesPerToken = default(0);
        int queueSizeInBytes = default(0);
        double fairnessThreshold = default(0.2);
        double fairnessAlphaGreen = default(0.01);
        double fairnessAlphaRed = default(-0.08);
        
    gates:
        inout inPort @labels(inputPort);
        inout outPort @labels(outputPort);
}

