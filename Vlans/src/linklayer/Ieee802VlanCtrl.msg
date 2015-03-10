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
cplusplus {{
#include "MACAddress.h"
#include "Ieee802Ctrl_m.h"
}}

class noncobject MACAddress;
class noncobject Ieee802Ctrl;

//
// Control structure for communication between LLC and higher layers
// Added interface name though in order for LLC to be able to set
// the vlan id. We extend so we are going to have backwards compatibility
// with the older inet modules, since casting to Ieee802Ctrl will be ok.
//
class Ieee802VlanCtrl extends Ieee802Ctrl
{
    string ifName;		 // Interface name (from routing decision)
}