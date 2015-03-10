//
// voip.ned: Fichier de configuration des objets du scénario.
//
// Auteurs : Martin Lévesque et François Lévesque
//
// Date : Novembre 2011

package inet.examples.wireless.lan80211e;

import inet.applications.udpapp.VoipConfigs;
import inet.networklayer.autorouting.FlatNetworkConfigurator;
import inet.world.ChannelControl;
import inet.nodes.wireless.WirelessAPWithEthe;
import inet.nodes.wireless.WirelessHoste;
import inet.nodes.inet.StandardHost;

network Ecko
{
    parameters:
        int nbVoiceClients;
        int nbDataClients;
        @display("bgb=887,445");
    submodules:
        flatNetworkConfigurator: FlatNetworkConfigurator {
            @display("p=375,24");
        }
        channelControl: ChannelControl {
            @display("p=101,19");
        }
        wirelessAPWithEthe: WirelessAPWithEthe {
            @display("p=337,119;i=device/accesspoint");
        }
        voiceClients[nbVoiceClients]: WirelessHoste {
            @display("p=337,244");
        }

        dataClients[nbDataClients]: WirelessHoste {
            @display("p=491,176");
        }

        server: StandardHost {
            @display("p=117,122");
        }
        voipConfigs: VoipConfigs {
            parameters:
                @display("p=545,32");
                nbVoipClients = nbVoiceClients;
                nbDataClients = nbDataClients;
        }
    connections allowunconnected:
        wirelessAPWithEthe.ethg++ <--> server.ethg++;
}
