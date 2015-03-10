//
// Copyright (C) 2006 Andras Varga
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

package FiWi.Wireless;

import FiWi.common.FiWiTrafGen;
import inet.base.NotificationBoard;
import inet.mobility.NullMobility;

//
// Models a 802.11 Access Point. It can be used in conjunction with
// WirelessHost, or any other host model containing WirelessNicSTA.
//
// @see WirelessAP, WirelessAPSimplified, WirelessAPWithEth, WirelessAPWithEthSimplified
// @see WirelessHost, WirelessHostSimplified
// @see MobileHost, MFMobileHost
//
module Basic80211AP
{
    parameters:
        @node();
        @labels(node,wireless-node);
        @display("i=device/accesspoint;bgb=303,275");
                               // 
        double propagationDelay = 0.00000075; // default(0.00000025); // 1 km at speed of light
        //double propagationDelay = 0.000000003; // default(0.00000025); // 1 km at speed of light
        
        string applicationType = default("normal");
        string PEVArrivalsFile = default("");
        
        string backoffCWmins = default("32"); 
        string backoffCWmaxs = default("1024"); 
        string deltas = default("0");
        string trafficClasses = default("1");

    gates:
        inout radios[];

        output rangeAreas[];
        inout ethg;
        
        inout collocatedRadios[];

    submodules:
        wlan: BasicIeee80211NicAP 
        {
            parameters:
                @display("p=110,179;q=queue");
                propagationDelay = propagationDelay;
                backoffCWmins = backoffCWmins;
                backoffCWmaxs = backoffCWmaxs;
                deltas = deltas;
                trafficClasses = trafficClasses;
        }
        notificationBoard: NotificationBoard 
        {
            parameters:
                @display("p=71,36");
        }
        cli: FiWiTrafGen 
        {
            parameters:
            	@display("p=110,107");
            	applicationType = applicationType;
            	PEVArrivalsFile = PEVArrivalsFile;
        }
    connections allowunconnected:

        for i=0..(sizeof(radios)-1) 
        {
            radios[i] <--> wlan.radios++;
        }
        
        for i=0..(sizeof(collocatedRadios)-1) 
        {
            collocatedRadios[i] <--> wlan.collocatedRadios++;
        }

		for i=0..(sizeof(rangeAreas)-1) {
        	rangeAreas[i] <-- wlan.rangeAreas++;
		}

        cli.out --> wlan.uppergateIn;

        ethg <--> wlan.ethg;
        wlan.uppergateOut --> cli.in;
}

