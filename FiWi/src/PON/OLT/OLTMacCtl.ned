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
simple OLTMacCtl
{
   parameters:
    	@display("i=block/queue");
    	int queueLimit = default(10000000); // In bytes
    	string queuingStrategy;
    	string admissionControlConfs;
    	
    gates:
        input upperLayerIn;   // higher layer protocols or applications
        output upperLayerOut; // higher layer protocols or applications
        input lowerLayerIn;     // to Ethernet MAC
        output lowerLayerOut;   // to Ethernet MAC
}
