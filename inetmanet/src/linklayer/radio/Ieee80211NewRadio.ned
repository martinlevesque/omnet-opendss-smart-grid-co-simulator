//
// Copyright (C) Andras Varga, Levente Meszaros
// Based on the Mobility Framework's SnrEval by Marc Loebbers
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


package inet.linklayer.radio;
import inet.linklayer.radio.PropagationModel;
//
// Physical layer for the IEEE 802.11 models. Its external interface
// (including gates and how it communicates with other modules)
// is the same as the Radio module's.
//
// The implementation is largely based on the Mobility Framework's
// SnrEval80211 and Decider80211 modules.
//
// @see Radio
// @author Andras Varga
//
simple Ieee80211NewRadio extends PropagationModel
{
    parameters:
        @class(Ieee80211NewRadio);
        int channelNumber = default(0); // channel identifier
        double carrierFrequency @unit("Hz") = default(2.4GHz);
        double transmitterPower @unit("mW") = default(20mW); // power used for transmission of messages (in mW)
        double bitrate @unit("bps"); // (in bits/s)
        double thermalNoise @unit("dBm") = default(-110dBm); // base noise level (dBm)
        double pathLossAlpha = default(2); // used by the path loss calculation
        double shadowingDeviation @unit("dB") = default(0dB); // used by the shadowing model calculation
        double snirThreshold @unit("dB") = default(4dB); // if signal-noise ratio is below this threshold, frame is considered noise (in dB)
        double sensitivity @unit("dBm"); // received signals with power below sensitivity are ignored
        string berTableFile=default("");
        string phyOpMode @enum ("b","g","a","p") = default("g"); 
        string WifiPreambleMode @enum ("LONG","SHORT") = default("LONG"); // Wifi preambre mode Ieee 2007, 19.3.2
        string attenuationModel = default("PathLossReceptionModel");// Values "pathlost" or "tworay"
        bool   AutoHeaderSize=default(false); // in the receiver the radio model compute the header size in function of timers and bitrate
        bool drawCoverage =default(true);
        double refresCoverageInterval @unit("s")=default(5s);
        int    btSize @unit("b")= default(8192b);// test size frame for Airtime Link Metric
        bool   airtimeLinkComputation=default(true);
        
        @display("i=block/wrxtx");
        double usage_radio_idle @unit("mA") = default (-1mA); // disable battery registration
        double usage_radio_recv @unit("mA") = default (-1mA); // disable battery registration
        double usage_radio_sleep @unit("mA") = default (-1mA); // disable battery registration 
        double usage_radio_send @unit("mA") = default (-1mA); // disable battery registration 
        
}

