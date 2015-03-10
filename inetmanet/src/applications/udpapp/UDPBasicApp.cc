//
// UDPBasicApp.cc : Modifié pour générer du trafic Voip.
//
// Auteurs : Martin Lévesque et François Lévesque
//
// Date : Novembre 2011

#include <omnetpp.h>
#include <string>
#include <math.h>
#include "UDPBasicApp.h"
#include "UDPControlInfo_m.h"
#include "IPAddressResolver.h"
#include "RawStat.h"
#include "Ieee80211eMac.h"
#include <sstream>
#include <map>
#include "VoipConfigs.h"

using namespace std;

Define_Module(UDPBasicApp);

int UDPBasicApp::counter;

// Pour enregistrement de statistiques.
long UDPBasicApp::overallPacketsSent = 0;
long UDPBasicApp::overallPacketsReceived = 0;

// Méthode qui initialise les configurations de l'application Voip.
void UDPBasicApp::initialize(int stage)
{

    if (stage!=3)
        return;

    counter = 0;
    numSent = 0;
    numReceived = 0;
    WATCH(numSent);
    WATCH(numReceived);

    localPort = par("localPort");
    destPort = par("destPort");


    overallThroughput = 0;
    sumDelays = 0;
    nbDelays = 0;
    messageLength = 0; // Configuré dans next arr.

    const char *destAddrs = par("destAddresses");
    cStringTokenizer tokenizer(destAddrs);
    const char *token;

    while ((token = tokenizer.nextToken())!=NULL)
        destAddresses.push_back(IPAddressResolver().resolve(token));

    bindToPort(localPort);

    if (destAddresses.empty())
    	return;

    // Premier paquet, timer
    double nextArrival = getNextArrivalTime();

    cMessage *timer = new cMessage("sendTimer");
    scheduleAt(simTime().dbl() + uniform(2,3) + nextArrival, timer);
}

IPvXAddress UDPBasicApp::chooseDestAddr()
{
    int k = intrand(destAddresses.size());
    return destAddresses[k];
}

// Création du paquet Voip
cPacket *UDPBasicApp::createPacket()
{
    char msgName[32];
    sprintf(msgName,"UDPBasicAppData-%d", counter++);

    cPacket *payload = new cPacket(msgName);
    payload->setBitLength(messageLength);
    return payload;
}

void UDPBasicApp::sendPacket()
{
    cPacket *payload = createPacket();
    IPvXAddress destAddr = chooseDestAddr();
    sendToUDP(payload, localPort, destAddr, destPort);

    numSent++;
    overallPacketsSent++;
}

// Selon le codec, définit la data rate et la taille de paquet.
// Selon ces 2 paramètres, le temps du prochain paquet est calculé
// Référence Cisco : http://www.cisco.com/en/US/tech/tk652/tk698/technologies_tech_note09186a0080094ae2.shtml
double UDPBasicApp::getNextArrivalTime()
{
	double bitsPerSecond = 0;

	// G.711 : 64 Kbps, G.729A : 8 Kbps et G723.1 : 6.3 Kbps
	if (VoipConfigs::codec == "G.711")
	{
		bitsPerSecond = 64 * 1000;
		messageLength = 160 * 8;
	}
	else
	if (VoipConfigs::codec == "G.729A")
	{
		bitsPerSecond = 8 * 1000;
		messageLength = 20 * 8;
	}
	else
	if (VoipConfigs::codec == "G723.1")
	{
		bitsPerSecond = 6.3 * 1000.0;
		messageLength = 24 * 8;
	}
	else
	{
		error("UDPBasicApp::getNextArrivalTime - Unknown codec (%s) !", VoipConfigs::codec.c_str());
	}

	// Ce n'est pas aléatoire puisque les paquets sont envoyés selon un taux constant.
	double lambda = ceilf(bitsPerSecond / (double)messageLength);

	if (lambda <= 0)
		lambda = 1;

	// Temps d'attente avant d'envoyer un autre paquet
	double waitTime = 1.0 / lambda;

	// Prochaine arrivée événementiel
	return simTime().dbl() + waitTime;
}

void UDPBasicApp::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        // send, then reschedule next sending
        sendPacket();

        scheduleAt(getNextArrivalTime(), msg);
    }
    else
    {
        // process incoming packet
        processPacket(PK(msg));
    }

    if (ev.isGUI())
    {
        char buf[40];
        sprintf(buf, "rcvd: %d pks\nsent: %d pks", numReceived, numSent);
        getDisplayString().setTagArg("t",0,buf);
    }
}

// Réception du paquet, enregistrement de statistiques
void UDPBasicApp::processPacket(cPacket *msg)
{
    EV << "Received packet: ";

    // Throughput:
    overallThroughput += msg->getBitLength();

    // Délai:
    double curDelay = (simTime() - msg->getCreationTime()).dbl();

    sumDelays += curDelay;
    ++nbDelays;

    // Jitter:
    UDPControlInfo* ctrl = dynamic_cast<UDPControlInfo*>(msg->getControlInfo());

    if (ctrl)
    {
    	arrivals[ctrl->getSrcAddr().get4().str()].push_back(simTime().dbl());
    }

    // Drop:
    overallPacketsReceived++;

    printPacket(msg);
    delete msg;

    numReceived++;
}

// Calcul du Jitter par source
double UDPBasicApp::calculateMeanJitter()
{
	double curJitter = 0;
	long nbCurJitter = 0;

	for (map<std::string, std::vector<double> >::iterator it = arrivals.begin(); it != arrivals.end(); ++it)
	{
		for (int i= 0; i < (int)it->second.size(); ++i)
		{
			if (i > 0)
			{
				// Cur packet vs last packet
				curJitter += it->second[i] - it->second[i - 1];
				++nbCurJitter;
			}
		}
	}

	if (nbCurJitter > 0)
		curJitter = curJitter / (double)nbCurJitter;

	return curJitter;
}

// Exécuté à la fin de la simulation, écritude de statistiques
void UDPBasicApp::finish ()
{
	// server !
	if (numReceived > 0)
	{
		stringstream ss (stringstream::in | stringstream::out);

		double simulationDuration = simTime().dbl() - 2; // 2 seconds of init

		// nb_voip_clients, nb_data_clients, codec, aifsn3, txop3, delay, jitter, throughput, % pkt drop, mos
		double throughputPerSecond = overallThroughput / simulationDuration;
		double meanDelay = (nbDelays > 0) ? (sumDelays / (double)nbDelays) : 0;
		double pktDropRatio = (overallPacketsSent > 0) ? ((double)(overallPacketsSent - overallPacketsReceived) / (double)overallPacketsSent) : 0;
		double jitter = calculateMeanJitter();

		ss << VoipConfigs::nbVoipClients << " " << VoipConfigs::nbDataClients << " " << VoipConfigs::codec << " " << Ieee80211eMac::aifsn3
				<< " " << Ieee80211eMac::txop3 << " " << meanDelay << " " << jitter << " " << throughputPerSecond << " " << pktDropRatio;

		RawStat s = RawStat("voipStats", "event", 1);
		s.log(simTime().dbl(), 777, ss.str());
	}
}

