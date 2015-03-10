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
simple FiWiTrafficMatrix
{
    parameters:
        string trafFile; 
        bool withGroupedPackets = default(true);
        int reqLength = default(1500);
        @display("i=block/app");
    gates:
}
