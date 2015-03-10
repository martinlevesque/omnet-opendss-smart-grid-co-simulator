//
// VoipConfigs.ned
//
// Auteurs : Martin Lévesque et François Lévesque
//
// Date : Novembre 2011

package inet.applications.udpapp;


simple VoipConfigs
{
    parameters:
        string codec = default("G723.1"); // G.711, G.729A et G723.1
        int nbDataClients = default(0);
        int nbVoipClients = default(0);
    gates:
        
}



