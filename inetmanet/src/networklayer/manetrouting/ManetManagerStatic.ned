//
//   Copyright (C) 2009 by Alfonso Ariza
//   Malaga University 
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

package inet.networklayer.manetrouting;

//
// Dispatches the packets to the selected routing algorithm
// (No dynamic module creation is done. All routing algorithms must be connected
// to the gates by hand)
//
// NOTE: you should not use this module directly because this is internal to the ManetRouterStatic
//
simple ManetManagerStatic
{
    parameters:
        // Manager parameters
        string routingProtocol;
        bool manetActive;
        bool manetPurgeRoutingTables = default (true);
        bool AUTOASSIGN_ADDRESS = default (false);
        string AUTOASSIGN_ADDRESS_BASE = default("10.0.0.0");
    gates:
        input from_ip;
        output to_ip;
        output to_aodv;
        input from_aodv;
        output to_dsr;
        input from_dsr;
        output to_dymo;
        input from_dymo;
        output to_olsr;
        input from_olsr;
        output to_dsdv;
        input from_dsdv;
        output to_batman;
        input from_batman;
}

