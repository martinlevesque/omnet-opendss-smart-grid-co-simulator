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

import inet.linklayer.ethernet.EtherBus;
import inet.applications.ethernet.EtherAppCli;
import inet.applications.ethernet.EtherAppSrv;
import inet.linklayer.ethernet.EtherLLC;
import inet.linklayer.ethernet.EtherMAC;


//
// An example host with one Ethernet port and traffic generators that
// generate Ethernet traffic directly. This host model does not contain
// higher layer protocols (\IP, \TCP).
//
module FiWiTrafGenNode
{
    parameters:
        @node();
        @labels(node,ethernet-node);
        @display("i=device/pc2");

        bool withConfirmation = default(false);

        string applicationType = default("normal");
        string PEVArrivalsFile = default("empty");
        string PEVsInChargingStation = default("empty");
        string SolarModelChargingStation = default("empty");
        string DMSSchedulingAlgorithm = default("profileBased");
        
        double SLMMaxSlotLengthInSeconds = default(100000000);
        
        string bestEffortProfile = default("");
        double bestEffortProfileMultiplier = default(0);

        string videoTrafficProfile = default("");
        string voipTrafficProfile = default("");
        
        int nbONUs = default(32);
        bool isTriplePlayGeneratorWithONUs = default(false);
        bool isDOS = default(false);

    gates:
        inout ethg @labels(EtherFrame-conn);
    submodules:
        cli: FiWiTrafGen {
            parameters:
                @display("p=155,62,col");
                applicationType = applicationType;
                PEVArrivalsFile = PEVArrivalsFile;
                DMSSchedulingAlgorithm = DMSSchedulingAlgorithm;
                SLMMaxSlotLengthInSeconds = SLMMaxSlotLengthInSeconds;
                bestEffortProfile = bestEffortProfile;
                bestEffortProfileMultiplier = bestEffortProfileMultiplier;
                videoTrafficProfile = videoTrafficProfile;
                voipTrafficProfile = voipTrafficProfile;
                nbONUs = nbONUs;
                isTriplePlayGeneratorWithONUs = isTriplePlayGeneratorWithONUs;
                isDOS = isDOS;
                PEVsInChargingStation = PEVsInChargingStation;
                SolarModelChargingStation = SolarModelChargingStation;
        }
        mac: LinklessMAC {
            parameters:
                queueModule = "";
                txQueueLimit = 1000000000;
                txrate = 1Gbps;
                promiscuous = true;
                @display("p=155,200;q=queue");
        }
    connections:
        mac.phys <--> ethg;

        cli.out --> mac.upperLayerIn;
        mac.upperLayerOut --> cli.in;
}

