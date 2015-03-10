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

package inet.examples.ethernet.crossed;

import inet.nodes.inet.StandardHost;
import inet.nodes.ethernet.EtherSwitch;

import inet.linklayer.ethernet.EtherBus;
import inet.linklayer.ethernet.EtherHub;

network Crossed
{
    @display("bgb=742,548");
    submodules:
        SW_A: EtherSwitch {
            @display("p=187,171");
            relayUnitType = "MACRelayUnitSTPNP";
        }
        SW_B: EtherSwitch {
            @display("p=568,171");
            relayUnitType = "MACRelayUnitSTPNP";
        }
        SW_C: EtherSwitch {
            @display("p=187,326");
            relayUnitType = "MACRelayUnitSTPNP";
        }
        C: StandardHost {
            @display("p=148,483");
            routingFile = "C.rt";
        }
        A: StandardHost {
            @display("p=132,27");
            routingFile = "A.rt";
        }
        B: StandardHost {
            @display("p=607,27");
            routingFile = "B.rt";
        }
        SW_D: EtherSwitch {
            @display("p=568,326");
            relayUnitType = "MACRelayUnitSTPNP";
        }
        D: StandardHost {
            @display("p=623,473");
            routingFile = "D.rt";
        }
        etherHub: EtherHub {
            @display("p=384,86");
        }
    connections:
        SW_A.ethg++ <--> SW_C.ethg++;
        SW_B.ethg++ <--> SW_D.ethg++;
        SW_A.ethg++ <--> SW_D.ethg++;
        SW_B.ethg++ <--> SW_C.ethg++;
        SW_A.ethg++ <--> etherHub.ethg++;
        SW_B.ethg++ <--> etherHub.ethg++;
        A.ethg++ <--> etherHub.ethg++;
        B.ethg++ <--> etherHub.ethg++;
        SW_C.ethg++ <--> C.ethg++;
        SW_D.ethg++ <--> D.ethg++;
}


