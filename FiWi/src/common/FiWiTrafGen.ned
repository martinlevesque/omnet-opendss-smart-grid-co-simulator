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

//
// A simple traffic generator for the Ethernet model and the 802.11 model,
// and generally for any L2 model that accepts Ieee80211Ctrl control info
// on the packets. It generates EtherAppReq packets. It should be connected
// directly to EtherLLC or a Ieee802NicXXX module.
//
simple FiWiTrafGen
{
    parameters:
        double startTime @unit("s") = default(0);  // time of sending the first request
        volatile int reqLength @unit(B) = default(100B);  // length of request packets
        
        // Application type:
        // normal
        // residentialPEV
        // chargingStationPEV
        // aggregator
        // dms
        // basicElectricNode
        // substation
        // none
        string applicationType = default("normal");
        
        string bestEffortProfile = default("");
        double bestEffortProfileMultiplier = default(0);
        
        string videoTrafficProfile = default("");
        
        string voipTrafficProfile = default("");
        
        bool isTriplePlayGeneratorWithONUs = default(false);
        
        bool isDOS = default(false); // sends smart grid control + smart grid notif
        
        int nbONUs = default(9);
        
        
        string PEVArrivalsFile = default("confsPEVResidentialOnly/PEVArrivalsHome.cfg");
        string PEVsInChargingStation = default("empty");
        string SolarModelChargingStation = default("empty");
        string PEVDepartureFile = default("confsPEVResidentialOnly/PEVDeparture.cfg");
        string PEVDistanceDistribution = default("confsPEVResidentialOnly/PEVDistanceDistribution.cfg");
        string DMSSchedulingAlgorithm = default("SLM"); // randomCharging or SLM
        
        double SLMMaxSlotLengthInSeconds = default(100000); // 
        
        string powerSystemBaseLoadFile = default("confsPEVResidentialOnly/summerBaseLoad.cfg");
        
        double DMSSlotInterval = default(1); // In seconds
        int HybridNbSecondsPerHour = default(4); // 60 *60 normally
        
        double PEVBatteryCapacity = default(19.2); // kW
        double PEVBatteryKwPerHour = default(1.44);
        double maximumPowerDemand = default(342 * 2);
        double PEVStateOfCharge = default(1); // 0..1
        double PEVPenetrationLevel = default(0.84);
        
        bool withChargingStations = default(false);
        string chargingStationSchedulingAlgorithm = default("solarOptimized"); // randomCharging, solarOptimized
        bool withChargingStationSolarPanels = default(false);
        
        double PEVAverageSpeed = default(30); // In miles per hour
        
        // NORMAL: 
        // int PEVRandomDeadlineMin = default(6); // base duration + random(PEVRandomDeadlineMin, PEVRandomDeadlineMax), in hours
        // int PEVRandomDeadlineMax = default(10);
        
        // STRESS:
        int PEVRandomDeadlineMin = default(20); // base duration + random(PEVRandomDeadlineMin, PEVRandomDeadlineMax), in hours
        int PEVRandomDeadlineMax = default(21);
        double PEVLambdaStateOfChargeMsg = default(0.125);
        
        @display("i=block/app");
    gates:
        // inout phys @labels(EtherFrame); // to physical layer or the network
        input in @labels(Ieee802Ctrl/up);
        output out @labels(Ieee802Ctrl/down);
}
