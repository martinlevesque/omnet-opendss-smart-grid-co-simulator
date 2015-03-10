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

//
// TODO auto-generated module
//
simple EPON_OLT_relayDefault like EPON_OLT_relay
{
    parameters:
        string addressTableFile = default("");  	  // set to empty string if not used
        int addressTableSize = default(1000); 		  // max size of address table
        double agingTime @unit("s") = default(7200s); // max idle time for address table entries
                                     				  // (when it expires, entry is removed from the table)
        
    gates:
        input toPONin;	   		// from OLT (EPON net)
        output toPONout;	   	// to OLT (EPON net)
        input ethIn;     	// to other ports
        output ethOut;     	// to other ports

    
}
