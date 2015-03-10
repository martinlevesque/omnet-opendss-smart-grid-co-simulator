//
// Copyright (C) 2006 Andras Varga and Levente M�sz�ros
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

//
// This is not a concrete module type, but a prototype for all IEEE 802.11
// management module types. It exists to specify what gates a management
// module should have in order to be usable within Ieee80211Nic.
//

package inet.underTest.linklayer.ieee80211.mgmt;
import inet.linklayer.ieee80211.mgmt.Ieee80211Mgmt;

moduleinterface Ieee80211MgmtInterface extends Ieee80211Mgmt
{
    gates:
        input agentIn;
        output agentOut;    
}
