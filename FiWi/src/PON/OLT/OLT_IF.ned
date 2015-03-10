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

package FiWi.PON.OLT;

import FiWi.PON.common_pon.EPON_mac;
import inet.linklayer.ethernet.EtherMAC2;
//
// TODO auto-generated module
//
module OLT_IF
{
    parameters:
        string queueType = default("DropTailQueue");
        @display("i=block/ifcard;bgb=168,241");
        string QMgmtUnit = default("OLT_QPL_RR");
        int nbWDMChannels = default(0);
        double propagationDelay; // 20 km at speed of light = 200 000 km per second
        int maximumGrantSize;
        int nbONUs;
        int queueLimit = default(10000000);
        string queuingStrategy;
        string admissionControlConfs = default("");

    gates:
        input netwIn;
        output netwOut;
        inout phys;

    submodules:
        epon_mac: EPON_mac 
        {
            @display("p=84,185;i=block/rxtx");
            promiscuous = true;
        }
        oltMacCtl: OLTMacCtl 
        {
            @display("p=84,116");
            queueLimit = queueLimit;
            queuingStrategy = queuingStrategy;
            admissionControlConfs = admissionControlConfs;
        }

        olt_Q_mgmt: <QMgmtUnit> like OLT_Q_mgmt {
            @display("p=84,42");
            nbWDMChannels = nbWDMChannels;
            propagationDelay = propagationDelay;
            maximumGrantSize = maximumGrantSize;
            nbONUs = nbONUs;
        }
    connections:
        epon_mac.upperLayerOut --> oltMacCtl.lowerLayerIn;
        oltMacCtl.lowerLayerOut --> epon_mac.upperLayerIn;

        olt_Q_mgmt.lowerLayerOut --> oltMacCtl.upperLayerIn;
        olt_Q_mgmt.upperLayerOut --> netwOut;

       	epon_mac.phys <--> phys;
        
        netwIn --> olt_Q_mgmt.upperLayerIn;
        oltMacCtl.upperLayerOut --> olt_Q_mgmt.lowerLayerIn;
}
