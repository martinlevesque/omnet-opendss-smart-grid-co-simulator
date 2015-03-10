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

import inet.linklayer.ethernet.EtherMAC;
import Vlans.linklayer.EtherMacVlan;
import FiWi.PON.common_pon.*;

package FiWi.PON.OLT;

//
// TODO auto-generated module
//
module EPON_OLT
{
    parameters:
        @node();
        @display("i=device/router;bgb=453,357");
        string relayUnitType = default("EPON_OLT_relayDefault");
        int nbWDMChannels = default(0);
        double propagationDelay; // 20 km at speed of light = 200 000 km per second
        int maximumGrantSize;
        int nbONUs;
        int queueLimit = default(10000000);
        string queuingStrategy = default("fifo");
        
    gates:
        inout ethg;
        inout epon;
        inout eponwdm[];
    submodules:
        epon_OLT_relay: <relayUnitType> like EPON_OLT_relay {
            @display("p=106,105");
        }
        mac: EtherMacVlan {
            @display("p=47,185");
            promiscuous = true;
            txQueueLimit = 100000000;
        }
        olt_if: OLT_IF {
            @display("p=165,185");
            nbWDMChannels = nbWDMChannels;
            propagationDelay = propagationDelay;
            maximumGrantSize = maximumGrantSize;
            nbONUs = nbONUs;
            queueLimit = queueLimit;
            queuingStrategy = queuingStrategy;
        }
        onuTable: ONUTable 
        {
            @display("p=39,26");
        }
        OLT_mux_demux: EPON_mux_demux {
            @display("p=311,244");
            propagationDelay = propagationDelay;
            atONU = false;
        }
    connections allowunconnected:

        mac.upperLayerIn <-- epon_OLT_relay.ethOut;
        mac.upperLayerOut --> epon_OLT_relay.ethIn;
        mac.phys <--> ethg;

        epon_OLT_relay.toPONout --> olt_if.netwIn;
        olt_if.netwOut --> epon_OLT_relay.toPONin;

		// EPON_mac to mux/demux
        olt_if.phys <--> OLT_mux_demux.portin;
        
        // epon to mux/demux
        OLT_mux_demux.portpon++ <--> epon;

        for i=0..sizeof(eponwdm)-1 
        {
            OLT_mux_demux.portpon++ <--> eponwdm[i];
        }
}
