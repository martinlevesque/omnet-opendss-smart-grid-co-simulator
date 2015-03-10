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
simple LinklessMAC
{
    parameters:
        bool promiscuous = default(false); // if true, all packets are received, otherwise only the
                                // ones with matching destination MAC address
        string address = default("auto");  // MAC address as hex string (12 hex digits), or
                                // "auto". "auto" values will be replaced by
                                // a generated MAC address in init stage 0.
        double txrate @unit("bps") = default(100Mbps); // maximum data rate supported by this station (bit/s);
                                // actually chosen speed may be lower due to auto-
                                // configuration. 0 means fully auto-configured.
        bool duplexEnabled = default(true);   // whether duplex mode can be enabled or not; whether
                                // MAC will actually use duplex mode depends on the result
                                // of the auto-configuration process (duplex is only
                                // possible with DTE-to-DTE connection).
        int txQueueLimit = default(1000); // maximum number of frames queued up for transmission;
                                // additional frames are dropped. Only used if queueModule==""
        string queueModule = default("");    // name of optional external queue module
        int mtu = default(1500);
        @display("i=block/rxtx");
    gates:
        input upperLayerIn @labels(EtherFrame);    // to EtherLLC or MACRelayUnitPP
        output upperLayerOut @labels(EtherFrame);  // to EtherLLC or MACRelayUnitPP
        inout phys @labels(EtherFrame); // to physical layer or the network
}

