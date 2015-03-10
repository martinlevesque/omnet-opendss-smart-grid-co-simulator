#include <omnetpp.h>
#include <string.h>
#include <cmessage.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <UDPAppBase.h>
#include <IPvXAddress.h>
#include <IPAddressResolver.h>
#include "akiMsg_m.h"
#include <UDPEchoApp.h>
#include "UDPControlInfo_m.h"
#include <BasicModule.h>
#include "AkiControlInfo_m.h"
#include <UDPSocket.h>
#include "AkiPacket_m.h"
#include <string>
#include <cstring>



class Aki : public UDPAppBase
{
    IPAddressResolver* myResolver;

  private:
    cMessage* timer;
    cMessage* akiPacketTimer;
    cOutVector endToEndDelayVector;
    cOutVector dataRateVector;
    cOutVector receiverNumberVector;
    cOutVector packetLossVector;

  protected:
    virtual void initialize();
    virtual void sendTimer();
    virtual void handleMessage(cMessage *msg);
    virtual AkiPacket *createPacket();
    IPvXAddress* chooseDestAddr();
    virtual void processPacket(cPacket *msg);
    void sendToUDP(cPacket *msg, int srcPort, const IPvXAddress srcAddr , const IPvXAddress& destAddr, int destPort);
    void returnPacket(cMessage *msg);

  public:
    int localPort;
    int destPort;
    int akitimer;
    double messtimer;
    int anfangspaket;
    int counter;
    IPvXAddress* srcAddr;
    IPvXAddress* destAddr;
    IPvXAddress* destAddress;
    int msgByteLength;
    long numReceived;
    long numSent;   //anzahl der gesendeten pakete innerhlab des timer-intervalls
    long allSent;   //anzahl aller gesendeten pakete
    long packetLoss;
    bool isSender;
    int receiverId;
    int numClients;
};

Define_Module( Aki );
