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

package FiWi.PON;
//
// TODO auto-generated module
//
simple PON_Splitter
{
    parameters:
        @node();
        @display("i=device/hub");
    gates:
        
        inout portu; // TDM, to OLT
        inout portg[];  // to stations; each one represents a port, TDM
        inout portwdmolt[]; // WDM, to OLT
        inout portwdmonu[]; // WDM, to ONU
        
}
