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

package FiWi.PON.common_pon;
//
// TODO auto-generated module
//
simple EPON_mac
{
    parameters:
        bool promiscuous = default(true);     // if true, all packets are received, otherwise only the
                                // ones with matching destination MAC address
        string address = default("auto");       // MAC address as hex string (12 hex digits), or
                                // "auto". "auto" values will be replaced by
                                // a generated MAC address in init stage 0.
        int txQueueLimit = default(100000000); 	// maximum number of frames queued up for transmission;
                                		// additional frames are dropped. Only used if queueModule==""
                                		// The 2nd frame is used in case of changing registers with MPCP
                                		// ... sometimes 1 is already transmitted and the next MPCP decides 
                                		// to transmitt again with the new times.
        string queueModule = default("");    // name of optional external queue module
        int mtu = default(1500);
        double txRatePerChannel = 1000000000; // not used anymore, for wdm.
        @display("i=block/queue");
    gates:
        input upperLayerIn;    // to EtherLLC or MACRelayUnitPP
        output upperLayerOut;  // to EtherLLC or MACRelayUnitPP
        inout phys;            // to physical layer or the network
}
