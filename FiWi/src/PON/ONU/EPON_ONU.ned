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

import inet.nodes.ethernet.EtherHost;
import inet.applications.ethernet.EtherAppCli;
import FiWi.PON.common_pon.EPON_mux_demux;
import inet.linklayer.ethernet.EthernetInterface;
import Vlans.linklayer.EtherMacVlan;

package FiWi.PON.ONU;
//
// TODO auto-generated module
//
module EPON_ONU
{
    parameters:
        @node();
        @display("i=device/modem;bgb=533,360");
        string relayUnitType = default("EPON_ONU_relayDefault");
        bool wdmEnabled = default(false);
        int nbWDMChannels = default(0);
        double propagationDelay; // 20 km at speed of light = 200 000 km per second
        int queueLimit;
        string queuingStrategy = default("fifo");
        int forceWavelength = default(-1); // for AWG-based PON

    gates:
        inout ethg;
        inout epon;
        inout eponwdm[];

    submodules:
        onu_port: ONU_Port {
            @display("p=78,134");
            wdmEnabled = wdmEnabled;
            nbWDMChannels = nbWDMChannels;
            queueLimit = queueLimit;
            queuingStrategy = queuingStrategy;
            forceWavelength = forceWavelength;
        }
        epon_ONU_relay: <relayUnitType> like EPON_ONU_relay {
            @display("p=147,56");
        }
        mac: EtherMacVlan {
            @display("p=212,137");
            promiscuous = true;
            txQueueLimit = 100000000; // in frames
        }

        ONU_mux_demux: EPON_mux_demux {
            @display("p=157,235");
            propagationDelay = propagationDelay;
            atONU = true;
        }
    connections allowunconnected:
        mac.upperLayerIn <-- epon_ONU_relay.ethOut;
        mac.upperLayerOut --> epon_ONU_relay.ethIn;
        
        
        mac.phys <--> ethg;
        
        epon_ONU_relay.toPONout --> onu_port.netwIn;
        epon_ONU_relay.toPONin <-- onu_port.netwOut;

        onu_port.phys <--> ONU_mux_demux.portin;

        ONU_mux_demux.portpon++ <--> epon;

        for i=0..sizeof(eponwdm)-1 
        {
            ONU_mux_demux.portpon++ <--> eponwdm[i];
        }
}
