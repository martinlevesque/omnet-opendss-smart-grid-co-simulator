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

package FiWi.common;
//
// TODO auto-generated module
//
simple FiWiGeneralConfigs
{
    parameters:
        // @node();
        // @display("i=device/hub");
        
        // PON VERSION:
        // 1 = TDM EPON or WDM EPON, 2 = AWG -> PON2
        int ponVersion = default(1); 
        double pollingCycleTime = default(0.000096);
        
        // Simulation type:
        // generic: dummy packets to simulate the network performances
        // withPowerSystem: PEV...
        string simulationType = default("generic");
        
        string openDSSConfiguration = default("homeChargingStationAndV2GAndRES"); // homeOnly, homeAndChargingStation, homeChargingStationAndV2G, homeChargingStationAndV2GAndRES
        
        
        string dmsExpectedLoadProfileFile = default("confsPEVResidentialOnly/baseLoad.cfg");
        string powerSystemAggregatorsFile = default("confsPEVResidentialOnly/aggregators.cfg");
        string fiwiPowerNodeMappings = default("confsPEVResidentialOnly/fiwiPowerNodeMappings.cfg");
        string ReactiveControlAlgorithm = default("voltageControl"); // voltageControl
        string ReactiveControlSensorType = default("dataRateBased"); // dataRateBased, eventBased
        double ReactiveControlSensorThreshold = default(0);
        
        double V2GThresholdCoefficient = default(0.7); // 1.0 pour désactiver v2g
    gates:
}
