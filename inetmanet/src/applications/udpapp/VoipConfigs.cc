//
// VoipConfigs.cc
//
// Auteurs : Martin Lévesque et François Lévesque
//
// Date : Novembre 2011

#include <omnetpp.h>
#include "VoipConfigs.h"
#include <string>
#include <sstream>
#include "Ieee80211eMac.h"
#include "RawStat.h"

using namespace std;

Define_Module(VoipConfigs);

string VoipConfigs::codec = "";
int VoipConfigs::nbDataClients = 0;
int VoipConfigs::nbVoipClients = 0;

double VoipConfigs::sumTCPDelays = 0;
double VoipConfigs::sumTCPThroughput = 0;
long VoipConfigs::nbTCPDelays = 0;

VoipConfigs::VoipConfigs()
{
	// TODO Auto-generated constructor stub

}

VoipConfigs::~VoipConfigs() {
	// TODO Auto-generated destructor stub
}

// Extraction des paramètres
void VoipConfigs::initialize(int stage)
{
    if (stage!=0)
        return;

    nbDataClients = par("nbDataClients").longValue();
    nbVoipClients = par("nbVoipClients").longValue();
    codec = par("codec").stringValue();

    EV << "VOIP CONFIGS, nb data clients = " << nbDataClients << endl;
    EV << "VOIP CONFIGS, nbVoipClients = " << nbVoipClients << endl;
    EV << "VOIP CONFIGS, codec = " << codec << endl;
}

// Écriture des statistiques
void VoipConfigs::finish()
{
	stringstream ss (stringstream::in | stringstream::out);

	double simulationDuration = simTime().dbl() - 2; // 2 seconds of init

	double throughputPerSecond = VoipConfigs::sumTCPThroughput / simulationDuration;
	double meanDelay = (VoipConfigs::nbTCPDelays > 0) ? (VoipConfigs::sumTCPDelays / (double)VoipConfigs::nbTCPDelays) : 0;

	ss << VoipConfigs::nbVoipClients << " " << VoipConfigs::nbDataClients << " " << VoipConfigs::codec << " " << Ieee80211eMac::aifsn3
			<< " " << Ieee80211eMac::txop3 << " " << meanDelay << " " << throughputPerSecond;

	RawStat s = RawStat("bestEffortStats", "event", 1);
	s.log(simTime().dbl(), 777, ss.str());
}
