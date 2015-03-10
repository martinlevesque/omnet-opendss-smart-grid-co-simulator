//
// Copyright (C) 2006 Andras Varga and Levente Meszaros
// Copyright (C) 2009 Lukáš Hlůže   lukas@hluze.cz (802.11e)
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

#include <algorithm>
#include "Ieee80211egMac.h"
#include "RadioState.h"
#include "IInterfaceTable.h"
#include "InterfaceTableAccess.h"
#include "PhyControlInfo_m.h"

Define_Module(Ieee80211egMac);

// don't forget to keep synchronized the C++ enum and the runtime enum definition
Register_Enum(Ieee80211egMac,
              (Ieee80211egMac::IDLE,
               Ieee80211egMac::DEFER,
               Ieee80211egMac::WAITAIFS,
               Ieee80211egMac::BACKOFF,
               Ieee80211egMac::WAITACK,
               Ieee80211egMac::WAITBROADCAST,
               Ieee80211egMac::WAITCTS,
               Ieee80211egMac::WAITSIFS,
               Ieee80211egMac::RECEIVE));

// don't forget to keep synchronized the C++ enum and the runtime enum definition
Register_Enum(RadioState,
              (RadioState::IDLE,
               RadioState::RECV,
               RadioState::TRANSMIT,
               RadioState::SLEEP));

/****************************************************************
 * Construction functions.
 */

Ieee80211egMac::Ieee80211egMac()
{
    endSIFS = NULL;
    endDIFS = NULL;
    endBackoff[0] = NULL;
    endBackoff[1] = NULL;
    endBackoff[2] = NULL;
    endBackoff[3] = NULL;
    endAIFS[0] = NULL;
    endAIFS[1] = NULL;
    endAIFS[2] = NULL;
    endAIFS[3] = NULL;
    endTimeout = NULL;
    endReserve = NULL;
    mediumStateChange = NULL;
    pendingRadioConfigMsg = NULL;
}

Ieee80211egMac::~Ieee80211egMac()
{
    cancelAndDelete(endSIFS);
    cancelAndDelete(endDIFS);
    cancelAndDelete(endAIFS[0]);
    cancelAndDelete(endAIFS[1]);
    cancelAndDelete(endAIFS[2]);
    cancelAndDelete(endAIFS[3]);
    cancelAndDelete(endBackoff[0]);
    cancelAndDelete(endBackoff[1]);
    cancelAndDelete(endBackoff[2]);
    cancelAndDelete(endBackoff[3]);
    cancelAndDelete(endTimeout);
    cancelAndDelete(endReserve);
    cancelAndDelete(mediumStateChange);
    cancelAndDelete(endTXOP);

    if (pendingRadioConfigMsg)
        delete pendingRadioConfigMsg;
    for (int i=0; i<=3; i++)
    {
        while (!transmissionQueue[i].empty())
        {
            Ieee80211Frame *temp = transmissionQueue[i].front();
            transmissionQueue[i].pop_front();
            delete temp;
        }
    }
}

/****************************************************************
 * Initialization functions.
 */
void Ieee80211egMac::initialize(int stage)
{
    WirelessMacBase::initialize(stage);

    if (stage == 0)
    {
        EV << "Initializing stage 0\n";

        // initialize parameters
        // Variable to apply the fsm fix
        fixFSM = par("fixFSM");
        opMode=par("opMode");
        if (opMode==1)
            opMode='b';
        else if (opMode==2)
            opMode='g';
        else
            opMode='b';

        if (opMode=='g')
        {//added by sorin
            PHY_HEADER_LENGTH=par("PHY_HEADER_LENGTH");//26us
            if (PHY_HEADER_LENGTH<0)
                PHY_HEADER_LENGTH=26e-6;//26us
        }
        else
        {
            opMode='b';//802.11b
            PHY_HEADER_LENGTH=192;//192bits
        }
        EV<<"Operating mode: 802.11"<<opMode;
        maxQueueSize = par("maxQueueSize");
        rtsThreshold = par("rtsThresholdBytes");

        // the variable is renamed due to a confusion in the standard
        // the name retry limit would be misleading, see the header file comment
        transmissionLimit = par("retryLimit");
        if (transmissionLimit == -1) transmissionLimit = 7;
        ASSERT(transmissionLimit >= 0);

        EV<<" retryLimit="<<transmissionLimit;

        cwMinData = par("cwMinData");
        if (cwMinData == -1) cwMinData = CW_MIN;
        ASSERT(cwMinData >= 0 && cwMinData <= 32767);

        cwMaxData = par("cwMaxData");
        if (cwMaxData == -1) cwMaxData = CW_MAX;
        ASSERT(cwMaxData >= 0 && cwMaxData <= 32767);

        cwMinBroadcast = par("cwMinBroadcast");
        if (cwMinBroadcast == -1) cwMinBroadcast = 31;
        ASSERT(cwMinBroadcast >= 0);
        EV<<" cwMinBroadcast="<<cwMinBroadcast;

        defaultAC = par("defaultAC");
        AIFSN[0] = par("AIFSN0");
        AIFSN[1] = par("AIFSN1");
        AIFSN[2] = par("AIFSN2");
        AIFSN[3] = par("AIFSN3");
        TXOP[0] = par("TXOP0");
        TXOP[1] = par("TXOP1");
        TXOP[2] = par("TXOP2");
        TXOP[3] = par("TXOP3");

        int i;
        for (i = 0; i < 4; i++)
        {
            ASSERT(AIFSN[i] >= 0 && AIFSN[i] < 16);
            if (i == 0 || i == 1)
            {
                cwMin[i] = cwMinData;
                cwMax[i] = cwMaxData;
            }
            if (i == 2)
            {
                cwMin[i] = (cwMinData + 1) / 2 - 1;
                cwMax[i] = cwMinData;
            }
            if (i == 3)
            {
                cwMin[i] = (cwMinData + 1) / 4 - 1;
                cwMax[i] = (cwMinData + 1) / 2 - 1;
            }
        }

        ST=par("slotTime");//added by sorin
        if (ST==-1)
            ST=20e-6;//20us
        EV<<" slotTime="<<ST*1e6<<"us DIFS="<< getDIFS()*1e6<<"us";

        basicBitrate = par("basicBitrate");
        if (basicBitrate==-1)
            basicBitrate=1e6;//1Mbps
        EV<<" basicBitrate="<<basicBitrate/1e6<<"M";

        bitrate = par("bitrate");
        if (bitrate==-1)
            bitrate=11e6;//11Mbps
        EV<<" bitrate="<<bitrate/1e6<<"M IDLE="<<IDLE<<" RECEIVE="<<RECEIVE<<endl;

        const char *addressString = par("address");
        if (!strcmp(addressString, "auto"))
        {
            // assign automatic address
            address = MACAddress::generateAutoAddress();
            // change module parameter from "auto" to concrete address
            par("address").setStringValue(address.str().c_str());
        }
        else
            address.setAddress(addressString);

        // subscribe for the information of the carrier sense
        nb->subscribe(this, NF_RADIOSTATE_CHANGED);

        // initalize self messages
        endSIFS = new cMessage("SIFS");
        endDIFS = new cMessage("DIFS");
        endAIFS[0] = new cMessage("AIFS", 0);
        endAIFS[1] = new cMessage("AIFS", 1);
        endAIFS[2] = new cMessage("AIFS", 2);
        endAIFS[3] = new cMessage("AIFS", 3);
        endTXOP = new cMessage("TXOP");
        endBackoff[0] = new cMessage("Backoff", 0);
        endBackoff[1] = new cMessage("Backoff", 1);
        endBackoff[2] = new cMessage("Backoff", 2);
        endBackoff[3] = new cMessage("Backoff", 3);
        endTimeout = new cMessage("Timeout");
        endReserve = new cMessage("Reserve");
        mediumStateChange = new cMessage("MediumStateChange");

        // interface
        registerInterface();

        // obtain pointer to external queue
        initializeQueueModule();

        // state variables
        fsm.setName("Ieee80211egMac State Machine");
        mode = DCF;
        sequenceNumber = 0;
        radioState = RadioState::IDLE;
        retryCounter[0] = 0;
        retryCounter[1] = 0;
        retryCounter[2] = 0;
        retryCounter[3] = 0;
        backoffPeriod[0] = -1;
        backoffPeriod[1] = -1;
        backoffPeriod[2] = -1;
        backoffPeriod[3] = -1;
        backoff[0] = false;
        backoff[1] = false;
        backoff[2] = false;
        backoff[3] = false;
        currentAC = 0;
        oldcurrentAC = 0;
        lastReceiveFailed = false;
        numDropped[0]=0;
        numDropped[1]=0;
        numDropped[2]=0;
        numDropped[3]=0;
        nav = false;
        txop = false;
        last =0;

        // statistics
        numRetry[0] = 0;
        numRetry[1] = 0;
        numRetry[2] = 0;
        numRetry[3] = 0;
        numSentWithoutRetry[0] = 0;
        numSentWithoutRetry[1] = 0;
        numSentWithoutRetry[2] = 0;
        numSentWithoutRetry[3] = 0;
        numGivenUp[0] = 0;
        numGivenUp[1] = 0;
        numGivenUp[2] = 0;
        numGivenUp[3] = 0;
        numCollision = 0;
        numInternalCollision = 0;
        numSent[0] = 0;
        numSent[1] = 0;
        numSent[2] = 0;
        numSent[3] = 0;
        numReceived = 0;
        numSentBroadcast = -1;//sorin
        numReceivedBroadcast = 0;
        numBites = 0;
        bites[0] =0;
        maxjitter[0]=0;
        minjitter[0]=0;
        bites[1] =0;
        maxjitter[1]=0;
        minjitter[1]=0;
        bites[2] =0;
        maxjitter[2]=0;
        minjitter[2]=0;
        bites[3] =0;
        maxjitter[3]=0;
        minjitter[3]=0;
        numSentTXOP = 0;
        numReceivedOther = 0;
        numAckSend = 0;
        stateVector.setName("State");
        stateVector.setEnum("Ieee80211egMac");
        radioStateVector.setName("RadioState");
        radioStateVector.setEnum("RadioState");
        throughput[0].setName("throughput AC0");
        throughput[1].setName("throughput AC1");
        throughput[2].setName("throughput AC2");
        throughput[3].setName("throughput AC3");
        macDelay[0].setName("Mac delay AC0");
        macDelay[1].setName("Mac delay AC1");
        macDelay[2].setName("Mac delay AC2");
        macDelay[3].setName("Mac delay AC3");
        jitter[0].setName("jitter AC0");
        jitter[1].setName("jitter AC1");
        jitter[2].setName("jitter AC2");
        jitter[3].setName("jitter AC3");

        // initialize watches
        WATCH(fsm);
        WATCH(radioState);
        WATCH(retryCounter[0]);
        WATCH(retryCounter[1]);
        WATCH(retryCounter[2]);
        WATCH(retryCounter[3]);
        WATCH(backoff[0]);
        WATCH(backoff[1]);
        WATCH(backoff[2]);
        WATCH(backoff[3]);
        WATCH(backoffPeriod[0]);
        WATCH(backoffPeriod[1]);
        WATCH(backoffPeriod[2]);
        WATCH(backoffPeriod[3]);
        WATCH(currentAC);
        WATCH(oldcurrentAC);
        WATCH_LIST(transmissionQueue[0]);
        WATCH_LIST(transmissionQueue[1]);
        WATCH_LIST(transmissionQueue[2]);
        WATCH_LIST(transmissionQueue[3]);
        WATCH(nav);
        WATCH(txop);

        WATCH(numRetry[0]);
        WATCH(numRetry[1]);
        WATCH(numRetry[2]);
        WATCH(numRetry[3]);
        WATCH(numSentWithoutRetry[0]);
        WATCH(numSentWithoutRetry[1]);
        WATCH(numSentWithoutRetry[2]);
        WATCH(numSentWithoutRetry[3]);
        WATCH(numGivenUp[0]);
        WATCH(numGivenUp[1]);
        WATCH(numGivenUp[2]);
        WATCH(numGivenUp[3]);
        WATCH(numCollision);
        WATCH(numSent[0]);
        WATCH(numSent[1]);
        WATCH(numSent[2]);
        WATCH(numSent[3]);
        WATCH(numBites);
        WATCH(numSentTXOP);
        WATCH(numReceived);
        WATCH(numSentBroadcast);
        WATCH(numReceivedBroadcast);
        WATCH(numDropped[0]);
        WATCH(numDropped[1]);
        WATCH(numDropped[2]);
        WATCH(numDropped[3]);
        radioModule = gate("lowergateOut")->getNextGate()->getOwnerModule()->getId();
    }
}
void Ieee80211egMac::finish()
{
    recordScalar("number of received packets", numReceived);
    recordScalar("number of collisions", numCollision);
    recordScalar("number of internal collisions", numInternalCollision);
    recordScalar("number of retry for AC 0", numRetry[0]);
    recordScalar("number of retry for AC 1", numRetry[1]);
    recordScalar("number of retry for AC 2", numRetry[2]);
    recordScalar("number of retry for AC 3", numRetry[3]);
    recordScalar("sent and receive bites", numBites);
    recordScalar("sent packet within AC 0", numSent[0]);
    recordScalar("sent packet within AC 1", numSent[1]);
    recordScalar("sent packet within AC 2", numSent[2]);
    recordScalar("sent packet within AC 3", numSent[3]);
    recordScalar("sent in TXOP ", numSentTXOP );
    recordScalar("sentWithoutRetry AC 0",numSentWithoutRetry[0] );
    recordScalar("sentWithoutRetry AC 1",numSentWithoutRetry[1] );
    recordScalar("sentWithoutRetry AC 2",numSentWithoutRetry[2] );
    recordScalar("sentWithoutRetry AC 3",numSentWithoutRetry[3] );
    recordScalar("numGivenUp AC 0", numGivenUp[0] );
    recordScalar("numGivenUp AC 1", numGivenUp[1] );
    recordScalar("numGivenUp AC 2", numGivenUp[2] );
    recordScalar("numGivenUp AC 3", numGivenUp[3] );
    recordScalar("numDropped AC 0", numDropped[0] );
    recordScalar("numDropped AC 1", numDropped[1] );
    recordScalar("numDropped AC 2", numDropped[2] );
    recordScalar("numDropped AC 3", numDropped[3] );
}
void Ieee80211egMac::registerInterface()
{
    IInterfaceTable *ift = InterfaceTableAccess().getIfExists();
    if (!ift)
        return;

    InterfaceEntry *e = new InterfaceEntry();

    // interface name: NetworkInterface module's name without special characters ([])
    char *interfaceName = new char[strlen(getParentModule()->getFullName()) + 1];
    char *d = interfaceName;
    for (const char *s = getParentModule()->getFullName(); *s; s++)
        if (isalnum(*s))
            *d++ = *s;
    *d = '\0';

    e->setName(interfaceName);
    delete [] interfaceName;

    // address
    e->setMACAddress(address);
    e->setInterfaceToken(address.formInterfaceIdentifier());

    // FIXME: MTU on 802.11 = ?
    e->setMtu(par("mtu"));

    // capabilities
    e->setBroadcast(true);
    e->setMulticast(true);
    e->setPointToPoint(false);

    // add
    ift->addInterface(e, this);
}

void Ieee80211egMac::initializeQueueModule()
{
    // use of external queue module is optional -- find it if there's one specified
    if (par("queueModule").stringValue()[0])
    {
        cModule *module = getParentModule()->getSubmodule(par("queueModule").stringValue());
        queueModule = check_and_cast<IPassiveQueue *>(module);

        EV << "Requesting first two frames from queue module\n";
        queueModule->requestPacket();
        // needed for backoff: mandatory if next message is already present
        queueModule->requestPacket();
    }
}

/****************************************************************
 * Message handling functions.
 */
void Ieee80211egMac::handleSelfMsg(cMessage *msg)
{
    EV << "received self message: " << msg << "(kind: " << msg->getKind() << ")" << endl;

    if (msg == endReserve)
        nav = false;

    if (msg == endTXOP)
        txop = false;

    if ( !strcmp(msg->getName(),"AIFS") || !strcmp(msg->getName(),"Backoff") )
    {
        EV << "Changing currentAC to " << msg->getKind() << endl;
        currentAC = msg->getKind();
    }
    //check internal colision
    if ( (strcmp(msg->getName(),"Backoff")== 0) || (strcmp(msg->getName(),"AIFS")==0))
    {
        int kind,i;
        kind = msg->getKind();
        EV <<" kind is " << kind << ",name is " << msg->getName() <<endl;
        for (i = 3; i > kind; i--)  //mozna prochaze jen 3..kind XXX
        {
            if (((endBackoff[i]->isScheduled() &&  endBackoff[i]->getArrivalTime() == simTime())
                    || (endAIFS[i]->isScheduled() && !backoff[i] &&endAIFS[i]->getArrivalTime() == simTime()))
                    && !transmissionQueue[i].empty())
            {
                EV << "Internal collision AC" << kind << " with AC" << i << endl;
                numInternalCollision++;
                EV << "Cancel backoff event and schedule new one for AC" << kind << endl;
                cancelEvent(endBackoff[kind]);
                if (retryCounter[currentAC] == transmissionLimit - 1)
                {
                    EV << "give up transmission for AC" << currentAC << endl;
                    giveUpCurrentTransmission();
                }
                else
                {
                    EV << "retry transmission for AC" << currentAC << endl;
                    retryCurrentTransmission();
                }
                return;
            }
        }
    }
    handleWithFSM(msg);
}


void Ieee80211egMac::handleUpperMsg(cPacket *msg)
{
    if (queueModule)
    {
        // the module are continuously asking for packets
        EV << "requesting another frame from queue module\n";
        queueModule->requestPacket();
    }

    // check if it's a command from the mgmt layer
    if (msg->getBitLength()==0 && msg->getKind()!=0)
    {
        handleCommand(msg);
        return;
    }

    // must be a Ieee80211DataOrMgmtFrame, within the max size because we don't support fragmentation
    Ieee80211DataOrMgmtFrame *frame = check_and_cast<Ieee80211DataOrMgmtFrame *>(msg);
    if (frame->getByteLength() > fragmentationThreshold)
        error("message from higher layer (%s)%s is too long for 802.11b, %d bytes (fragmentation is not supported yet)",
              msg->getClassName(), msg->getName(), (int)(msg->getByteLength()));
    EV << "frame " << frame << " received from higher layer, receiver = " << frame->getReceiverAddress() << endl;

    // if you get error from this assert check if is client associated to AP
    ASSERT(!frame->getReceiverAddress().isUnspecified());

    // fill in missing fields (receiver address, seq number), and insert into the queue
    frame->setTransmitterAddress(address);
    frame->setSequenceNumber(sequenceNumber);
    sequenceNumber = (sequenceNumber+1) % 4096;  //XXX seqNum must be checked upon reception of frames!

    if (MappingAccessCategory(frame) ==  200)
    {
        // if function MappingAccessCategory() returns 200, it means transsmissionQueue is full
        return;
    }
    frame->setMACArrive(simTime());
    handleWithFSM(frame);
}

int Ieee80211egMac::MappingAccessCategory(Ieee80211DataOrMgmtFrame *frame)
{
    bool isDataFrame = dynamic_cast<Ieee80211DataFrame *>(frame) != NULL;
    IPDatagram *ipdata = NULL;
    currentAC=defaultAC;

    if (isDataFrame)
    {
#if OMNETPP_VERSION > 0x0400
        ipdata = dynamic_cast<IPDatagram *>(frame->getEncapsulatedPacket());
#else
        ipdata = dynamic_cast<IPDatagram *>(frame->getEncapsulatedMsg());
#endif
    }

    if (!isDataFrame)
    {
        currentAC=3;
    }
    else if (ipdata)
    {
#if OMNETPP_VERSION > 0x0400
        ICMPMessage *icmp = dynamic_cast<ICMPMessage *>(ipdata->getEncapsulatedPacket());
        UDPPacket *udp = dynamic_cast<UDPPacket *>(ipdata->getEncapsulatedPacket());
        TCPSegment *tcp = dynamic_cast<TCPSegment *>(ipdata->getEncapsulatedPacket());
#else
        ICMPMessage *icmp = dynamic_cast<ICMPMessage *>(ipdata->getEncapsulatedMsg());
        UDPPacket *udp = dynamic_cast<UDPPacket *>(ipdata->getEncapsulatedMsg());
        TCPSegment *tcp = dynamic_cast<TCPSegment *>(ipdata->getEncapsulatedMsg());
#endif

        EV << "We obtain ip datagram, going to recognize access category.\n";
        if (icmp)
        {
            EV << "We recognize icmp message in ip datagram.\n";
            currentAC = 1;
        }
        else if (udp)
        {
            EV << "We reconize udp packet.\n";
            if (udp->getDestinationPort() == 5000 || udp->getSourcePort() == 5000)  //voice
            {
                currentAC = 3;
            }
            if (udp->getDestinationPort() == 4000 || udp->getSourcePort() == 4000)  //video
            {
                currentAC = 2;
            }
            if (udp->getDestinationPort() == 80 || udp->getSourcePort() == 80)  //voice
            {
                currentAC = 1;
            }
            if (udp->getDestinationPort() == 21 || udp->getSourcePort() == 21)  //voice
            {
                currentAC = 0;
            }
        }
        else if (tcp)
        {
            EV << "We recognize tcp segment.\n";
            if (tcp->getDestPort() == 80 || tcp->getSrcPort() == 80)
            {
                currentAC = 1;
            }
            if (tcp->getDestPort() == 21 || tcp->getSrcPort() == 21)
            {
                currentAC = 0;
            }
            if (tcp->getDestPort() == 4000 || tcp->getSrcPort() == 4000)
            {
                currentAC = 2;
            }
            if (tcp->getDestPort() == 5000 || tcp->getSrcPort() == 5000)
            {
                currentAC = 3;
            }
        }
        else
        {

        };
    }
    // check for queue overflow
    if (isDataFrame && maxQueueSize && (int)transmissionQueue[currentAC].size() >= maxQueueSize)
    {
        EV << "message " << frame << " received from higher layer but AC queue is full, dropping message\n";
        numDropped[currentAC]++;
        delete frame;
        return 200;
    }
    if (isDataFrame)
    {
        transmissionQueue[currentAC].push_back(frame);
    }
    else
    {
        if (transmissionQueue[currentAC].empty() || transmissionQueue[currentAC].size() == 1)
        {
            transmissionQueue[currentAC].push_back(frame);
        }
        else
        {
            std::list<Ieee80211DataOrMgmtFrame*>::iterator p;
            //we don't know if first frame in the queue is in middle of transmission
            //so for sure we placed it on second place
            p=transmissionQueue[currentAC].begin();
            p++;
            transmissionQueue[currentAC].insert(p,frame);
        }
    }
    EV << "frame classified as access category "<< currentAC <<" (0 background, 1 best effort, 2 video, 3 voice)\n";
    return true;
}

void Ieee80211egMac::handleCommand(cMessage *msg)
{
    if (msg->getKind()==PHY_C_CONFIGURERADIO)
    {
        EV << "Passing on command " << msg->getName() << " to physical layer\n";
        if (pendingRadioConfigMsg != NULL)
        {
            // merge contents of the old command into the new one, then delete it
            PhyControlInfo *pOld = check_and_cast<PhyControlInfo *>(pendingRadioConfigMsg->getControlInfo());
            PhyControlInfo *pNew = check_and_cast<PhyControlInfo *>(msg->getControlInfo());
            if (pNew->getChannelNumber()==-1 && pOld->getChannelNumber()!=-1)
                pNew->setChannelNumber(pOld->getChannelNumber());
            if (pNew->getBitrate()==-1 && pOld->getBitrate()!=-1)
                pNew->setBitrate(pOld->getBitrate());
            delete pendingRadioConfigMsg;
            pendingRadioConfigMsg = NULL;
        }

        if (fsm.getState() == IDLE || fsm.getState() == DEFER || fsm.getState() == BACKOFF)
        {
            EV << "Sending it down immediately\n";
            sendDown(msg);
        }
        else
        {
            EV << "Delaying " << msg->getName() << " until next IDLE or DEFER state\n";
            pendingRadioConfigMsg = msg;
        }
    }
    else
    {
        error("Unrecognized command from mgmt layer: (%s)%s msgkind=%d", msg->getClassName(), msg->getName(), msg->getKind());
    }
}

void Ieee80211egMac::handleLowerMsg(cPacket *msg)
{
    EV<<"->Enter handleLowerMsg...\n";
    EV << "received message from lower layer: " << msg << endl;

    nb->fireChangeNotification(NF_LINK_FULL_PROMISCUOUS, msg);

    if (msg->getControlInfo())
        delete msg->removeControlInfo();

    Ieee80211Frame *frame = dynamic_cast<Ieee80211Frame *>(msg);
    if (!frame)
    {
#ifdef FRAMETYPESTOP
        error("message from physical layer (%s)%s is not a subclass of Ieee80211Frame",msg->getClassName(), msg->getName());
#endif
        EV << "message from physical layer (%s)%s is not a subclass of Ieee80211Frame" << msg->getClassName() << " " << msg->getName() <<  endl;
        delete msg;
        return;
        // error("message from physical layer (%s)%s is not a subclass of Ieee80211Frame",msg->getClassName(), msg->getName());
    }

    EV << "Self address: " << address
    << ", receiver address: " << frame->getReceiverAddress()
    << ", received frame is for us: " << isForUs(frame)
    << ", received frame was sent by us: " << isSentByUs(frame)<<endl;

    Ieee80211TwoAddressFrame *twoAddressFrame = dynamic_cast<Ieee80211TwoAddressFrame *>(msg);
    ASSERT(!twoAddressFrame || twoAddressFrame->getTransmitterAddress() != address);

#ifdef LWMPLS
    int msgKind = msg->getKind();
    if (msgKind != COLLISION && msgKind != BITERROR && twoAddressFrame!=NULL)
        nb->fireChangeNotification(NF_LINK_REFRESH, twoAddressFrame);
#endif


    handleWithFSM(msg);

    // if we are the owner then we did not send this message up
    if (msg->getOwner() == this)
        delete msg;
    EV<<"Leave handleLowerMsg...\n";
}

void Ieee80211egMac::receiveChangeNotification(int category, const cPolymorphic *details)
{
    Enter_Method_Silent();
    printNotificationBanner(category, details);

    if (category == NF_RADIOSTATE_CHANGED)
    {
        RadioState * rstate = check_and_cast<RadioState *>(details);
        if (rstate->getRadioId()!=getRadioModuleId())
            return;

        RadioState::State newRadioState = rstate->getState();



        // FIXME: double recording, because there's no sample hold in the gui
        radioStateVector.record(radioState);
        radioStateVector.record(newRadioState);

        radioState = newRadioState;

        handleWithFSM(mediumStateChange);
    }
}

/**
 * Msg can be upper, lower, self or NULL (when radio state changes)
 */
void Ieee80211egMac::handleWithFSM(cMessage *msg)
{
    // skip those cases where there's nothing to do, so the switch looks simpler
    if (isUpperMsg(msg) && fsm.getState() != IDLE)
    {
        if (fsm.getState() == WAITAIFS && endDIFS->isScheduled())
        {
            // a difs was schedule because all queues ware empty
            // change difs for aifs
            simtime_t remaint = getAIFS(currentAC)-getDIFS();
            scheduleAt(endDIFS->getArrivalTime()+remaint, endAIFS[currentAC]);
            cancelEvent(endDIFS);
        }
        else if (fsm.getState() == BACKOFF && endBackoff[3]->isScheduled() &&  transmissionQueue[3].empty())
        {
            // a backoff was schedule with all the queues empty
            // reschedule the backoff with the appropriate AC
            backoffPeriod[currentAC]=backoffPeriod[3];
            backoff[currentAC]=backoff[3];
            backoff[3]=false;
            scheduleAt(endBackoff[3]->getArrivalTime(), endBackoff[currentAC]);
            cancelEvent(endBackoff[3]);
        }
        EV << "deferring upper message transmission in " << fsm.getStateName() << " state\n";
        return;
    }

    Ieee80211Frame *frame = dynamic_cast<Ieee80211Frame*>(msg);
    int frameType = frame ? frame->getType() : -1;
    int msgKind = msg->getKind();
    logState();
    stateVector.record(fsm.getState());

    if (frame && isLowerMsg(frame))
    {
        lastReceiveFailed =(msgKind == COLLISION || msgKind == BITERROR);
        scheduleReservePeriod(frame);
    }

    // TODO: fix bug according to the message: [omnetpp] A possible bug in the Ieee80211's FSM.
    FSMA_Switch(fsm)
    {
        FSMA_State(IDLE)
        {
            FSMA_Enter(sendDownPendingRadioConfigMsg());
            /*
            if (fixFSM)
            {
            FSMA_Event_Transition(Data-Ready,
                                  // isUpperMsg(msg),
                                  isUpperMsg(msg) && backoffPeriod[currentAC] > 0,
                                  DEFER,
                //ASSERT(isInvalidBackoffPeriod() || backoffPeriod == 0);
                //invalidateBackoffPeriod();
               ASSERT(false);

            );
            FSMA_No_Event_Transition(Immediate-Data-Ready,
                                     //!transmissionQueue.empty(),
                !transmissionQueueEmpty(),
                                     DEFER,
              //  invalidateBackoffPeriod();
                ASSERT(backoff[currentAC]);

            );
            }
            */
            FSMA_Event_Transition(Data-Ready,
                                  isUpperMsg(msg),
                                  DEFER,
                                  ASSERT(isInvalidBackoffPeriod() || backoffPeriod[currentAC] == 0);
                                  invalidateBackoffPeriod();
                                 );
            FSMA_No_Event_Transition(Immediate-Data-Ready,
                                     !transmissionQueueEmpty(),
                                     DEFER,
                                     invalidateBackoffPeriod();
                                    );
            FSMA_Event_Transition(Receive,
                                  isLowerMsg(msg),
                                  RECEIVE,
                                 );
        }
        FSMA_State(DEFER)
        {
            FSMA_Enter(sendDownPendingRadioConfigMsg());
            FSMA_Event_Transition(Wait-AIFS,
                                  isMediumStateChange(msg) && isMediumFree(),
                                  WAITAIFS,
                                  ;);
            FSMA_No_Event_Transition(Immediate-Wait-AIFS,
                                     isMediumFree() || ( !backoff[0] && !backoff[1] && !backoff[2] && !backoff[3]),
                                     WAITAIFS,
                                     ;);
            FSMA_Event_Transition(Receive,
                                  isLowerMsg(msg),
                                  RECEIVE,
                                  ;);
        }
        FSMA_State(WAITAIFS)
        {
            FSMA_Enter(scheduleAIFSPeriod());

            FSMA_Event_Transition(EDCAF-Do-Nothing,
                                  isMsgAIFS(msg) && transmissionQueue[currentAC].empty(),
                                  WAITAIFS,
                                  ASSERT(0==1);
                                  ;);
            FSMA_Event_Transition(Immediate-Transmit-RTS,
                                  isMsgAIFS(msg) && !transmissionQueue[currentAC].empty() && !isBroadcast(getCurrentTransmission())
                                  && getCurrentTransmission()->getByteLength() >= rtsThreshold && !backoff[currentAC],
                                  WAITCTS,
                                  sendRTSFrame(getCurrentTransmission());
                                  oldcurrentAC = currentAC;
                                  cancelAIFSPeriod();
                                 );
            FSMA_Event_Transition(Immediate-Transmit-Broadcast,
                                  isMsgAIFS(msg) && isBroadcast(getCurrentTransmission()) && !backoff[currentAC],
                                  WAITBROADCAST,
                                  sendBroadcastFrame(getCurrentTransmission());
                                  oldcurrentAC = currentAC;
                                  cancelAIFSPeriod();
                                 );
            FSMA_Event_Transition(Immediate-Transmit-Data,
                                  isMsgAIFS(msg) && !isBroadcast(getCurrentTransmission()) && !backoff[currentAC],
                                  WAITACK,
                                  sendDataFrame(getCurrentTransmission());
                                  oldcurrentAC = currentAC;
                                  cancelAIFSPeriod();
                                 );
            /*FSMA_Event_Transition(AIFS-Over,
                                  isMsgAIFS(msg) && backoff[currentAC],
                                  BACKOFF,
                if (isInvalidBackoffPeriod())
                    generateBackoffPeriod();
            );*/
            FSMA_Event_Transition(AIFS-Over,
                                  isMsgAIFS(msg),
                                  BACKOFF,
                                  if (isInvalidBackoffPeriod())
                                  generateBackoffPeriod();
                                 );
            // end the difs and no other packet has been received
            FSMA_Event_Transition(DIFS-Over,
                                  msg == endDIFS && transmissionQueueEmpty(),
                                  BACKOFF,
                                  currentAC = 3;
                                  if (isInvalidBackoffPeriod())
                                     generateBackoffPeriod();
                                   );
            FSMA_Event_Transition(DIFS-Over,
                                   msg == endDIFS,
                                   BACKOFF,
                                   for (int i=3;i>=0;i--)
                                   {
                                       if (!transmissionQueue[i].empty())
                                       {
                                            currentAC = i;
                                       }
                                   }
                                   if (isInvalidBackoffPeriod())
                                      generateBackoffPeriod();
                                    );
            FSMA_Event_Transition(Busy,
                                  isMediumStateChange(msg) && !isMediumFree(),
                                  DEFER,
                                  if (endAIFS[0]->isScheduled()) backoff[0] = true;
                                  if (endAIFS[1]->isScheduled()) backoff[1] = true;
                                  if (endAIFS[2]->isScheduled()) backoff[2] = true;
                                  if (endAIFS[3]->isScheduled()) backoff[3] = true;
                                  if (endDIFS->isScheduled()) backoff[3] = true;
                                           cancelAIFSPeriod();
                                      );
            FSMA_No_Event_Transition(Immediate-Busy,
                                     !isMediumFree(),
                                     DEFER,
                                     if (endAIFS[0]->isScheduled()) backoff[0] = true;
                                     if (endAIFS[1]->isScheduled()) backoff[1] = true;
                                     if (endAIFS[2]->isScheduled()) backoff[2] = true;
                                     if (endAIFS[3]->isScheduled()) backoff[3] = true;
                                     if (endDIFS->isScheduled()) backoff[3] = true;
                                             cancelAIFSPeriod();
                                     );
            // radio state changes before we actually get the message, so this must be here
            FSMA_Event_Transition(Receive,
                                  isLowerMsg(msg),
                                  RECEIVE,
                                  cancelAIFSPeriod();
                                  ;);
        }
        FSMA_State(BACKOFF)
        {
            FSMA_Enter(scheduleBackoffPeriod());
            if (getCurrentTransmission())
            {
                FSMA_Event_Transition(Transmit-RTS,
                                      msg == endBackoff[currentAC] && !isBroadcast(getCurrentTransmission())
                                      && getCurrentTransmission()->getByteLength() >= rtsThreshold,
                                      WAITCTS,
                                      sendRTSFrame(getCurrentTransmission());
                                      oldcurrentAC = currentAC;
                                      cancelAIFSPeriod();
                                      decreaseBackoffPeriod();
                                      cancelBackoffPeriod();
                                     );
                FSMA_Event_Transition(Transmit-Broadcast,
                                      msg == endBackoff[currentAC] && isBroadcast(getCurrentTransmission()),
                                      WAITBROADCAST,
                                      sendBroadcastFrame(getCurrentTransmission());
                                      oldcurrentAC = currentAC;
                                      cancelAIFSPeriod();
                                      decreaseBackoffPeriod();
                                      cancelBackoffPeriod();
                                     );
                FSMA_Event_Transition(Transmit-Data,
                                      msg == endBackoff[currentAC] && !isBroadcast(getCurrentTransmission()),
                                      WAITACK,
                                      sendDataFrame(getCurrentTransmission());
                                      oldcurrentAC = currentAC;
                                      cancelAIFSPeriod();
                                      decreaseBackoffPeriod();
                                      cancelBackoffPeriod();
                                     );
            }
            FSMA_Event_Transition(AIFS-Over-backoff,
                                  isMsgAIFS(msg) && backoff[currentAC],
                                  BACKOFF,
                                  if (isInvalidBackoffPeriod())
                                  generateBackoffPeriod();
                                 );
            FSMA_Event_Transition(AIFS-Immediate-Transmit-RTS,
                                  isMsgAIFS(msg) && !transmissionQueue[currentAC].empty() && !isBroadcast(getCurrentTransmission())
                                  && getCurrentTransmission()->getByteLength() >= rtsThreshold && !backoff[currentAC],
                                  WAITCTS,
                                  sendRTSFrame(getCurrentTransmission());
                                  oldcurrentAC = currentAC;
                                  cancelAIFSPeriod();
                                  decreaseBackoffPeriod();
                                  cancelBackoffPeriod();
                                 );
            FSMA_Event_Transition(AIFS-Immediate-Transmit-Broadcast,
                                  isMsgAIFS(msg) && isBroadcast(getCurrentTransmission()) && !backoff[currentAC],
                                  WAITBROADCAST,
                                  sendBroadcastFrame(getCurrentTransmission());
                                  oldcurrentAC = currentAC;
                                  cancelAIFSPeriod();
                                  decreaseBackoffPeriod();
                                  cancelBackoffPeriod();
                                 );
            FSMA_Event_Transition(AIFS-Immediate-Transmit-Data,
                                  isMsgAIFS(msg) && !isBroadcast(getCurrentTransmission()) && !backoff[currentAC],
                                  WAITACK,
                                  sendDataFrame(getCurrentTransmission());
                                  oldcurrentAC = currentAC;
                                  cancelAIFSPeriod();
                                  decreaseBackoffPeriod();
                                  cancelBackoffPeriod();
                                 );
            FSMA_Event_Transition(Backoff-Idle,
            		              (msg == endBackoff[0] || msg == endBackoff[1] || msg == endBackoff[2] || msg == endBackoff[3])  && transmissionQueueEmpty(),
                                  IDLE,
                                  resetStateVariables();
                                  );
            FSMA_Event_Transition(Backoff-Busy,
                                  isMediumStateChange(msg) && !isMediumFree(),
                                  DEFER,
                                  cancelAIFSPeriod();
                                  decreaseBackoffPeriod();
                                  cancelBackoffPeriod();
                                 );

        }
        FSMA_State(WAITACK)
        {
            FSMA_Enter(scheduleDataTimeoutPeriod(getCurrentTransmission()));
            FSMA_Event_Transition(Receive-ACK-TXOP,
                                  isLowerMsg(msg) && isForUs(frame) && frameType == ST_ACK && txop,
                                  WAITSIFS,
                                  currentAC=oldcurrentAC;
                                  if (retryCounter[currentAC] == 0) numSentWithoutRetry[currentAC]++;
                                  numSent[currentAC]++;
                                  fr=getCurrentTransmission();
                                  numBites += fr->getBitLength();
                                  bites[currentAC] += fr->getBitLength();


                                  macDelay[currentAC].record(simTime() - fr->getMACArrive());
                                  if (maxjitter[currentAC] == 0 || maxjitter[currentAC] < (simTime() - fr->getMACArrive())) maxjitter[currentAC]=simTime() - fr->getMACArrive();
                                      if (minjitter[currentAC] == 0 || minjitter[currentAC] > (simTime() - fr->getMACArrive())) minjitter[currentAC]=simTime() - fr->getMACArrive();
                                          EV << "record macDelay AC" << currentAC << " value " << simTime() - fr->getMACArrive() <<endl;
                                          numSentTXOP++;
                                          cancelTimeoutPeriod();

                                          finishCurrentTransmission();
                                         );
/*
            FSMA_Event_Transition(Receive-ACK,
                                  isLowerMsg(msg) && isForUs(frame) && frameType == ST_ACK,
                                  IDLE,
                                  currentAC=oldcurrentAC;
                                  if (retryCounter[currentAC] == 0) numSentWithoutRetry[currentAC]++;
                                  numSent[currentAC]++;
                                  fr=getCurrentTransmission();
                                  numBites += fr->getBitLength();
                                  bites[currentAC] += fr->getBitLength();

                                  macDelay[currentAC].record(simTime() - fr->getMACArrive());
                                  if (maxjitter[currentAC] == 0 || maxjitter[currentAC] < (simTime() - fr->getMACArrive())) maxjitter[currentAC]=simTime() - fr->getMACArrive();
                                      if (minjitter[currentAC] == 0 || minjitter[currentAC] > (simTime() - fr->getMACArrive())) minjitter[currentAC]=simTime() - fr->getMACArrive();
                                          EV << "record macDelay AC" << currentAC << " value " << simTime() - fr->getMACArrive() <<endl;

                                          cancelTimeoutPeriod();
                                          finishCurrentTransmission();
                                         );

             */
             /*Ieee 802.11 2007 9.9.1.2 EDCA TXOPs*/
             FSMA_Event_Transition(Receive-ACK,
                                  isLowerMsg(msg) && isForUs(frame) && frameType == ST_ACK,
                                  DEFER,
                                  currentAC=oldcurrentAC;
                                  if (retryCounter[currentAC] == 0) numSentWithoutRetry[currentAC]++;
                                  numSent[currentAC]++;
                                  fr=getCurrentTransmission();
                                  numBites += fr->getBitLength();
                                  bites[currentAC] += fr->getBitLength();

                                  macDelay[currentAC].record(simTime() - fr->getMACArrive());
                                  if (maxjitter[currentAC] == 0 || maxjitter[currentAC] < (simTime() - fr->getMACArrive())) maxjitter[currentAC]=simTime() - fr->getMACArrive();
                                  if (minjitter[currentAC] == 0 || minjitter[currentAC] > (simTime() - fr->getMACArrive())) minjitter[currentAC]=simTime() - fr->getMACArrive();
                                  EV << "record macDelay AC" << currentAC << " value " << simTime() - fr->getMACArrive() <<endl;
                                  cancelTimeoutPeriod();
                                  finishCurrentTransmission();
                                  resetCurrentBackOff();
                                  );
            FSMA_Event_Transition(Transmit-Data-Failed,
                                  msg == endTimeout && retryCounter[oldcurrentAC] == transmissionLimit - 1,
                                  IDLE,
                                  currentAC=oldcurrentAC;
                                  giveUpCurrentTransmission();
                                  txop = false;
                                  if (endTXOP->isScheduled()) cancelEvent(endTXOP);
                                 );
            FSMA_Event_Transition(Receive-ACK-Timeout,
                                  msg == endTimeout,
                                  DEFER,
                                  currentAC=oldcurrentAC;
                                  retryCurrentTransmission();
                                  txop = false;
                                  if (endTXOP->isScheduled()) cancelEvent(endTXOP);
                                 );
        }
        // wait until broadcast is sent
        FSMA_State(WAITBROADCAST)
        {
            FSMA_Enter(scheduleBroadcastTimeoutPeriod(getCurrentTransmission()));
            /*
                        FSMA_Event_Transition(Transmit-Broadcast,
                                              msg == endTimeout,
                                              IDLE,
                            currentAC=oldcurrentAC;
                            finishCurrentTransmission();
                            numSentBroadcast++;
                        );
            */
            ///changed
            FSMA_Event_Transition(Transmit-Broadcast,
                                  msg == endTimeout,
                                  DEFER,
                                  currentAC=oldcurrentAC;
                                  fr=getCurrentTransmission();
                                  numBites += fr->getBitLength();
                                  bites[currentAC] += fr->getBitLength();
                                  finishCurrentTransmission();
                                  numSentBroadcast++;
                                  resetCurrentBackOff();
                                 );
        }
        // accoriding to 9.2.5.7 CTS procedure
        FSMA_State(WAITCTS)
        {
            FSMA_Enter(scheduleCTSTimeoutPeriod());
            FSMA_Event_Transition(Receive-CTS,
                                  isLowerMsg(msg) && isForUs(frame) && frameType == ST_CTS,
                                  WAITSIFS,
                                  cancelTimeoutPeriod();
                                 );
            FSMA_Event_Transition(Transmit-RTS-Failed,
                                  msg == endTimeout && retryCounter[oldcurrentAC] == transmissionLimit - 1,
                                  IDLE,
                                  currentAC=oldcurrentAC;
                                  giveUpCurrentTransmission();
                                 );
            FSMA_Event_Transition(Receive-CTS-Timeout,
                                  msg == endTimeout,
                                  DEFER,
                                  currentAC=oldcurrentAC;
                                  retryCurrentTransmission();
                                 );
        }
        FSMA_State(WAITSIFS)
        {
            FSMA_Enter(scheduleSIFSPeriod(frame));
            FSMA_Event_Transition(Transmit-Data-TXOP,
                                  msg == endSIFS && getFrameReceivedBeforeSIFS()->getType() == ST_ACK,
                                  WAITACK,
                                  sendDataFrame(getCurrentTransmission());
                                 );
            FSMA_Event_Transition(Transmit-Data-TXOP,
                                  msg == endSIFS && getFrameReceivedBeforeSIFS()->getType() == ST_ACK,
                                  WAITACK,
                                  sendDataFrame(getCurrentTransmission());
                                 );
            FSMA_Event_Transition(Transmit-CTS,
                                  msg == endSIFS && getFrameReceivedBeforeSIFS()->getType() == ST_RTS,
                                  IDLE,
                                  sendCTSFrameOnEndSIFS();
                                  if (fixFSM)
                                  finishReception();
                                  else
                                      resetStateVariables();

                                     );
            FSMA_Event_Transition(Transmit-DATA,
                                  msg == endSIFS && getFrameReceivedBeforeSIFS()->getType() == ST_CTS,
                                  WAITACK,
                                  sendDataFrameOnEndSIFS(getCurrentTransmission());
                                 );
            FSMA_Event_Transition(Transmit-ACK,
                                  msg == endSIFS && isDataOrMgmtFrame(getFrameReceivedBeforeSIFS()),
                                  IDLE,
                                  sendACKFrameOnEndSIFS();
                                  if (fixFSM)
                                  finishReception();
                                  else
                                      resetStateVariables();

                                     );
        }
        // this is not a real state
        FSMA_State(RECEIVE)
        {
            FSMA_No_Event_Transition(Immediate-Receive-Error,
                                     isLowerMsg(msg) && (msgKind == COLLISION || msgKind == BITERROR),
                                     IDLE,
                                     EV << "received frame contains bit errors or collision, next wait period is EIFS\n";
                                     numCollision++;
                                     if (fixFSM)
                                     finishReception();
                                     else
                                         resetStateVariables();
                                        );
            FSMA_No_Event_Transition(Immediate-Receive-Broadcast,
                                     isLowerMsg(msg) && isBroadcast(frame) && !isSentByUs(frame) && isDataOrMgmtFrame(frame),
                                     IDLE,
                                     sendUp(frame);
                                     numReceivedBroadcast++;
                                     if (fixFSM)
                                     finishReception();
                                     else
                                         resetStateVariables();
                                        );
            FSMA_No_Event_Transition(Immediate-Receive-Data,
                                     isLowerMsg(msg) && isForUs(frame) && isDataOrMgmtFrame(frame),
                                     WAITSIFS,
                                     sendUp(frame);
                                     numReceived++;
                                    );
            FSMA_No_Event_Transition(Immediate-Receive-RTS,
                                     isLowerMsg(msg) && isForUs(frame) && frameType == ST_RTS,
                                     WAITSIFS,
                                    );
            FSMA_No_Event_Transition(Immediate-Receive-Other-backtobackoff,
                                     isLowerMsg(msg) && (backoff[0] || backoff[1] || backoff[2] || backoff[3]),
                                     DEFER,
                                    );

            FSMA_No_Event_Transition(Immediate-Promiscuous-Data,
                                     isLowerMsg(msg) && !isForUs(frame) && isDataOrMgmtFrame(frame),
                                     IDLE,
                                     nb->fireChangeNotification(NF_LINK_PROMISCUOUS, frame);
                                     if (fixFSM)
                                     finishReception();
                                     else
                                         resetStateVariables();
                                         numReceivedOther++;
                                        );
            FSMA_No_Event_Transition(Immediate-Receive-Other,
                                     isLowerMsg(msg),
                                     IDLE,
                                     if (fixFSM)
                                     finishReception();
                                     else
                                         resetStateVariables();
                                         numReceivedOther++;
                                        );
        }
    }
    EV<<"leaving handleWithFSM\n\t";
    logState();
    stateVector.record(fsm.getState());
    if (simTime() - last > 0.1)
    {
        int i;
        for (i =0; i<4; i++)
        {
            throughput[i].record(bites[i]/(simTime()-last));
            bites[i]=0;
            if (maxjitter[i] >0 && minjitter[i] >0)
            {
                jitter[i].record(maxjitter[i]-minjitter[i]);
                maxjitter[i]=0;
                minjitter[i]=0;
            }
        }
        last=simTime();
    }
}

void Ieee80211egMac::finishReception()
{
    if (getCurrentTransmission())
    {
        backoff[currentAC] = true;
        getCurrentTransmission()->setRetry(false);
    }
    else
    {
        backoff[currentAC] = false;
    }
}


/****************************************************************
 * Timing functions.
 */
simtime_t Ieee80211egMac::getSIFS()
{
// TODO:   return aRxRFDelay() + aRxPLCPDelay() + aMACProcessingDelay() + aRxTxTurnaroundTime();
    return SIFS;
}

simtime_t Ieee80211egMac::getSlotTime()
{
// TODO:   return aCCATime() + aRxTxTurnaroundTime + aAirPropagationTime() + aMACProcessingDelay();
    return ST;
}

simtime_t Ieee80211egMac::getPIFS()
{
    return getSIFS() + getSlotTime();
}

simtime_t Ieee80211egMac::getDIFS(int category)
{
    if (category<0 || category>3)
    {
        return getSIFS() + ((double)AIFSN[3] * getSlotTime());
    }
    else
    {
        return getSIFS() + ((double)AIFSN[category]) * getSlotTime();
    }

}

simtime_t Ieee80211egMac::getAIFS(int AccessCategory)
{
    return AIFSN[AccessCategory] * getSlotTime() + getSIFS();
}

simtime_t Ieee80211egMac::getEIFS()
{
// FIXME:   return getSIFS() + getDIFS() + (8 * ACKSize + aPreambleLength + aPLCPHeaderLength) / lowestDatarate;

    if (opMode=='b')
        return getSIFS() + getDIFS() + (8 * LENGTH_ACK + PHY_HEADER_LENGTH) / 1E+6;
    else
        return getSIFS() + getDIFS() + (8 * LENGTH_ACK) / 1E+6 + PHY_HEADER_LENGTH;
}

simtime_t Ieee80211egMac::computeBackoffPeriod(Ieee80211Frame *msg, int r)
{
    int cw;

    EV << "generating backoff slot number for retry: " << r << endl;
    if (msg && isBroadcast(msg))
        cw = cwMinBroadcast;
    else
    {
        ASSERT(0 <= r && r < transmissionLimit);

        cw = (cwMin[currentAC] + 1) * (1 << r) - 1;

        if (cw > cwMax[currentAC])
            cw = cwMax[currentAC];
    }

    int c = intrand(cw + 1);

    EV << "generated backoff slot number: " << c << " , cw: " << cw << " ,cwMin:cwMax = " << cwMin[currentAC] << ":" << cwMax[currentAC] << endl;

    return ((double)c) * getSlotTime();
}

/****************************************************************
 * Timer functions.
 */
void Ieee80211egMac::scheduleSIFSPeriod(Ieee80211Frame *frame)
{
    EV << "scheduling SIFS period\n";
    endSIFS->setContextPointer(frame->dup());
    scheduleAt(simTime() + getSIFS(), endSIFS);
}

void Ieee80211egMac::scheduleDIFSPeriod()
{
    if (lastReceiveFailed)
    {
        EV << "receiption of last frame failed, scheduling EIFS period\n";
        scheduleAt(simTime() + getEIFS(), endDIFS);
    }
    else
    {
        EV << "scheduling DIFS period\n";
        scheduleAt(simTime() + getDIFS(), endDIFS);
    }
}

void Ieee80211egMac::cancelDIFSPeriod()
{
    EV << "cancelling DIFS period\n";
    cancelEvent(endDIFS);
}

void Ieee80211egMac::scheduleAIFSPeriod()
{
    int i;
    bool schedule=false;
    for (i =0; i<4; i++)
    {
        if (!endAIFS[i]->isScheduled() && !transmissionQueue[i].empty())
        {
            if (lastReceiveFailed)
            {
                EV << "receiption of last frame failed, scheduling EIFS-DIFS+AIFS period (" << i << ")\n";
                scheduleAt(simTime() + getEIFS() - getDIFS() + getAIFS(i), endAIFS[i]);
            }
            else
            {
                EV << "scheduling AIFS period (" << i << ")\n";
                scheduleAt(simTime() + getAIFS(i), endAIFS[i]);
            }

        }
        if (endAIFS[i]->isScheduled())
            schedule=true;
    }
    if (!schedule && !endDIFS->isScheduled())
    {
        // schedule default DIFS
    	currentAC=3;
        scheduleDIFSPeriod();
    }
}

void Ieee80211egMac::rescheduleAIFSPeriod(int AccessCategory)
{
    ASSERT(1);
    EV << "rescheduling AIFS[" << AccessCategory << "]\n";
    cancelEvent(endAIFS[AccessCategory]);
    scheduleAt(simTime() + getAIFS(AccessCategory), endAIFS[AccessCategory]);
}

void Ieee80211egMac::cancelAIFSPeriod()
{
    EV << "cancelling AIFS period\n";
    cancelEvent(endAIFS[0]);
    cancelEvent(endAIFS[1]);
    cancelEvent(endAIFS[2]);
    cancelEvent(endAIFS[3]);
    cancelEvent(endDIFS);
}

//XXXvoid Ieee80211egMac::checkInternalColision()
//{
//  EV << "We obtain endAIFS, so we have to check if there
//}


void Ieee80211egMac::scheduleDataTimeoutPeriod(Ieee80211DataOrMgmtFrame *frameToSend)
{
    if (!endTimeout->isScheduled())
    {
        EV << "scheduling data timeout period\n";
        double tim = computeFrameDuration(frameToSend) +SIMTIME_DBL( getSIFS()) + computeFrameDuration(LENGTH_ACK, basicBitrate) + MAX_PROPAGATION_DELAY * 2;
        EV<<" time out="<<tim*1e6<<"us"<<endl;
        scheduleAt(simTime() + tim, endTimeout);
    }
}

void Ieee80211egMac::scheduleBroadcastTimeoutPeriod(Ieee80211DataOrMgmtFrame *frameToSend)
{
    if (!endTimeout->isScheduled())
    {
        EV << "scheduling broadcast timeout period\n";
        scheduleAt(simTime() + computeFrameDuration(frameToSend), endTimeout);
    }
}

void Ieee80211egMac::cancelTimeoutPeriod()
{
    EV << "cancelling timeout period\n";
    cancelEvent(endTimeout);
}

void Ieee80211egMac::scheduleCTSTimeoutPeriod()
{
    if (!endTimeout->isScheduled())
    {
        EV << "scheduling CTS timeout period\n";
        scheduleAt(simTime() + computeFrameDuration(LENGTH_RTS, basicBitrate) + getSIFS()
                   + computeFrameDuration(LENGTH_CTS, basicBitrate) + MAX_PROPAGATION_DELAY * 2, endTimeout);
    }
}

void Ieee80211egMac::scheduleReservePeriod(Ieee80211Frame *frame)
{
    simtime_t reserve = frame->getDuration();

    // see spec. 7.1.3.2
    if (!isForUs(frame) && reserve != 0 && reserve < 32768)
    {
        if (endReserve->isScheduled())
        {
            simtime_t oldReserve = endReserve->getArrivalTime() - simTime();

            if (oldReserve > reserve)
                return;

            reserve = std::max(reserve, oldReserve);
            cancelEvent(endReserve);
        }
        else if (radioState == RadioState::IDLE)
        {
            // NAV: the channel just became virtually busy according to the spec
            scheduleAt(simTime(), mediumStateChange);
        }

        EV << "scheduling reserve period for: " << reserve << endl;

        ASSERT(reserve > 0);

        nav = true;
        scheduleAt(simTime() + reserve, endReserve);
    }
}

void Ieee80211egMac::invalidateBackoffPeriod()
{
    backoffPeriod[currentAC] = -1;
}

bool Ieee80211egMac::isInvalidBackoffPeriod()
{
    return backoffPeriod[currentAC] == -1;
}

void Ieee80211egMac::generateBackoffPeriod()
{
    backoffPeriod[currentAC] = computeBackoffPeriod(getCurrentTransmission(), retryCounter[currentAC]);
    ASSERT(backoffPeriod[currentAC] >= 0);
    EV << "backoff period set to " << backoffPeriod[currentAC] << endl;
}

void Ieee80211egMac::decreaseBackoffPeriod()
{
    // see spec 9.9.1.5
    // decrase for every EDCAF
    // cancel event endBackoff after decrease or we don't know which endBackoff is scheduled
    int i;
    for (i =0; i<4; i++)
    {
        if (backoff[i] && endBackoff[i]->isScheduled())
        {
            EV<< "old backoff[" << i << "] is " << backoffPeriod[i] << ", sim time is " << simTime()
            << ", endbackoff sending period is " << endBackoff[i]->getSendingTime() << endl;
            simtime_t elapsedBackoffTime = simTime() - endBackoff[i]->getSendingTime();
            backoffPeriod[i] -= ((int)(elapsedBackoffTime / getSlotTime())) * getSlotTime();
            EV << "actual backoff[" << i << "] is " <<backoffPeriod[i] << ", elapsed is " << elapsedBackoffTime << endl;
            ASSERT(backoffPeriod[i] >= 0);
            EV << "backoff[" << i << "] period decreased to " << backoffPeriod[i] << endl;
        }
    }
}

void Ieee80211egMac::scheduleBackoffPeriod()
{
    EV << "scheduling backoff period\n";

    scheduleAt(simTime() + backoffPeriod[currentAC], endBackoff[currentAC]);
}

void Ieee80211egMac::cancelBackoffPeriod()
{
    EV << "cancelling Backoff period - only if some is scheduled\n";
    cancelEvent(endBackoff[0]);
    cancelEvent(endBackoff[1]);
    cancelEvent(endBackoff[2]);
    cancelEvent(endBackoff[3]);
}

/****************************************************************
 * Frame sender functions.
 */
void Ieee80211egMac::sendACKFrameOnEndSIFS()
{
    Ieee80211Frame *frameToACK = (Ieee80211Frame *)endSIFS->getContextPointer();
    endSIFS->setContextPointer(NULL);
    sendACKFrame(check_and_cast<Ieee80211DataOrMgmtFrame*>(frameToACK));
    delete frameToACK;
}

void Ieee80211egMac::sendACKFrame(Ieee80211DataOrMgmtFrame *frameToACK)
{
    EV << "sending ACK frame\n";
    numAckSend++;
    sendDown(setBasicBitrate(buildACKFrame(frameToACK)));
}

void Ieee80211egMac::sendDataFrameOnEndSIFS(Ieee80211DataOrMgmtFrame *frameToSend)
{
    Ieee80211Frame *ctsFrame = (Ieee80211Frame *)endSIFS->getContextPointer();
    endSIFS->setContextPointer(NULL);
    sendDataFrame(frameToSend);
    delete ctsFrame;
}

void Ieee80211egMac::sendDataFrame(Ieee80211DataOrMgmtFrame *frameToSend)
{
    simtime_t t=0,time=0;
    int count;
    std::list<Ieee80211DataOrMgmtFrame*>::iterator frame;

    frame = transmissionQueue[currentAC].begin();
    ASSERT(*frame==frameToSend);
    if (!txop && TXOP[currentAC] > 0 && transmissionQueue[currentAC].size() >= 2 )
    {
        //we start packet burst within TXOP time period
        txop = true;

        for (frame=transmissionQueue[currentAC].begin(); frame != transmissionQueue[currentAC].end(); ++frame)
        {
            count++;
            t = computeFrameDuration(*frame) + 2 * getSIFS() + computeFrameDuration(LENGTH_ACK, basicBitrate);
            EV << "t is " << t << endl;
            if (TXOP[currentAC]>time+t)
            {
                time += t;
                EV << "adding t \n";
            }
            else
            {
                break;
            }
        }
        //to be sure we get endTXOP earlier then receive ACK and we have to minus SIFS time from first packet
        time -= getSIFS()/2 + getSIFS();
        EV << "scheduling TXOP for AC" << currentAC << ", duration is " << time << ",count is " << count << endl;
        scheduleAt(simTime() + time, endTXOP);
    }
    EV << "sending Data frame\n";
    sendDown(buildDataFrame(frameToSend));
}

void Ieee80211egMac::sendRTSFrame(Ieee80211DataOrMgmtFrame *frameToSend)
{
    EV << "sending RTS frame\n";
    sendDown(setBasicBitrate(buildRTSFrame(frameToSend)));
}

void Ieee80211egMac::sendBroadcastFrame(Ieee80211DataOrMgmtFrame *frameToSend)
{
    EV << "sending Broadcast frame\n";
    sendDown(buildBroadcastFrame(frameToSend));
}

void Ieee80211egMac::sendCTSFrameOnEndSIFS()
{
    Ieee80211Frame *rtsFrame = (Ieee80211Frame *)endSIFS->getContextPointer();
    endSIFS->setContextPointer(NULL);
    sendCTSFrame(check_and_cast<Ieee80211RTSFrame*>(rtsFrame));
    delete rtsFrame;
}

void Ieee80211egMac::sendCTSFrame(Ieee80211RTSFrame *rtsFrame)
{
    EV << "sending CTS frame\n";
    sendDown(setBasicBitrate(buildCTSFrame(rtsFrame)));
}

/****************************************************************
 * Frame builder functions.
 */
Ieee80211DataOrMgmtFrame *Ieee80211egMac::buildDataFrame(Ieee80211DataOrMgmtFrame *frameToSend)
{
    Ieee80211DataOrMgmtFrame *frame = (Ieee80211DataOrMgmtFrame *)frameToSend->dup();

    if (isBroadcast(frameToSend))
        frame->setDuration(0);
    else if (!frameToSend->getMoreFragments())
    {
        if (txop)
        {
            std::list<Ieee80211DataOrMgmtFrame*>::iterator nextframeToSend;
            // ++ operation is safe because txop is true
            nextframeToSend=transmissionQueue[currentAC].begin();
            ASSERT(transmissionQueue[currentAC].end() != nextframeToSend);
            nextframeToSend++;
            frame->setDuration(3 * getSIFS() + 2 * computeFrameDuration(LENGTH_ACK, basicBitrate)
                               + computeFrameDuration(*nextframeToSend));
        }
        else
            frame->setDuration(getSIFS() + computeFrameDuration(LENGTH_ACK, basicBitrate));
    }
    else
        // FIXME: shouldn't we use the next frame to be sent?
        frame->setDuration(3 * getSIFS() + 2 * computeFrameDuration(LENGTH_ACK, basicBitrate) + computeFrameDuration(frameToSend));

    return frame;
}

Ieee80211ACKFrame *Ieee80211egMac::buildACKFrame(Ieee80211DataOrMgmtFrame *frameToACK)
{
    Ieee80211ACKFrame *frame = new Ieee80211ACKFrame("wlan-ack");
    frame->setReceiverAddress(frameToACK->getTransmitterAddress());

    if (!frameToACK->getMoreFragments())
        frame->setDuration(0);
    else
        frame->setDuration(frameToACK->getDuration() - getSIFS() - computeFrameDuration(LENGTH_ACK, basicBitrate));

    return frame;
}

Ieee80211RTSFrame *Ieee80211egMac::buildRTSFrame(Ieee80211DataOrMgmtFrame *frameToSend)
{
    Ieee80211RTSFrame *frame = new Ieee80211RTSFrame("wlan-rts");
    frame->setTransmitterAddress(address);
    frame->setReceiverAddress(frameToSend->getReceiverAddress());
    frame->setDuration(3 * getSIFS() + computeFrameDuration(LENGTH_CTS, basicBitrate) +
                       computeFrameDuration(frameToSend) +
                       computeFrameDuration(LENGTH_ACK, basicBitrate));

    return frame;
}

Ieee80211CTSFrame *Ieee80211egMac::buildCTSFrame(Ieee80211RTSFrame *rtsFrame)
{
    Ieee80211CTSFrame *frame = new Ieee80211CTSFrame("wlan-cts");
    frame->setReceiverAddress(rtsFrame->getTransmitterAddress());
    frame->setDuration(rtsFrame->getDuration() - getSIFS() - computeFrameDuration(LENGTH_CTS, basicBitrate));

    return frame;
}

Ieee80211DataOrMgmtFrame *Ieee80211egMac::buildBroadcastFrame(Ieee80211DataOrMgmtFrame *frameToSend)
{
    Ieee80211DataOrMgmtFrame *frame = (Ieee80211DataOrMgmtFrame *)frameToSend->dup();

    PhyControlInfo *phyControlInfo_old = dynamic_cast<PhyControlInfo *>( frameToSend->getControlInfo() );
    if (phyControlInfo_old)
    {
        ev<<"Per frame1 params"<<endl;
        PhyControlInfo *phyControlInfo_new=new PhyControlInfo;
        *phyControlInfo_new=*phyControlInfo_old;
        //EV<<"PhyControlInfo bitrate "<<phyControlInfo->getBitrate()/1e6<<"Mbps txpower "<<phyControlInfo->txpower()<<"mW"<<endl;
        frame->setControlInfo(  phyControlInfo_new  );
    }

    frame->setDuration(0);
    return frame;
}

Ieee80211Frame *Ieee80211egMac::setBasicBitrate(Ieee80211Frame *frame)
{
    ASSERT(frame->getControlInfo()==NULL);
    PhyControlInfo *ctrl = new PhyControlInfo();
    ctrl->setBitrate(basicBitrate);
    frame->setControlInfo(ctrl);
    return frame;
}

/****************************************************************
 * Helper functions.
 */
void Ieee80211egMac::finishCurrentTransmission()
{
    popTransmissionQueue();
    resetStateVariables();
}

void Ieee80211egMac::giveUpCurrentTransmission()
{
    Ieee80211DataOrMgmtFrame *temp = (Ieee80211DataOrMgmtFrame*) transmissionQueue[currentAC].front();
    nb->fireChangeNotification(NF_LINK_BREAK, temp);
    popTransmissionQueue();
    resetStateVariables();
    numGivenUp[currentAC]++;
}

void Ieee80211egMac::retryCurrentTransmission()
{
    ASSERT(retryCounter[currentAC] < transmissionLimit - 1);
    getCurrentTransmission()->setRetry(true);
    retryCounter[currentAC]++;
    numRetry[currentAC]++;
    backoff[currentAC] = true;
    generateBackoffPeriod();
}

Ieee80211DataOrMgmtFrame *Ieee80211egMac::getCurrentTransmission()
{
    return transmissionQueue[currentAC].empty() ? NULL : (Ieee80211DataOrMgmtFrame *)transmissionQueue[currentAC].front();
}

void Ieee80211egMac::sendDownPendingRadioConfigMsg()
{
    if (pendingRadioConfigMsg != NULL)
    {
        sendDown(pendingRadioConfigMsg);
        pendingRadioConfigMsg = NULL;
    }
}

void Ieee80211egMac::setMode(Mode mode)
{
    if (mode == PCF)
        error("PCF mode not yet supported");

    this->mode = mode;
}

void Ieee80211egMac::resetStateVariables()
{
    backoffPeriod[currentAC] = 0;
    retryCounter[currentAC] = 0;

    if (!transmissionQueue[currentAC].empty())
    {
        backoff[currentAC] = true;
        getCurrentTransmission()->setRetry(false);
    }
    else
    {
        backoff[currentAC] = false;
    }
}

bool Ieee80211egMac::isMediumStateChange(cMessage *msg)
{
    return msg == mediumStateChange || (msg == endReserve && radioState == RadioState::IDLE);
}

bool Ieee80211egMac::isMediumFree()
{
    return radioState == RadioState::IDLE && !endReserve->isScheduled();
}

bool Ieee80211egMac::isBroadcast(Ieee80211Frame *frame)
{
    return ( frame && frame->getReceiverAddress().isBroadcast() ) ||
           ( frame && frame->getReceiverAddress().isMulticast() );
}

bool Ieee80211egMac::isForUs(Ieee80211Frame *frame)
{
    return frame && frame->getReceiverAddress() == address;
}

bool Ieee80211egMac::isSentByUs(Ieee80211Frame *frame)
{

    if (dynamic_cast<Ieee80211DataOrMgmtFrame *>(frame))
    {
        //EV<<"ad3 "<<((Ieee80211DataOrMgmtFrame *)frame)->getAddress3();
        //EV<<"myad "<<address<<endl;
        if ( ((Ieee80211DataOrMgmtFrame *)frame)->getAddress3() == address)//received frame sent by us
            return 1;
    }
    else
        EV<<"Cast failed"<<endl;

    return 0;

}

bool Ieee80211egMac::isDataOrMgmtFrame(Ieee80211Frame *frame)
{
    return dynamic_cast<Ieee80211DataOrMgmtFrame*>(frame);
}

bool Ieee80211egMac::isMsgAIFS(cMessage *msg)
{
    return msg == endAIFS[0] || msg == endAIFS[1] || msg == endAIFS[2] || msg == endAIFS[3];
}

Ieee80211Frame *Ieee80211egMac::getFrameReceivedBeforeSIFS()
{
    return (Ieee80211Frame *)endSIFS->getContextPointer();
}

void Ieee80211egMac::popTransmissionQueue()
{
    EV << "dropping frame from transmission queue\n";

    Ieee80211Frame *temp = transmissionQueue[currentAC].front();
    ASSERT(!transmissionQueue[currentAC].empty());
    transmissionQueue[currentAC].pop_front();
    delete temp;
}

double Ieee80211egMac::computeFrameDuration(Ieee80211Frame *msg)
{

    PhyControlInfo *ctrl;
    double duration;
    ev<<*msg;
    ctrl = dynamic_cast<PhyControlInfo*> ( msg->removeControlInfo() );
    if ( ctrl )
    {
        ev<<"Per frame2 params bitrate "<<ctrl->getBitrate()/1e6<<endl;
        duration = computeFrameDuration(msg->getBitLength(), ctrl->getBitrate());
        delete ctrl;
        return duration;
    }
    else

        return computeFrameDuration(msg->getBitLength(), bitrate);
}

double Ieee80211egMac::computeFrameDuration(int bits, double bitrate)
{


    double duration;
    if (opMode=='g')
        duration=4*ceil((16+bits+6)/(bitrate/1e6*4))*1e-6 + PHY_HEADER_LENGTH;
    else
        duration=bits / bitrate + PHY_HEADER_LENGTH / BITRATE_HEADER;
    EV<<" duration="<<duration*1e6<<"us("<<bits<<"bits "<<bitrate/1e6<<"Mbps)"<<endl;
    return duration;
}

void Ieee80211egMac::logState()
{
    char a[10]="";
    char b[10]="";
    char c[10]="";
    char d[10]="";
    char ab[10]="";
    char bb[10]="";
    char cb[10]="";
    char db[10]="";
    char medium[5]="busy";

    if (isMediumFree()) strcpy(medium,"free");
    if (endBackoff[0]->isScheduled()) strcpy(ab,"scheduled");
    if (endBackoff[1]->isScheduled()) strcpy(bb,"scheduled");
    if (endBackoff[2]->isScheduled()) strcpy(cb,"scheduled");
    if (endBackoff[3]->isScheduled()) strcpy(db,"scheduled");
    if (endAIFS[0]->isScheduled()) strcpy(a,"scheduled");
    if (endAIFS[1]->isScheduled()) strcpy(b,"scheduled");
    if (endAIFS[2]->isScheduled()) strcpy(c,"scheduled");
    if (endAIFS[3]->isScheduled()) strcpy(d,"scheduled");

    EV  << "# state information: mode = " << modeName(mode) << ", state = " << fsm.getStateName()
    << ", backoff 0..3 = " << backoff[0] << " " << backoff[1] << " " << backoff[2] << " " << backoff[3]
    << ", \n# backoffPeriod 0..3 = " << backoffPeriod[0] << " " << backoffPeriod[1]
    << " " << backoffPeriod[2] << " " << backoffPeriod[3]
    << ", \n#    retryCounter 0..3 = " << retryCounter[0] << " " << retryCounter[1]
    << " " << retryCounter[2] << " " << retryCounter[3] << ", radioState = " << radioState
    << ", nav = " << nav <<  ",txop is "<< txop << endl
    << "queue size 0..3 = " << transmissionQueue[0].size() << " " << transmissionQueue[1].size() << " "
    << transmissionQueue[2].size() << " " << transmissionQueue[3].size()
    << " medium is " << medium << ", scheduled AIFS are 0(" << a << ") 1("
    << b << ") 2(" << c << ") 3(" << d << ")"
    << ", scheduled backoff are 0(" << ab << ") 1("
    << bb << ") 2(" << cb << ") 3(" << db << ")" << endl
    << "# currentAC: " << currentAC << ", oldcurrentAC: " << oldcurrentAC << endl;
}

const char *Ieee80211egMac::modeName(int mode)
{
#define CASE(x) case x: s=#x; break
    const char *s = "???";
    switch (mode)
    {
        CASE(DCF);
        CASE(PCF);
    }
    return s;
#undef CASE
}

bool Ieee80211egMac::transmissionQueueEmpty()
{
    return (transmissionQueue[0].empty() && transmissionQueue[1].empty() &&
            transmissionQueue[2].empty() && transmissionQueue[3].empty());
}

