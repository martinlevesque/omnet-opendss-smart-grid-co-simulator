#include <aki.h>
//#include "cstringtokenizer.h"


void Aki::initialize()
{
    myResolver = new IPAddressResolver();
    ev << this->getFullPath() << ": INIT\n";

    counter = 0;
    numSent = 0;
    allSent = 0;
    numReceived = 0;
    packetLoss = 0;
    WATCH(numSent);
    WATCH(numReceived);
    localPort = par("local_port");
    destPort = par("dest_port");
    akitimer = par("aki_timer");
    messtimer = par("mess_timer");
    numClients = par("num_Clients");
    anfangspaket = par("anfangs_paket");
    isSender = par("is_Sender");
    receiverId = par("receiversAdressId");
    msgByteLength = par("message_length").longValue() / 8; // FIXME param should be in bytes instead
    destAddress = new IPvXAddress(par("dest_addresses"));

    endToEndDelayVector.setName("End-to-End-Delay");
    dataRateVector.setName("DataRate");
    receiverNumberVector.setName("NumReceived");
    packetLossVector.setName("Packet-Loss");

    ev << this->getFullPath() << ": INIT DEST-ADDRESS IS " << *destAddress<< "\n";

    sendTimer();                              //Funktion um Timer für Messintervalle aufrufen zu können
    bindToPort(localPort);                      //ports öffnen sich

    if (this->isSender == true)
    {
        ev << "MY INDEX IS: " << (this->getParentModule())->getIndex() << "\n";

        akiPacketTimer = new cMessage("AkiPacketTimer");               //hier wird erstes Self-Message verschickt, um dann das AkiPacket verschicken zu können
        scheduleAt((double) par("message_freq") + anfangspaket, akiPacketTimer);

        ev << "SELFMESSAGE SEND TO (IN INIT): " << this->getFullPath() << "\n";
    }
    else
    {
        ev << "MY INDEX IS: " << (this->getParentModule())->getIndex()
        << "\t NOTHING TO DO...\n";
    }
}

void Aki::handleMessage(cMessage *msg)
{
    ev << this->getFullPath() << ": RECEIVED MESSAGE \t" << msg << "\n";

    if (msg == timer)                                           //Wenn msg = Timer für Messintervalle
    {
        ev << "DIE ANZAHL DER INSGESAMT GESENDETEN PAKETE BETRÄGT: "<<allSent<<"\n";
        ev << "DIE ANZAHL DER GESENDETEN PAKETE INNERHALB DES TIMERS BETRÄGT: "<<numSent<<"\n";
        ev << "DIE ANZAHL DER INSGESAMT VERLORENEN PAKETE BETRÄGT: "<<packetLoss<<"\n";

        dataRateVector.record((numSent/messtimer)*msgByteLength);                 //VEKTORAUSGABE
        receiverNumberVector.record((numReceived/messtimer)*msgByteLength);       //VEKTORAUSGABE

        numReceived = 0;
        numSent = 0;
        scheduleAt(simTime() + messtimer, timer);
    }

    else if (msg == akiPacketTimer)                 //Wenn msg = Timer um AkiPacket zu schicken
    {
        if (this->isSender == true)                   //wenn ich akiPacketTimer bekomme und ich bin sender
        {

            IPvXAddress src;
            IPvXAddress dest;

            std::string srcAddress = this->getParentModule()->getName(); //  "vita[";  //dynamische "src"-Auswahl
            //ev << "VITALI: " << this->getParentModule()->getName()<< endl;;
            char numClientschar[5];
            sprintf(numClientschar, "%i",(this->getParentModule()->getIndex()));
            srcAddress.append("[");
            srcAddress.append(numClientschar);
            srcAddress.append("]");
            src = myResolver->resolve(srcAddress.c_str());

            std::string connectAddress =  "vita[0]";            //dynamische "dest"-Auswahl
//              char receiverIdchar[5];
//              sprintf(receiverIdchar, "%i",receiverId);
//              connectAddress.append(receiverIdchar);
//              connectAddress.append("]");
            dest = myResolver->resolve(connectAddress.c_str());

            AkiPacket* myAkiMessage = createPacket();
            sendToUDP(myAkiMessage, localPort, src, dest, destPort);

            endToEndDelayVector.record(simTime() - msg->getCreationTime());       //VEKTORAUSGABE

            cancelEvent(akiPacketTimer);
            scheduleAt(simTime() + akitimer, akiPacketTimer);                           //jede sekunde wird AkiPacket verschickt!!!!!!!!!!!
            ev << "ZEITPUNKT, AN DEM DAS "<<allSent<< ". AkiPacket GESENDET WIRD: " << simTime()
            << ". SEKUNDE!!!\n";
            myAkiMessage = NULL;
        }
        else                                                  //wenn ich akiPacketTimer bekomme und ich bin kein sender
        {
            delete msg;
        }
    }
    else if (this->isSender == true)    //ich bin sender und bekomme ein paket zurück(keine timer), dann kann es sich nur um
    {
        std::string s = "ERROR";    //ein Paket von einem anderen Modul handeln, der es zurückschickt und die gelöscht wird#
        if (msg->getName()!=s)
        {
            delete msg;
        }
        else
        {
            packetLoss++;
            ev << "PAKETVERLUST BETRÄGT: " << packetLoss << "\n";
            packetLossVector.record(packetLoss);                                  //VEKTORAUSGABE
            delete msg;
        }
    }
    else
    {
        cPacket* pckt = check_and_cast<cPacket *>(msg);    //bin kein sender und bekomme keinen timer, sondern ein anderes paket
        processPacket(pckt);                              //und schicke es zurück
    }
}


void Aki::returnPacket(cMessage *msg)
{
    AkiPacket* myAkiMsg = createPacket();
    UDPControlInfo *controlInfo = check_and_cast<UDPControlInfo *>(msg->getControlInfo());

    localPort = controlInfo->getDestPort();
    IPvXAddress src = controlInfo->getDestAddr();
    IPvXAddress dest = controlInfo->getSrcAddr();
    destPort = controlInfo->getSrcPort();

    ev << "ANGEKOMMENE NACHRICHTEN = " << numReceived << "\n";
    sendToUDP(myAkiMsg, localPort, src, dest, destPort);                 //Packet wird zurück zu Modul[0] geschickt
    delete msg;
}


AkiPacket *Aki::createPacket()
{
    char msgName[32];
    sprintf(msgName, "VITAPacket");
    AkiPacket *payload = new AkiPacket(msgName);
    payload->setByteLength(msgByteLength);
    return payload;
}


IPvXAddress* Aki::chooseDestAddr()
{
    return destAddress;
}


void Aki::processPacket(cPacket *msg)
{
    AkiPacket* myAkiPckt = dynamic_cast<AkiPacket *>(msg);
    if ( myAkiPckt != NULL)
    {
        ev << this->getFullPath() << ": SCHICKT PAKET " << myAkiPckt << " ZURÜCK.\n";
        returnPacket(myAkiPckt);      //ich bekomme keinen timer, sondern ein anderes paket und schicke es zurück
        numReceived++;
    }
    else delete msg;        //paketloss hochzählen !!!!

}


void Aki::sendTimer()                                       //Timer für Messintervalle
{
    timer = new cMessage("Messinterval");
    scheduleAt(messtimer, timer);
}


void Aki::sendToUDP(cPacket *msg, int srcPort, const IPvXAddress srcAddr,
                    const IPvXAddress& destAddr, int destPort)
{
    msg->setKind(UDP_C_DATA);
    UDPControlInfo *ctrl = new UDPControlInfo();

    ctrl->setSrcPort(srcPort);
    ctrl->setDestAddr(destAddr);
    ctrl->setDestPort(destPort);
    ctrl->setSrcAddr(srcAddr);
    msg->setControlInfo(ctrl);

    EV<< "Sending packet: "<<msg<<"\n";
    send(msg, "udpOut");
    numSent++;
    allSent++;
}

