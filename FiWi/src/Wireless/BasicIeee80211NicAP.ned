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

//
// This NIC implements an 802.11 network interface card,
// in an AP, using infrastructure mode.
//
module BasicIeee80211NicAP
{
    parameters:
        @display("i=block/ifcard");
        
        double propagationDelay;
        
        int aggregationTresholdBytes = default(0); // For 802.11n, a small value = 1 packet aggregated.
        double maximumAggregationDuration = default(0.0001);
        double BER = default(0); // no error
        bool selectiveBERAck = default(false); // MSDU if selectiveBERAck = false && aggregationTresholdBytes > 0, 
        								// MPDU if selectiveBERAck = true && aggregationTresholdBytes > 0
        string wiType = default("MSDU"); // MSDU or MPDU
        double phyHeaderDuration = default(0.000020);
        
        string backoffCWmins;
        string backoffCWmaxs;
        string deltas;
        string trafficClasses;
        
    gates:
        inout radios[];
        inout collocatedRadios[]; 
        input uppergateIn;
        output uppergateOut;
        output rangeAreas[];
        inout ethg;
    submodules:
        mgmt: BasicIeee80211MgmtAP 
        {
            parameters:
                @display("p=96,69;q=wlanDataQueue");
                aggregationTresholdBytes = aggregationTresholdBytes;
                maximumAggregationDuration = maximumAggregationDuration;
                wiType = wiType;
        }
        
        mac: BasicIeee80211Mac 
        {
            parameters:
                queueModule = "mgmt";
                phyHeaderDuration = phyHeaderDuration;
                
                backoffCWmins = backoffCWmins;
                backoffCWmaxs = backoffCWmaxs;
                deltas = deltas;
                propagationDelay = propagationDelay;
                trafficClasses = trafficClasses;
                
                @display("p=96,155");
        }
        radio: Basic80211Radio 
        {
            parameters:
                @display("p=96,240");
                propagationDelay = propagationDelay;
                BER = BER;
                selectiveBERAck = selectiveBERAck;
                phyHeaderDuration = phyHeaderDuration;
        }
    connections:
        for i=0..sizeof(radios)-1
        {
        	radios[i] <--> radio.radios++;
        }
        
        for i=0..sizeof(collocatedRadios)-1
        {
        	collocatedRadios[i] <--> mgmt.collocatedRadios++;
        }
        
        for i=0..sizeof(rangeAreas)-1 {
        	rangeAreas[i] <-- radio.rangeAreas++;
        }
        
        radio.uppergateOut --> mac.lowergateIn;
        radio.uppergateIn <-- mac.lowergateOut;
        
        mac.uppergateOut --> mgmt.macIn;
        mac.uppergateIn <-- mgmt.macOut;

        mgmt.uppergateOut --> uppergateOut;
        mgmt.uppergateIn <-- uppergateIn;

		ethg <--> mgmt.ethg;
}

