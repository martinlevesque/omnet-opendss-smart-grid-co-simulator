//
// UDPBasicApp.h : Modifié pour générer du trafic Voip.
//
// Auteurs : Martin Lévesque et François Lévesque
//
// Date : Novembre 2011

#ifndef __INET_UDPBASICAPP_H
#define __INET_UDPBASICAPP_H

#include <vector>
#include <map>
#include <string>
#include <omnetpp.h>
#include "UDPAppBase.h"


/**
 * UDP application. See NED for more info.
 */
class INET_API UDPBasicApp : public UDPAppBase
{
  public:

  protected:
    std::string nodeName;
    int localPort, destPort;
    std::vector<IPvXAddress> destAddresses;

    static long overallPacketsSent;
    static long overallPacketsReceived;

    double overallThroughput;
    double sumDelays;
    long nbDelays;
    std::map<std::string, std::vector<double> > arrivals;
    long messageLength;

    static int counter; // counter for generating a global number for each packet

    int numSent;
    int numReceived;

    // chooses random destination address
    virtual IPvXAddress chooseDestAddr();
    virtual cPacket *createPacket();
    virtual void sendPacket();
    virtual void processPacket(cPacket *msg);
    virtual void finish();

    double getNextArrivalTime();

    double calculateMeanJitter();
    double calculateMOS(double latency, double jitter, double pktLoss);

  protected:
    virtual int numInitStages() const {return 4;}
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
};

#endif


