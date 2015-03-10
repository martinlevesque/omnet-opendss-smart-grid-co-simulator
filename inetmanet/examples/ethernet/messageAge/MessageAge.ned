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

package inet.examples.ethernet.messageAge;

import inet.nodes.inet.StandardHost;
import inet.nodes.ethernet.EtherSwitch;
import inet.linklayer.ethernet.EtherHub;

import inet.underTest.channels.ProgramedFailureChannel;

//
// Message Age test topology
//
network MessageAge
{
    @display("bgb=760,298");
    submodules:
        SW_111: EtherSwitch {
            @display("p=135,69");
            relayUnitType = "MACRelayUnitSTPNP";
        }
        SW_222: EtherSwitch {
            @display("p=351,69");
            relayUnitType = "MACRelayUnitSTPNP";
        }
        SW_333: EtherSwitch {
            @display("p=505,69");
            relayUnitType = "MACRelayUnitSTPNP";
        }
        SW_444: EtherSwitch {
            @display("p=655,69");
            relayUnitType = "MACRelayUnitSTPNP";
        }
        SW_555: EtherSwitch {
            @display("p=661,197");
            relayUnitType = "MACRelayUnitSTPNP";
        }
        SW_666: EtherSwitch {
            @display("p=553,197");
            relayUnitType = "MACRelayUnitSTPNP";
        }
        SW_777: EtherSwitch {
            @display("p=411,197");
            relayUnitType = "MACRelayUnitSTPNP";
        }
        SW_888: EtherSwitch {
            @display("p=266,197");
            relayUnitType = "MACRelayUnitSTPNP";
        }
        SW_999: EtherSwitch {
            @display("p=127,197");
            relayUnitType = "MACRelayUnitSTPNP";
        }
    connections:
        SW_111.ethg++ <--> SW_222.ethg++;
        SW_222.ethg++ <--> SW_333.ethg++;
        SW_333.ethg++ <--> SW_444.ethg++;
        SW_444.ethg++ <--> SW_555.ethg++;
        SW_555.ethg++ <--> SW_666.ethg++;
        SW_666.ethg++ <--> SW_777.ethg++;
        SW_777.ethg++ <--> SW_888.ethg++;
        SW_999.ethg++ <--> SW_888.ethg++;
}
