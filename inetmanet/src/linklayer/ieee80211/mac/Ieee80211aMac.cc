//
// Copyright (C) 2006 Andras Varga, Levente Meszaros and Ahmed Ayadi
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#include <algorithm>
#include "Ieee80211aMac.h"
#include "RadioState.h"
#include "IInterfaceTable.h"
#include "InterfaceTableAccess.h"
#include "PhyControlInfo_m.h"
#include "AirFrame_m.h"
#include "Radio80211aControlInfo_m.h"

//#define LWMPLS

Define_Module(Ieee80211aMac);

// don't forget to keep synchronized the C++ enum and the runtime enum definition
Register_Enum(Ieee80211aMac,
              (Ieee80211aMac::IDLE,
               Ieee80211aMac::DEFER,
               Ieee80211aMac::WAITDIFS,
               Ieee80211aMac::BACKOFF,
               Ieee80211aMac::WAITACK,
               Ieee80211aMac::WAITBROADCAST,
               Ieee80211aMac::WAITCTS,
               Ieee80211aMac::WAITSIFS,
               Ieee80211aMac::RECEIVE));

// don't forget to keep synchronized the C++ enum and the runtime enum definition
Register_Enum(RadioState,
              (RadioState::IDLE,
               RadioState::RECV,
               RadioState::TRANSMIT,
               RadioState::SLEEP));


/****************************************************************
 * Construction functions.
 */
Ieee80211aMac::Ieee80211aMac()
{
    endSIFS = NULL;
    endDIFS = NULL;
    endBackoff = NULL;
    endTimeout = NULL;
    endReserve = NULL;
    mediumStateChange = NULL;
    pendingRadioConfigMsg = NULL;
}

Ieee80211aMac::~Ieee80211aMac()
{
    cancelAndDelete(endSIFS);
    cancelAndDelete(endDIFS);
    cancelAndDelete(endBackoff);
    cancelAndDelete(endTimeout);
    cancelAndDelete(endReserve);
    cancelAndDelete(mediumStateChange);

    if (pendingRadioConfigMsg)
        delete pendingRadioConfigMsg;

    while (!transmissionQueue.empty())
    {
        Ieee80211Frame *temp = transmissionQueue.front();
        transmissionQueue.pop_front();
        delete temp;
    }
}


/****************************************************************
 * Initialization functions.
 */
void Ieee80211aMac::initialize(int stage)
{
    WirelessMacBase::initialize(stage);

    if (stage == 0)
    {
        EV << "Initializing stage 0\n";

        // initialize parameters
        // Variable to apply the fsm fix
        fixFSM = par("fixFSM");

        opMode = hasPar("opMode") ? par("opMode") : 'b';
        if (opMode==1)
            opMode='b';
        else if (opMode==2)
            opMode='g';
        else if (opMode==3)
            opMode='a';
        else
            opMode='b';

        EV<<"Operating mode: 802.11 "<<opMode;
        maxQueueSize = par("maxQueueSize");
        bitrate = par("bitrate");
        bool found = false;

        if (opMode == 'b')
        {
            rateIndex = 0;
            for (int i = 0; i < 4; i++)
            {
                if (bitrate == BITRATES_80211b[i])
                {
                    found = true;
                    rateIndex = i;
                    break;
                }
            }
            if (!found)
            {
                bitrate = BITRATES_80211b[3];
                rateIndex = 3;
            }
        }
        else
        {
            rateIndex = 0;
            for (int i = 0; i < 8; i++)
            {
                if (bitrate == BITRATES_80211a[i])
                {
                    found = true;
                    rateIndex = i;
                    break;
                }
            }
            if (!found)
            {
                bitrate = BITRATES_80211a[7];
                rateIndex = 7;
            }
        }
        EV<<" bitrate="<<bitrate/1e6<<"M IDLE="<<IDLE<<" RECEIVE="<<RECEIVE<<endl;

        PHY_HEADER_LENGTH_G=par("PHY_HEADER_LENGTH");
        if (PHY_HEADER_LENGTH_G<0)
            PHY_HEADER_LENGTH_G=26e-6;//26us

        //basicBitrate = 2e6; //FIXME make it parameter
        basicBitrate = par("basicBitrate");
        if (basicBitrate==-1)
        {
            if (opMode == 'b')
                basicBitrate = 1e6;//1Mbps
            //else basicBitrate = 6e6;//6Mbps
            else basicBitrate = 2e6;//6Mbps
        }
        EV<<" basicBitrate="<<basicBitrate/1e6<<"M"<<endl;

        rtsThreshold = par("rtsThresholdBytes");

        retryLimit = par("retryLimit");
        if (retryLimit == -1) retryLimit = 7;
        ASSERT(retryLimit >= 0);

        cwMinData = par("cwMinData");
        if (cwMinData == -1) cwMinData = CW_MIN;
        ASSERT(cwMinData >= 0);

        cwMinBroadcast = par("cwMinBroadcast");
        if (cwMinBroadcast == -1) cwMinBroadcast = 31;
        ASSERT(cwMinBroadcast >= 0);

        AIFSN=par("AIFSN");
        if (AIFSN==-1)
            AIFSN=2;

        EV<<" AIFSN="<<AIFSN;

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

        std::cout<<getParentModule()->getFullPath()<<"."<<getClassName()<<":"<<"MAC ADDRESS = "<<address<<endl;

        minSuccessThreshold = hasPar("minSuccessThreshold") ? par("minSuccessThreshold") : 10;
        minTimerTimeout = hasPar("minTimerTimeout") ? par("minTimerTimeout") : 15;
        timerTimeout = hasPar("timerTimeout") ? par("timerTimeout") : minTimerTimeout;
        successThreshold = hasPar("successThreshold") ? par("successThreshold") : minSuccessThreshold;
        autoBitrate = hasPar("autoBitrate") ? par("autoBitrate") : 0;

        switch (autoBitrate)
        {
        case 0:
        {
            rateControlMode = RATE_CR;
            EV<<"MAC Transmossion algorithm : Constant Rate"  <<endl;
            break;
        }

        case 1:
        {
            rateControlMode = RATE_ARF;
            EV<<"MAC Transmossion algorithm : ARF Rate"  <<endl;
            break;
        }

        case 2:
        {
            rateControlMode = RATE_AARF;
            successCoeff = hasPar("successCoeff") ? par("successCoeff") : 2.0;
            timerCoeff = hasPar("timerCoeff") ? par("timerCoeff") : 2.0;
            maxSuccessThreshold = hasPar("maxSuccessThreshold") ? par("maxSuccessThreshold") : 60;
            EV<<"MAC Transmossion algorithm : AARF Rate"  <<endl;
        }
        break;

        default:
            rateControlMode = RATE_CR;
        }
        // subscribe for the information of the carrier sense
        nb->subscribe(this, NF_RADIOSTATE_CHANGED);

        // initalize self messages
        endSIFS = new cMessage("SIFS");
        endDIFS = new cMessage("DIFS");
        endBackoff = new cMessage("Backoff");
        endTimeout = new cMessage("Timeout");
        endReserve = new cMessage("Reserve");
        mediumStateChange = new cMessage("MediumStateChange");
        fsm.setState(WAITBROADCAST,"WAITBROADCAST");
        scheduleAt(0, endTimeout);

        // interface
        registerInterface();

        // obtain pointer to external queue
        initializeQueueModule();

        // state variables
        fsm.setName("Ieee80211aMac State Machine");
        mode = DCF;
        sequenceNumber = 0;
        radioState = RadioState::IDLE;
        retryCounter = 0;
        backoffPeriod=-1;
        backoff = false;
        lastReceiveFailed = false;
        noFrame=true;
        nav = false;
        i=0;
        j=0;
        recvdThroughput=0;
        _snr=0;
        samplingCoeff = 50;

        // statistics
        numRetry = 0;
        numSentWithoutRetry = 0;
        numGivenUp = 0;
        numCollision = 0;
        numSent = 0;
        numReceived = 0;
        numSentBroadcast = 0;
        numReceivedBroadcast = 0;
        numReceivedOther = 0;
        numAckSend = 0;
        successCounter = 0;
        failedCounter = 0;
        recovery = 0;
        timer = 0;
        timeStampLastMessageReceived = 0;

        stateVector.setName("State");
        stateVector.setEnum("Ieee80211aMac");
        radioStateVector.setName("RadioState");
        radioStateVector.setEnum("RadioState");
        receiveBroadcastVector.setName("ReceiveBcastVector");
        cwVector.setName("Contention Window");
        cwStats.setName("Contetion Window");
        receivedThroughput.setName("Received Throughput");
        sendThroughput.setName("Send Throughput");
        PHYRateVector.setName("PHY bit rate");


        // initialize watches
        WATCH(fsm);
        WATCH(radioState);
        WATCH(retryCounter);
        WATCH(backoff);
        WATCH(nav);
        WATCH(numRetry);
        WATCH(numSentWithoutRetry);
        WATCH(numGivenUp);
        WATCH(numCollision);
        WATCH(numSent);
        WATCH(numReceived);
        WATCH(numSentBroadcast);
        WATCH(numReceivedBroadcast);
        radioModule = gate("lowergateOut")->getNextGate()->getOwnerModule()->getId();
    }
}

void Ieee80211aMac::registerInterface()
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

void Ieee80211aMac::initializeQueueModule()
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
void Ieee80211aMac::handleSelfMsg(cMessage *msg)
{
    EV << "received self message: " << msg << endl;

    if (msg == endReserve)
        nav = false;

    handleWithFSM(msg);
}

void Ieee80211aMac::handleUpperMsg(cPacket *msg)
{
    EV<<"Ieee80211aMac handle message from upper layer"<<endl;
    // check for queue overflow
    if (maxQueueSize && (int) transmissionQueue.size() == maxQueueSize)
    {
        EV << "message " << msg << " received from higher layer but MAC queue is full, dropping message\n";
        delete msg;
        return;
    }

    // must be a Ieee80211DataOrMgmtFrame, within the max size because we don't support fragmentation
    Ieee80211DataOrMgmtFrame *frame = check_and_cast<Ieee80211DataOrMgmtFrame *>(msg);
    if (frame->getByteLength() > fragmentationThreshold)
        error("message from higher layer (%s)%s is too long for 802.11b, %d bytes (fragmentation is not supported yet)",
              msg->getClassName(), msg->getName(), (int)(msg->getByteLength()));
    EV << "frame " << frame << " received from higher layer, receiver = " << frame->getReceiverAddress() << endl;

    ASSERT(!frame->getReceiverAddress().isUnspecified());

    // fill in missing fields (receiver address, seq number), and insert into the queue
    frame->setTransmitterAddress(address);
    frame->setSequenceNumber(sequenceNumber);
    sequenceNumber = (sequenceNumber+1) % 4096;  //XXX seqNum must be checked upon reception of frames!

    transmissionQueue.push_back(frame);

    handleWithFSM(frame);
}

void Ieee80211aMac::handleCommand(cMessage *msg)
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
            PhyControlInfo *phyControlInfo = dynamic_cast<PhyControlInfo *>(msg->getControlInfo());
            if (phyControlInfo)
                phyControlInfo->setAdativeSensitivity(true);
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

void Ieee80211aMac::handleLowerMsg(cPacket *msg)
{
    EV << "received message from lower layer: " << msg << endl;

    Ieee80211Frame *frame;

    nb->fireChangeNotification(NF_LINK_FULL_PROMISCUOUS, msg);
    if (msg->getControlInfo() && dynamic_cast<Radio80211aControlInfo *>(msg->getControlInfo()))
    {
        Radio80211aControlInfo *cinfo = (Radio80211aControlInfo*) msg->removeControlInfo();
        if (j%10==0)
        {
            snr = _snr;
            j=0;
            _snr=0;
        }
        j++;
        _snr+=cinfo->getSnr()/10;
        lossRate = cinfo->getLossRate();
        delete cinfo;
    }


    if (i%samplingCoeff==0)
    {
        receivedThroughput.record(recvdThroughput);
        i=0;
        recvdThroughput=0;
    }
    i++;

    frame = dynamic_cast<Ieee80211Frame *>(msg);
    if (timeStampLastMessageReceived == 0)
        timeStampLastMessageReceived = simTime();
    else
    {
        if (frame)
            recvdThroughput+=((frame->getBitLength()/(simTime()-timeStampLastMessageReceived))/1000000)/samplingCoeff;
        timeStampLastMessageReceived = simTime();
    }


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
    << ", received frame is for us: " << isForUs(frame) << endl;

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
}

void Ieee80211aMac::receiveChangeNotification(int category, const cPolymorphic *details)
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
void Ieee80211aMac::handleWithFSM(cMessage *msg)
{
    // skip those cases where there's nothing to do, so the switch looks simpler
//   if (isUpperMsg(msg) && fsm.getState() != IDLE)
//    {
//        EV << "deferring upper message transmission in " << fsm.getStateName() << " state\n";
//        return;
//    }
    // skip those cases where there's nothing to do, so the switch looks simpler
    if (noFrame==true && isUpperMsg(msg))
    {
        noFrame=false;
        EV<<"New frame arrived while backing-off with noFrame\n";
    }
    else if (isUpperMsg(msg) && fsm.getState() != IDLE)
    {
        EV << "deferring upper message transmission in " << fsm.getStateName() << " state\n";
        return;
    }

    Ieee80211Frame *frame = dynamic_cast<Ieee80211Frame*>(msg);
    int frameType = frame ? frame->getType() : -1;
    int msgKind = msg->getKind();
    logState();
//     stateVector.record(fsm.getState());

    if (frame && isLowerMsg(frame))
    {
        lastReceiveFailed =(msgKind == COLLISION || msgKind == BITERROR);
        scheduleReservePeriod(frame);
    }

    // TODO: fixed bug according to the message: [omnetpp] A possible bug in the Ieee80211's FSM. It's necessary to check
    FSMA_Switch(fsm)
    {
        FSMA_State(IDLE)
        {
            FSMA_Enter(sendDownPendingRadioConfigMsg());
            if (fixFSM)
            {
                FSMA_Event_Transition(Data-Ready,
                                      // isUpperMsg(msg),
                                      isUpperMsg(msg) && backoffPeriod > 0,
                                      DEFER,
                                      //ASSERT(isInvalidBackoffPeriod() || backoffPeriod == 0);
                                      //invalidateBackoffPeriod();
                                      ASSERT(false);

                                     );
                FSMA_No_Event_Transition(Immediate-Data-Ready,
                                         //!transmissionQueue.empty(),
                                         !transmissionQueue.empty() && backoffPeriod > 0,
                                         DEFER,
//                invalidateBackoffPeriod();
                                         ASSERT(backoff);

                                        );
            }
            FSMA_Event_Transition(Data-Ready,
                                  isUpperMsg(msg),
                                  DEFER,
                                  ASSERT(isInvalidBackoffPeriod() || backoffPeriod == 0);
                                  invalidateBackoffPeriod();
                                 );
            FSMA_No_Event_Transition(Immediate-Data-Ready,
                                     !transmissionQueue.empty(),
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
            FSMA_Event_Transition(Wait-DIFS,
                                  (isMediumStateChange(msg) && isMediumFree()),
                                  WAITDIFS,
                                  ;);
            FSMA_No_Event_Transition(Immediate-Wait-DIFS,
                                     (isMediumFree() || (!backoff)),
                                     WAITDIFS,
                                     ;);
            FSMA_Event_Transition(Receive,
                                  isLowerMsg(msg),
                                  RECEIVE,
                                  ;);
        }

        FSMA_State(WAITDIFS)
        {
            FSMA_Enter(scheduleDIFSPeriod());
            if (!transmissionQueue.empty())
            {
                FSMA_Event_Transition(Immediate-Transmit-RTS,
                                      msg == endDIFS && !isBroadcast(getCurrentTransmission()) && !isMulticast(getCurrentTransmission())
                                      && getCurrentTransmission()->getByteLength() >= rtsThreshold && !backoff,
                                      WAITCTS,
                                      sendRTSFrame(getCurrentTransmission());
                                      cancelDIFSPeriod();
                                     );//ahmed
                FSMA_Event_Transition(Immediate-Transmit-Broadcast,
                                      msg == endDIFS && isBroadcast(getCurrentTransmission()) && !backoff,
                                      WAITBROADCAST,
                                      sendBroadcastFrame(getCurrentTransmission());
                                      cancelDIFSPeriod();
                                     );
                FSMA_Event_Transition(Immediate-Transmit-Multicast,
                                      msg == endDIFS && isMulticast(getCurrentTransmission()) && !backoff,
                                      WAITMULTICAST,
                                      sendMulticastFrame(getCurrentTransmission());
                                      cancelDIFSPeriod();
                                     );
                FSMA_Event_Transition(Immediate-Transmit-Data,
                                      msg == endDIFS && !isBroadcast(getCurrentTransmission()) && !isMulticast(getCurrentTransmission()) && !backoff,
                                      WAITACK,
                                      sendDataFrame(getCurrentTransmission());//ahmed
                                      cancelDIFSPeriod();
                                     );
            }
            FSMA_Event_Transition(DIFS-Over,
                                  msg == endDIFS,
                                  BACKOFF,
                                  ASSERT(backoff);
                                  if (isInvalidBackoffPeriod())
                                  generateBackoffPeriod();
                                 );
            FSMA_Event_Transition(Busy,
                                  isMediumStateChange(msg) && !isMediumFree(),
                                  DEFER,
                                  backoff = true;
                                  cancelDIFSPeriod();
                                 );
            FSMA_No_Event_Transition(Immediate-Busy,
                                     !isMediumFree(),
                                     DEFER,
                                     backoff = true;
                                     cancelDIFSPeriod();
                                    );
            // radio state changes before we actually get the message, so this must be here
            FSMA_Event_Transition(Receive,
                                  isLowerMsg(msg),
                                  RECEIVE,
                                  cancelDIFSPeriod();
                                  ;);
        }
        FSMA_State(BACKOFF)
        {
            FSMA_Enter(scheduleBackoffPeriod());
            if (!transmissionQueue.empty())
            {
                FSMA_Event_Transition(Transmit-RTS,
                                      msg == endBackoff && !isBroadcast(getCurrentTransmission()) && !isMulticast(getCurrentTransmission())
                                      && getCurrentTransmission()->getByteLength() >= rtsThreshold,
                                      WAITCTS,
                                      sendRTSFrame(getCurrentTransmission());
                                     );//ahmed
                FSMA_Event_Transition(Transmit-Broadcast,
                                      msg == endBackoff && isBroadcast(getCurrentTransmission()),
                                      WAITBROADCAST,
                                      sendBroadcastFrame(getCurrentTransmission());
                                     );
                FSMA_Event_Transition(Transmit-Multicast,
                                      msg == endBackoff && isMulticast(getCurrentTransmission()),
                                      WAITMULTICAST,
                                      sendMulticastFrame(getCurrentTransmission());
                                     );//ahmed
                FSMA_Event_Transition(Transmit-Data,
                                      msg == endBackoff && !isBroadcast(getCurrentTransmission()) && !isMulticast(getCurrentTransmission()),
                                      WAITACK,
                                      sendDataFrame(getCurrentTransmission());
                                     );//ahmed
            }

            FSMA_Event_Transition(Backoff-Idle,
                                  msg==endBackoff && transmissionQueue.empty(),
                                  IDLE,
                                  resetStateVariables();
                                 );

            FSMA_Event_Transition(Backoff-Busy,
                                  isMediumStateChange(msg) && !isMediumFree(),
                                  DEFER,
                                  cancelBackoffPeriod();
                                  decreaseBackoffPeriod();
                                 );
        }
        FSMA_State(WAITACK)
        {
            FSMA_Enter(scheduleDataTimeoutPeriod(getCurrentTransmission()));
            /*FSMA_Event_Transition(Receive-ACK,
                                  isLowerMsg(msg) && isForUs(frame) && frameType == ST_ACK,
                                  IDLE,
                                  if (retryCounter == 0) numSentWithoutRetry++;
                                  numSent++;
                                  cancelTimeoutPeriod();
                                  finishCurrentTransmission();
                                 );*/
            // 9.2.5.2 Backoff procedure for DCF, after a ACK a backoff must be programmed even if not more frames are present
            // The state jumps to DEFER from Defer to DIFS and like backoff is true jumps to BACKOFF
            FSMA_Event_Transition(Receive-ACK,
                                  isLowerMsg(msg) && isForUs(frame) && frameType == ST_ACK,
                                  DEFER,
                                  if (retryCounter == 0) numSentWithoutRetry++;
                                  numSent++;
                                  cancelTimeoutPeriod();
                                  finishCurrentTransmission();
                                  backoff=true;
                                  invalidateBackoffPeriod();
                                 );
            FSMA_Event_Transition(Transmit-Data-Failed,
                                  msg == endTimeout && retryCounter == retryLimit-1,
                                  IDLE,
                                  giveUpCurrentTransmission();
                                 );
            FSMA_Event_Transition(Receive-ACK-Timeout,
                                  msg == endTimeout,
                                  DEFER,
                                  retryCurrentTransmission();
                                 );
        }
        // wait until broadcast is sent
        FSMA_State(WAITBROADCAST)
        {
            // if(!transmissionQueue.empty())
            FSMA_Enter(scheduleBroadcastTimeoutPeriod(getCurrentTransmission()));
            /*
                        FSMA_Event_Transition(Transmit-Broadcast,
                                              msg == endTimeout,
                                              IDLE,
                            finishCurrentTransmission();
                            numSentBroadcast++;
                        );
            *///changed

            FSMA_Event_Transition(Transmit-Broadcast,
                                  msg == endTimeout,
                                  DEFER,
                                  finishCurrentTransmission();
                                  numSentBroadcast++;
                                  backoff=true;
                                  invalidateBackoffPeriod();
                                 );

        }

        FSMA_State(WAITMULTICAST)
        {
            FSMA_Enter(scheduleMulticastTimeoutPeriod(getCurrentTransmission()));
            FSMA_Event_Transition(Transmit-Multicast,
                                  msg == endTimeout,
                                  IDLE,
                                  finishCurrentTransmission();
                                 );
        }//ahmed
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
                                  msg == endTimeout && retryCounter == retryLimit - 1,
                                  IDLE,
                                  giveUpCurrentTransmission();
                                 );
            FSMA_Event_Transition(Receive-CTS-Timeout,
                                  msg == endTimeout,
                                  DEFER,
                                  retryCurrentTransmission();
                                 );
        }
        FSMA_State(WAITSIFS)
        {
            FSMA_Enter(scheduleSIFSPeriod(frame));
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
            /*
                        FSMA_No_Event_Transition(Immediate-Receive-Broadcast,
                                                 isLowerMsg(msg) && isBroadcast(frame) && isDataOrMgmtFrame(frame),
                                                 IDLE,
                            sendUp(frame);
                            numReceivedBroadcast++;
                            if (fixFSM)
                                finishReception();
                            else
                                resetStateVariables();
                        );
            *///added && !isSentByUs(frame)
            FSMA_No_Event_Transition(Immediate-Receive-Broadcast,
                                     isLowerMsg(msg) && isBroadcast(frame) && !isSentByUs(frame) && isDataOrMgmtFrame(frame),
                                     IDLE,
                                     sendUp(frame);
                                     numReceivedBroadcast++;
                                     /*
                                             tCycle+=simTime()-oldT;
                                             oldT=simTime();
                                             if(tCycle>=1000.0E-3){
                                                 receiveBroadcastVector.record(numReceivedBroadcast*(msg->getBitLength()-62*8)/tCycle/1e6);
                                                 thStats.collect(numReceivedBroadcast*(msg->getBitLength()-62*8)/tCycle/1e6);
                                                 tCycle=0.0;
                                                 numReceivedBroadcast=0;
                                                 }
                                     */
                                     if (fixFSM)
                                     finishReception();
                                     else
                                         resetStateVariables();
                                        );

            FSMA_No_Event_Transition(Immediate-Receive-Multicast,
                                     isLowerMsg(msg) && isMulticast(frame) && isDataOrMgmtFrame(frame),
                                     IDLE,
                                     sendUp(frame);
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

    logState();
//     stateVector.record(fsm.getState());
}

void Ieee80211aMac::finishReception()
{
    if (!transmissionQueue.empty())
    {
        backoff = true;
    }
    else
    {
        backoffPeriod = 0;
        retryCounter = 0;
        backoff = false;
        noFrame=false;//sorin
    }
}


/****************************************************************
 * Timing functions.
 */
simtime_t Ieee80211aMac::getSIFS()
{
// TODO:   return aRxRFDelay() + aRxPLCPDelay() + aMACProcessingDelay() + aRxTxTurnaroundTime();
    return SIFS;
}

simtime_t Ieee80211aMac::getSlotTime()
{
// TODO:   return aCCATime() + aRxTxTurnaroundTime + aAirPropagationTime() + aMACProcessingDelay();
    return ST;
}

simtime_t Ieee80211aMac::getPIFS()
{
    return getSIFS() + getSlotTime();
}

simtime_t Ieee80211aMac::getDIFS()
{
    return getSIFS() + ((double)AIFSN) * getSlotTime();
}

simtime_t Ieee80211aMac::getEIFS()
{
// FIXME:   return getSIFS() + getDIFS() + (8 * ACKSize + aPreambleLength + aPLCPHeaderLength) / lowestDatarate;
    return getSIFS() + getDIFS() + (8 * LENGTH_ACK + PHY_HEADER_LENGTH_A) / 1E+6;
}

simtime_t Ieee80211aMac::computeBackoffPeriod(Ieee80211Frame *msg, int r)
{
    int cw;

    if (msg && isBroadcast(msg))
        cw = cwMinBroadcast;
    else
    {
        ASSERT(0 <= r && r < retryLimit);
        cw = (cwMinData + 1) * (1 << r) - 1;
        if (cw > CW_MAX)
            cw = CW_MAX;
    }


    int c = intrand(cw + 1);

    EV << "generated backoff slot number: " << c << " , cw: " << cw << endl;
    cwVector.record(((double)c) );
    cwStats.collect(((double)c) );

    return ((double)c) * getSlotTime();

}

/****************************************************************
 * Timer functions.
 */
void Ieee80211aMac::scheduleSIFSPeriod(Ieee80211Frame *frame)
{
    EV << "scheduling SIFS period\n";
    endSIFS->setContextPointer(frame->dup());
    scheduleAt(simTime() + getSIFS(), endSIFS);
}

void Ieee80211aMac::scheduleDIFSPeriod()
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

void Ieee80211aMac::cancelDIFSPeriod()
{
    EV << "cancelling DIFS period\n";
    cancelEvent(endDIFS);
}

void Ieee80211aMac::scheduleDataTimeoutPeriod(Ieee80211DataOrMgmtFrame *frameToSend)
{
    EV << "scheduling data timeout period\n";
    simtime_t prop_delay = MAX_PROPAGATION_DELAY;
    double delay;
    delay = computeFrameDuration(frameToSend) + SIMTIME_DBL(getSIFS()) + computeFrameDuration(LENGTH_ACK, basicBitrate) + SIMTIME_DBL(prop_delay) * 2;
    scheduleAt(simTime() +delay , endTimeout);
}

void Ieee80211aMac::scheduleBroadcastTimeoutPeriod(Ieee80211DataOrMgmtFrame *frameToSend)
{
    EV << "scheduling broadcast timeout period\n";
    scheduleAt(simTime() + computeFrameDuration(frameToSend), endTimeout);
}

void Ieee80211aMac::scheduleMulticastTimeoutPeriod(Ieee80211DataOrMgmtFrame *frameToSend)
{
    EV << "scheduling multicast timeout period\n";
    scheduleAt(simTime() + computeFrameDuration(frameToSend), endTimeout);
}//ahmed

void Ieee80211aMac::cancelTimeoutPeriod()
{
    EV << "cancelling timeout period\n";
    cancelEvent(endTimeout);
}

void Ieee80211aMac::scheduleCTSTimeoutPeriod()
{
    scheduleAt(simTime() + computeFrameDuration(LENGTH_RTS, basicBitrate) + getSIFS() + computeFrameDuration(LENGTH_CTS, basicBitrate) + MAX_PROPAGATION_DELAY * 2, endTimeout);
}

void Ieee80211aMac::scheduleReservePeriod(Ieee80211Frame *frame)
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

void Ieee80211aMac::invalidateBackoffPeriod()
{
    backoffPeriod = -1;
}

bool Ieee80211aMac::isInvalidBackoffPeriod()
{
    return backoffPeriod == -1;
}

void Ieee80211aMac::generateBackoffPeriod()
{
    backoffPeriod = computeBackoffPeriod(getCurrentTransmission(), retryCounter);
    ASSERT(backoffPeriod >= 0);
    EV << "backoff period set to " << backoffPeriod << endl;
}

void Ieee80211aMac::decreaseBackoffPeriod()
{
    // see spec 9.2.5.2
    simtime_t elapsedBackoffTime = simTime() - endBackoff->getSendingTime();
    backoffPeriod -= ((int)(elapsedBackoffTime / getSlotTime())) * getSlotTime();
    ASSERT(backoffPeriod >= 0);
    EV << "backoff period decreased to " << backoffPeriod << endl;
}

void Ieee80211aMac::scheduleBackoffPeriod()
{
    EV << "scheduling backoff period\n";
    scheduleAt(simTime() + backoffPeriod, endBackoff);
}

void Ieee80211aMac::cancelBackoffPeriod()
{
    EV << "cancelling Backoff period\n";
    cancelEvent(endBackoff);
}

/****************************************************************
 * Frame sender functions.
 */
void Ieee80211aMac::sendACKFrameOnEndSIFS()
{
    Ieee80211Frame *frameToACK = (Ieee80211Frame *)endSIFS->getContextPointer();
    endSIFS->setContextPointer(NULL);
    sendACKFrame(check_and_cast<Ieee80211DataOrMgmtFrame*>(frameToACK));
    delete frameToACK;
}

void Ieee80211aMac::sendACKFrame(Ieee80211DataOrMgmtFrame *frameToACK)
{
    EV << "sending ACK frame\n";
    numAckSend++;
    sendDown(setBasicBitrate(buildACKFrame(frameToACK)));
}

void Ieee80211aMac::sendDataFrameOnEndSIFS(Ieee80211DataOrMgmtFrame *frameToSend)
{
    Ieee80211Frame *ctsFrame = (Ieee80211Frame *)endSIFS->getContextPointer();
    endSIFS->setContextPointer(NULL);

    sendDataFrame(frameToSend);
    delete ctsFrame;
}

void Ieee80211aMac::sendDataFrame(Ieee80211DataOrMgmtFrame *frameToSend)
{
    EV << "sending Data frame\n";
    PhyControlInfo *phyControlInfo = dynamic_cast<PhyControlInfo *>(frameToSend->getControlInfo() );
    if (phyControlInfo)
        phyControlInfo->setAdativeSensitivity(true);
    sendDown(buildDataFrame(frameToSend));
}

void Ieee80211aMac::sendBroadcastFrame(Ieee80211DataOrMgmtFrame *frameToSend)
{
    EV << "sending Broadcast frame\n";
    PhyControlInfo *phyControlInfo = dynamic_cast<PhyControlInfo *>(frameToSend->getControlInfo() );
    if (phyControlInfo)
        phyControlInfo->setAdativeSensitivity(true);
    sendDown(buildBroadcastFrame(frameToSend));
}

void Ieee80211aMac::sendRTSFrame(Ieee80211DataOrMgmtFrame *frameToSend)
{
    EV << "sending RTS frame\n";
    PhyControlInfo *phyControlInfo = dynamic_cast<PhyControlInfo *>(frameToSend->getControlInfo() );
    if (phyControlInfo)
        phyControlInfo->setAdativeSensitivity(true);
    sendDown(setBasicBitrate(buildRTSFrame(frameToSend)));
}

void Ieee80211aMac::sendCTSFrameOnEndSIFS()
{
    Ieee80211Frame *rtsFrame = (Ieee80211Frame *)endSIFS->getContextPointer();
    endSIFS->setContextPointer(NULL);
    sendCTSFrame(check_and_cast<Ieee80211RTSFrame*>(rtsFrame));
    delete rtsFrame;
}

void Ieee80211aMac::sendCTSFrame(Ieee80211RTSFrame *rtsFrame)
{
    EV << "sending CTS frame\n";
    PhyControlInfo *phyControlInfo = dynamic_cast<PhyControlInfo *>(rtsFrame->getControlInfo() );
    if (phyControlInfo)
        phyControlInfo->setAdativeSensitivity(true);
    sendDown(setBasicBitrate(buildCTSFrame(rtsFrame)));
}

void Ieee80211aMac::sendMulticastFrame(Ieee80211DataOrMgmtFrame *frameToSend)
{
    EV << "sending Multicast frame\n";
    PhyControlInfo *phyControlInfo = dynamic_cast<PhyControlInfo *>(frameToSend->getControlInfo() );
    if (phyControlInfo)
        phyControlInfo->setAdativeSensitivity(true);
    sendDown(buildMulticastFrame(frameToSend));
}//ahmed

/****************************************************************
 * Frame builder functions.
 */
Ieee80211DataOrMgmtFrame *Ieee80211aMac::buildDataFrame(Ieee80211DataOrMgmtFrame *frameToSend)
{
    Ieee80211DataOrMgmtFrame *frame = (Ieee80211DataOrMgmtFrame *)frameToSend->dup();

    if (isBroadcast(frameToSend))
        frame->setDuration(0);
    else if (!frameToSend->getMoreFragments())
        frame->setDuration(getSIFS() + computeFrameDuration(LENGTH_ACK, basicBitrate));
    else
        // FIXME: shouldn't we use the next frame to be sent?
        frame->setDuration(3 * getSIFS() + 2 * computeFrameDuration(LENGTH_ACK, basicBitrate) + computeFrameDuration(frameToSend));

    return frame;
}

Ieee80211ACKFrame *Ieee80211aMac::buildACKFrame(Ieee80211DataOrMgmtFrame *frameToACK)
{
    Ieee80211ACKFrame *frame = new Ieee80211ACKFrame("wlan-ack");
    frame->setReceiverAddress(frameToACK->getTransmitterAddress());

    if (!frameToACK->getMoreFragments())
        frame->setDuration(0);
    else
        frame->setDuration(frameToACK->getDuration() - getSIFS() - computeFrameDuration(LENGTH_ACK, basicBitrate));

    return frame;
}

Ieee80211RTSFrame *Ieee80211aMac::buildRTSFrame(Ieee80211DataOrMgmtFrame *frameToSend)
{
    Ieee80211RTSFrame *frame = new Ieee80211RTSFrame("wlan-rts");
    frame->setTransmitterAddress(address);
    frame->setReceiverAddress(frameToSend->getReceiverAddress());
    frame->setDuration(3 * getSIFS() + computeFrameDuration(LENGTH_CTS, basicBitrate) +
                       computeFrameDuration(frameToSend) +
                       computeFrameDuration(LENGTH_ACK, basicBitrate));

    return frame;
}

Ieee80211CTSFrame *Ieee80211aMac::buildCTSFrame(Ieee80211RTSFrame *rtsFrame)
{
    Ieee80211CTSFrame *frame = new Ieee80211CTSFrame("wlan-cts");
    frame->setReceiverAddress(rtsFrame->getTransmitterAddress());
    frame->setDuration(rtsFrame->getDuration() - getSIFS() - computeFrameDuration(LENGTH_CTS, basicBitrate));

    return frame;
}

Ieee80211DataOrMgmtFrame *Ieee80211aMac::buildBroadcastFrame(Ieee80211DataOrMgmtFrame *frameToSend)
{
    Ieee80211DataOrMgmtFrame *frame = (Ieee80211DataOrMgmtFrame *)frameToSend->dup();
    frame->setDuration(0);
    PhyControlInfo *phyControlInfo_old = dynamic_cast<PhyControlInfo *>( frameToSend->getControlInfo() );
    if (phyControlInfo_old)
    {
        ev<<"Per frame1 params"<<endl;
        PhyControlInfo *phyControlInfo_new=new PhyControlInfo;
        *phyControlInfo_new=*phyControlInfo_old;
        //EV<<"PhyControlInfo bitrate "<<phyControlInfo->getBitrate()/1e6<<"Mbps txpower "<<phyControlInfo->txpower()<<"mW"<<endl;
        frame->setControlInfo(  phyControlInfo_new  );
    }

    return frame;
}

Ieee80211DataOrMgmtFrame *Ieee80211aMac::buildMulticastFrame(Ieee80211DataOrMgmtFrame *frameToSend)
{
    Ieee80211DataOrMgmtFrame *frame = (Ieee80211DataOrMgmtFrame *)frameToSend->dup();
    frame->setDuration(getSIFS() + computeFrameDuration(LENGTH_ACK, basicBitrate));
    return frame;
}//ahmed

Ieee80211Frame *Ieee80211aMac::setBasicBitrate(Ieee80211Frame *frame)
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
void Ieee80211aMac::finishCurrentTransmission()
{
    popTransmissionQueue();
    resetStateVariables();
}

void Ieee80211aMac::giveUpCurrentTransmission()
{
    Ieee80211DataOrMgmtFrame *temp = (Ieee80211DataOrMgmtFrame*) transmissionQueue.front();
    nb->fireChangeNotification(NF_LINK_BREAK, temp);
    popTransmissionQueue();
    resetStateVariables();
    numGivenUp++;
}

void Ieee80211aMac::retryCurrentTransmission()
{
    ASSERT(retryCounter < retryLimit-1);
    getCurrentTransmission()->setRetry(true);
    if (rateControlMode == RATE_AARF || rateControlMode == RATE_ARF) reportDataFailed();
    else retryCounter ++;
    numRetry++;
    backoff = true;
    generateBackoffPeriod();
}

Ieee80211DataOrMgmtFrame *Ieee80211aMac::getCurrentTransmission()
{
    if (transmissionQueue.empty())
        return NULL;
    return (Ieee80211DataOrMgmtFrame *)transmissionQueue.front();
}

void Ieee80211aMac::sendDownPendingRadioConfigMsg()
{
    if (pendingRadioConfigMsg != NULL)
    {
        sendDown(pendingRadioConfigMsg);
        pendingRadioConfigMsg = NULL;
    }
}

void Ieee80211aMac::setMode(Mode mode)
{
    if (mode == PCF)
        error("PCF mode not yet supported");

    this->mode = mode;
}

void Ieee80211aMac::resetStateVariables()
{
    backoffPeriod = 0;
    if (rateControlMode == RATE_AARF || rateControlMode == RATE_ARF) reportDataOk ();
    else retryCounter = 0;

    if (!transmissionQueue.empty())
    {
        backoff = true;
        getCurrentTransmission()->setRetry(false);
    }
    else
    {
        backoff = false;
        noFrame=false;//sorin
    }
}

bool Ieee80211aMac::isMediumStateChange(cMessage *msg)
{
    return msg == mediumStateChange || (msg == endReserve && radioState == RadioState::IDLE);
}

bool Ieee80211aMac::isMediumFree()
{
    return radioState == RadioState::IDLE && !endReserve->isScheduled();
}

bool Ieee80211aMac::isBroadcast(Ieee80211Frame *frame)
{
    return frame && frame->getReceiverAddress().isBroadcast();
}

bool Ieee80211aMac::isMulticast(Ieee80211Frame *frame) //ahmed
{
    return frame && frame->getReceiverAddress().isMulticast();
}//ahmed

bool Ieee80211aMac::isForUs(Ieee80211Frame *frame)
{
    //int msgKind = frame->getKind();
    //bool lastReceiveFailed =(msgKind == COLLISION || msgKind == BITERROR);
    //if (frame && frame->getReceiverAddress() != address && !lastReceiveFailed)
    //  nb->fireChangeNotification(NF_LINK_PROMISCUOUS, frame);
    return frame && frame->getReceiverAddress() == address;
}

bool Ieee80211aMac::isSentByUs(Ieee80211Frame *frame)
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

bool Ieee80211aMac::isDataOrMgmtFrame(Ieee80211Frame *frame)
{
    return dynamic_cast<Ieee80211DataOrMgmtFrame*>(frame);
}

Ieee80211Frame *Ieee80211aMac::getFrameReceivedBeforeSIFS()
{
    return (Ieee80211Frame *)endSIFS->getContextPointer();
}

void Ieee80211aMac::popTransmissionQueue()
{
// Sorin changes
    if (!transmissionQueue.empty())
    {
        EV << "dropping frame from transmission queue\n";
        Ieee80211Frame *temp = transmissionQueue.front();
        transmissionQueue.pop_front();
        delete temp;
    }
    if (queueModule)
    {
        // tell queue module that we've become idle
        EV << "requesting another frame from queue module\n";
        queueModule->requestPacket();
        if (transmissionQueue.empty())
        {
            EV<<"\t but queue is empty\n";
            noFrame=true;
        }
    }
}

double Ieee80211aMac::computeFrameDuration(Ieee80211Frame *msg)
{
    return computeFrameDuration(msg->getBitLength(), bitrate);
}

double Ieee80211aMac::computeFrameDuration(int bits, double bitrate)
{
    double duration;
    if (opMode=='a')
        duration= (16+bits+6)/bitrate+PLCP_PREAMBLE_DELAY+PLCP_SIGNAL_DELAY+T_SYM/2;
    else if (opMode=='g')
        duration=4*ceil((16+bits+6)/(bitrate/1e6*4))*1e-6 + PHY_HEADER_LENGTH_G;
    else
        duration=bits / bitrate + PHY_HEADER_LENGTH_B / BITRATE_HEADER;

    if (bitrate>11e6)
        EV << "cuidado "<<endl;
    EV<<"MAC:frameDuration="<<duration*1e6<<"us("<<bits<<"bits "<< bitrate << "bitrate)"<<endl;
    return duration;
}


void Ieee80211aMac::reportDataOk ()
{
    retryCounter = 0;
    successCounter ++;
    failedCounter = 0;
    recovery = false;
    PHYRateVector.record(getBitrate()/1000000);

    if ((successCounter == getSuccessThreshold() || timer == getTimerTimeout ())
            && (rateIndex < (getMaxBitrate ())))
    {
        //  rateIndex++;
        //  if (opMode=='b') setBitrate(BITRATES_80211b[rateIndex]);
        //  else setBitrate(BITRATES_80211a[rateIndex]);
        timer = 0;
        successCounter = 0;
        recovery = true;
    }
}

void Ieee80211aMac::reportDataFailed (void)
{
    timer++;
    failedCounter++;
    retryCounter++;
    successCounter = 0;

    PHYRateVector.record(getBitrate()/1000000);

    if (recovery)
    {
        if (retryCounter == 1)
        {
            reportRecoveryFailure ();
            if (rateIndex != getMinBitrate ())
            {
                rateIndex--;
                if (opMode=='b') setBitrate(BITRATES_80211b[rateIndex]);
                else setBitrate(BITRATES_80211a[rateIndex]);
            }
        }
        timer = 0;
    }
    else
    {
        if (needNormalFallback ())
        {
            reportFailure ();
            if (rateIndex != getMinBitrate ())
            {
                rateIndex--;
                if (opMode=='b') setBitrate(BITRATES_80211b[rateIndex]);
                else setBitrate(BITRATES_80211a[rateIndex]);
            }
        }
        if (retryCounter >= 2)
        {
            timer = 0;
        }
    }
}

int Ieee80211aMac::getMinTimerTimeout (void)
{
    return minTimerTimeout;
}

int Ieee80211aMac::getMinSuccessThreshold (void)
{
    return minSuccessThreshold;
}

int Ieee80211aMac::getTimerTimeout (void)
{
    return timerTimeout;
}

int Ieee80211aMac::getSuccessThreshold (void)
{
    return successThreshold;
}

void Ieee80211aMac::setTimerTimeout (int timer_timeout)
{
    if (timer_timeout >= minTimerTimeout)
        timerTimeout = timer_timeout;
    else error("timer_timeout is less than minTimerTimeout");
}
void Ieee80211aMac::setSuccessThreshold (int success_threshold)
{
    if (success_threshold >= minSuccessThreshold)
        successThreshold = success_threshold;
    else error("success_threshold is less than minSuccessThreshold");
}

void Ieee80211aMac::reportRecoveryFailure (void)
{
    if (rateControlMode == RATE_AARF)
    {
        setSuccessThreshold ((int)(std::min ((double)getSuccessThreshold () * successCoeff,(double) maxSuccessThreshold)));
        setTimerTimeout ((int)(std::max ((double)getMinTimerTimeout (),(double)(getSuccessThreshold () * timerCoeff))));
    }
}

void Ieee80211aMac::reportFailure (void)
{
    if (rateControlMode == RATE_AARF)
    {
        setTimerTimeout (getMinTimerTimeout ());
        setSuccessThreshold (getMinSuccessThreshold ());
    }
}

bool Ieee80211aMac::needRecoveryFallback (void)
{
    if (retryCounter == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Ieee80211aMac::needNormalFallback (void)
{
    int retryMod = (retryCounter - 1) % 2;
    if (retryMod == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

double Ieee80211aMac::getBitrate()
{
    return bitrate;
}

void Ieee80211aMac::setBitrate(double rate)
{
    bitrate = rate;
}


int Ieee80211aMac::getMaxBitrate(void)
{
    if (opMode=='b')
        return 3;
    else return 7;
}

int Ieee80211aMac::getMinBitrate(void)
{
    return 0;
}

void Ieee80211aMac::logState()
{
//     stateVector.record(fsm.getState());
    EV  << "state information: mode = " << modeName(mode) << ", state = " << fsm.getStateName()
    << ", backoff = " << backoff << ", backoffPeriod = " << backoffPeriod
    << ", retryCounter = " << retryCounter << ", radioState = " << radioState
    << ", nav = " << nav << endl;
}


const char *Ieee80211aMac::modeName(int mode)
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


void Ieee80211aMac::finish()
{
    //     thStats.record();
    //     cwStats.record();
    //     recordScalar("Received Throughput", ((double)numReceivedBroadcast)*800.0/simTime()/1.0e6);
    //     recordScalar("Sent Throughput", ((double)numSentBroadcast*800.0)/simTime()/1.0e6);

    recordScalar("numSent", numSent);
    recordScalar("numSentWithoutRetry",numSentWithoutRetry);
    recordScalar("numReceived", numReceived);
    recordScalar("numSentBroadcast", numSentBroadcast);
    recordScalar("numReceivedBroadcast", numReceivedBroadcast);
    recordScalar("numReceivedOther", numReceivedOther);
    recordScalar("numCollision", numCollision);
    recordScalar("numGivenUp",numGivenUp);
    recordScalar("numAckSend", numAckSend);
    recordScalar("numRetry",numRetry);
}

