//
// Copyright (C) 2012 Martin Lévesque
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

#include "RadioState.h"
#include "IInterfaceTable.h"
#include "InterfaceTableAccess.h"
#include "PhyControlInfo_m.h"
#include "EtherMAC.h"
#include "Ieee80211Frame_m.h"
#include "FiWiTrafGen.h"
#include "MyUtil.h"
#include <utility>
#include <string>
#include <vector>
#include "BasicIeee80211Mac.h"
#include "RawStat.h"
#include "FiWiGeneralConfigs.h"
#include "FiWiRoutingTable.h"

using namespace std;

Define_Module(BasicIeee80211Mac);

// don't forget to keep synchronized the C++ enum and the runtime enum definition
Register_Enum(BasicIeee80211Mac,
              (BasicIeee80211Mac::IDLE,
               BasicIeee80211Mac::DEFER,
               BasicIeee80211Mac::WAITDIFS,
               BasicIeee80211Mac::BACKOFF,
               BasicIeee80211Mac::WAITACK,
               BasicIeee80211Mac::WAITCTS,
               BasicIeee80211Mac::WAITSIFS,
               BasicIeee80211Mac::RECEIVE,
               BasicIeee80211Mac::POSTBACKOFF));

// don't forget to keep synchronized the C++ enum and the runtime enum definition
Register_Enum(RadioState,
              (RadioState::IDLE,
               RadioState::RECV,
               RadioState::TRANSMIT,
               RadioState::SLEEP));

RawStat BasicIeee80211Mac::MACFrameLogs = RawStat("MACFrameLogs", "event", 1);
double BasicIeee80211Mac::cycleBegin = -1;

/****************************************************************
 * Construction functions.
 */
BasicIeee80211Mac::BasicIeee80211Mac()
{
    endSIFS = NULL;
    endDIFS = NULL;
    endTimeout = NULL;
    endReserve = NULL;
    mediumStateChange = NULL;
    pendingRadioConfigMsg = NULL;
}

BasicIeee80211Mac::~BasicIeee80211Mac()
{
    cancelAndDelete(endSIFS);
    cancelAndDelete(endDIFS);
    cancelAndDelete(endTimeout);
    cancelAndDelete(endReserve);
    cancelAndDelete(mediumStateChange);


    if (pendingRadioConfigMsg)
        delete pendingRadioConfigMsg;

    for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
    {
		while ( ! it->second.transmissionQueue.empty())

		{
			Ieee80211Frame *temp = it->second.transmissionQueue.front();
			it->second.transmissionQueue.pop_front();
			delete temp;
		}
    }
}

void BasicIeee80211Mac::handleCommand(cMessage *msg)
{
}

/****************************************************************
 * Initialization functions.
 */
void BasicIeee80211Mac::initialize(int stage)
{
    WirelessMacBase::initialize(stage);

    if (stage == 0)
    {
        EV << "Initializing stage 0\n";

        numRTS = 0;
		numCTSTimeout = 0;
		numDataSent = 0;
		numACKTimeout = 0;
		numBackoff = 0;
		sumBackoffs = 0;
		numCancelBackoff = 0;
		numDIFSBegin = 0;
		numDIFSEnd = 0;

        // initialize parameters
        // Variable to apply the fsm fix
        maxQueueSize = par("maxQueueSize");
        bitrate = par("bitrate");

        EV << "MAC bitrate = " << bitrate << endl;

        phyHeaderDuration = par("phyHeaderDuration").doubleValue();

        basicBitrate = bitrate;

        rtsThreshold = par("rtsThresholdBytes");

        vector<int> iClasses = MyUtil::extractIntVect(par("trafficClasses").stringValue());
        vector<int> ibackoffCWmins = MyUtil::extractIntVect(par("backoffCWmins").stringValue());
        vector<int> ibackoffCWmaxs = MyUtil::extractIntVect(par("backoffCWmaxs").stringValue());
        vector<int> ideltas = MyUtil::extractIntVect(par("deltas").stringValue());

        ASSERT(iClasses.size() > 0);

        for (int i = 0; i < (int)iClasses.size(); ++i)
        {
        	// std::vector<WiTrafClass> wiClasses
        	WiTrafClass c;

        	c.id = iClasses[i];
        	c.retryLimit = par("retryLimit");
        	c.delta = ideltas[i];
        	c.backoffCWmin = ibackoffCWmins[i];
        	c.backoffCWmax = ibackoffCWmaxs[i];
        	c.retryCounter = 0;
        	c.endBackoff = new cMessage("Backoff", c.id);
        	c.endPostBackoff = new cMessage("PostBackoff", c.id);
        	c.endAIFS = new cMessage("AIFS", c.id);
        	c.backoff = false;
        	c.backoffPeriod = -1;
        	c.addedAttemptInCycle = false;

        	EV << "WI CLASS " << c.id << " retry limit = " << c.retryLimit << " delta " << c.delta << " cwmin = " << c.backoffCWmin << " cwmax = " << c.backoffCWmax << " retry counter " << c.retryCounter << endl;

        	this->wiClasses[c.id] = c;
        }

        fsm.setName("Ieee80211Mac State Machine");
        WATCH(fsm);

        propagationDelay = par("propagationDelay").doubleValue();

        ASSERT(propagationDelay > 0);

        EV << "propagationDelay = " << propagationDelay << endl;

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
        endTimeout = new cMessage("Timeout");
        endReserve = new cMessage("Reserve");
        mediumStateChange = new cMessage("MediumStateChange");

        // interface
        registerInterface();

        // obtain pointer to external queue
        initializeQueueModule();

        // state variables

        mode = DCF;
        sequenceNumber = 0;
        radioState = RadioState::IDLE;
        lastReceiveFailed = false;
        nav = false;

        // statistics
        numRetry = 0;
        numSentWithoutRetry = 0;
        numGivenUp = 0;
        numCollision = 0;
        numSent = 0;
        numReceived = 0;
        numReceivedOther = 0;
        numAckSend = 0;

        // initialize watches

        WATCH(radioState);
        WATCH(nav);

        WATCH(numRetry);
        WATCH(numSentWithoutRetry);
        WATCH(numGivenUp);
        WATCH(numCollision);
        WATCH(numSent);
        WATCH(numReceived);
        radioModule = gate("lowergateOut")->getNextGate()->getOwnerModule()->getId();

        managementModule = dynamic_cast<BasicIeee80211MgmtAP*>(MyUtil::findModuleUp(this, "mgmt"));

        if ( ! managementModule)
        {
        	error("Problem in BasicIeee80211Mac::initialize, no management module.");
        }
    }
}

bool BasicIeee80211Mac::classSupported(int ac)
{
	for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
	{
		if (it->second.id == ac)
			return true;
	}

	return false;
}

void BasicIeee80211Mac::registerInterface()
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

    e->setMtu(par("mtu"));

    // capabilities
    e->setMulticast(true);
    e->setPointToPoint(false);

    // add
    ift->addInterface(e, this);
}

void BasicIeee80211Mac::initializeQueueModule()
{
	EV << "Requesting first two frames from queue module\n";
	requestPacket();
	// needed for backoff: mandatory if next message is already present
	requestPacket();

}

/****************************************************************
 * Message handling functions.
 */
void BasicIeee80211Mac::handleSelfMsg(cMessage *msg)
{
    EV << "received self message: " << msg << endl;

    if (msg == endReserve)
        nav = false;

    // POST BACKOFF HANDLING
    if (isEndPostBackoffMsg(msg))
    {

    	wiClasses[ACOfEndPostBackoff(msg)].backoff = false;
    	wiClasses[ACOfEndPostBackoff(msg)].backoffPeriod = -1;
    }
    // end post backoff

    if ( !strcmp(msg->getName(),"AIFS") || !strcmp(msg->getName(),"Backoff") )
	{
		EV << "Changing currentAC to " << msg->getKind() << endl;
		currentAC = msg->getKind();
		ASSERT(classSupported(currentAC));
	}

    if ( (strcmp(msg->getName(),"Backoff")== 0) || (strcmp(msg->getName(),"AIFS")==0))
	{
		int acCur;
		acCur = msg->getKind();
		EV <<" acCur is " << acCur << ",name is " << msg->getName() <<endl;

		for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
		{
			if (((it->second.endBackoff->isScheduled() &&  it->second.endBackoff->getArrivalTime() == simTime())
					|| ( it->second.endAIFS->isScheduled() && ! it->second.backoff && it->second.endAIFS->getArrivalTime() == simTime()))
					&& ! it->second.transmissionQueue.empty() && it->second.id != acCur)
			{
				int acInColl = min(acCur, it->second.id);

				EV << "Internal collision AC" << acInColl << " with AC" << acCur << " or " << it->second.id << endl;

				MACFrameLogs.log(simTime().dbl(), acInColl, "INTERNAL COLLISION");

				EV << "Cancel backoff event and schedule new one for AC" << acCur << endl;
				cancelEvent(wiClasses[acInColl].endBackoff);

				EV << "retry transmission for AC" << currentAC << endl;
				if (wiClasses[acInColl].retryCounter < wiClasses[acInColl].retryLimit - 1)
				{
					retryCurrentTransmission(acInColl);
				}
				else
				{
					giveUpCurrentTransmission(acInColl);
				}

				// PUT BACK THE higher priority class
				currentAC = max(acCur, it->second.id);

				return;
			}
		}
	}

    handleWithFSM(msg);
}

void BasicIeee80211Mac::handleUpperMsg(cPacket *msg)
{
	// the module are continuously asking for packets
	EV << "requesting another frame from queue module\n";
	requestPacket();

    // must be a Ieee80211DataOrMgmtFrame, within the max size because we don't support fragmentation
    Ieee80211DataOrMgmtFrame *frame = check_and_cast<Ieee80211DataOrMgmtFrame *>(msg);

    /* No more check, for aggregation
     *
    if (frame->getByteLength() > fragmentationThreshold)
        error("message from higher layer (%s)%s is too long for 802.11b, %d bytes (fragmentation is not supported yet)",
              msg->getClassName(), msg->getName(), (int)(msg->getByteLength()));
    */

    ASSERT(frame);

    EV << "frame " << frame << " received from higher layer, receiver = " << frame->getReceiverAddress() << endl;

    ASSERT(!frame->getReceiverAddress().isUnspecified());

    // fill in missing fields (receiver address, seq number), and insert into the queue
    frame->setTransmitterAddress(address);
    frame->setSequenceNumber(sequenceNumber);
    sequenceNumber = (sequenceNumber+1) % 4096;  // seqNum must be checked upon reception of frames!


    if ( ! MappingAccessCategory(frame))
	{
		return;
	}

    beginNewCycle();

    handleWithFSM(frame);
}

bool BasicIeee80211Mac::MappingAccessCategory(Ieee80211DataOrMgmtFrame *frame)
{
	ListEtherFrames* subFrames = dynamic_cast<ListEtherFrames*>(frame->getEncapsulatedMsg());

	ASSERT(subFrames && subFrames->getFramesArraySize() > 0);

	int ac = subFrames->getFrames(0).getTrafficClass();

	ASSERT(classSupported(ac));

	if (maxQueueSize && (int)wiClasses[ac].transmissionQueue.size() >= maxQueueSize)
	{
		EV << "message " << frame << " received from higher layer but AC queue is full, dropping message\n";
		delete frame;
		return false;
	}

	wiClasses[ac].transmissionQueue.push_back(frame);

	return true;
}

void BasicIeee80211Mac::handleLowerMsg(cPacket *msg)
{
    EV << "received message from lower layer: " << msg << endl;

    Ieee80211Frame *frame = dynamic_cast<Ieee80211Frame *>(msg);

    nb->fireChangeNotification(NF_LINK_FULL_PROMISCUOUS, msg);

    if (msg->getControlInfo())
        delete msg->removeControlInfo();

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
    if (msgKind != COLLISION && ! entirelyInError(frame) && twoAddressFrame!=NULL)
        nb->fireChangeNotification(NF_LINK_REFRESH, twoAddressFrame);
#endif

    handleWithFSM(msg);

    // if we are the owner then we did not send this message up
    if (msg->getOwner() == this)
        delete msg;
}

void BasicIeee80211Mac::requestPacket()
{
	// Send packet to UPPER
	Ieee80211DataFrame* frame = new Ieee80211DataFrame("requestPacket");

	frame->setKind(IEEE80211MAC_REQUEST_FOR_PACKET);

	sendUp(frame);
}

bool BasicIeee80211Mac::transmissionQueueEmpty(int ac)
{
	return wiClasses[ac].transmissionQueue.empty();
}

bool BasicIeee80211Mac::transmissionQueueEmpty()
{
	for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
	{
		if ( ! it->second.transmissionQueue.empty())
			return false;
	}

	return true;
}

void BasicIeee80211Mac::receiveChangeNotification(int category, const cPolymorphic *details)
{
    Enter_Method_Silent();
    printNotificationBanner(category, details);

    if (category == NF_RADIOSTATE_CHANGED)
    {
        RadioState * rstate = check_and_cast<RadioState *>(details);
        if (rstate->getRadioId()!=getRadioModuleId())
            return;

        RadioState::State newRadioState = rstate->getState();

        radioState = newRadioState;

        handleWithFSM(mediumStateChange);
    }
}

/**
 * Msg can be upper, lower, self or NULL (when radio state changes)
 */
void BasicIeee80211Mac::handleWithFSM(cMessage *msg)
{

    if (isUpperMsg(msg) && fsm.getState() != IDLE && fsm.getState() != POSTBACKOFF)
    {
        EV << "deferring upper message transmission in " << fsm.getStateName() << " state\n";
        return;
    }

    Ieee80211Frame* frame = dynamic_cast<Ieee80211Frame*>(msg);
    int frameType = frame ? frame->getType() : -1;
    int msgKind = msg->getKind();

    EV << "OK frame type = " << frameType << " ST_ACK = " << ST_ACK << " ST_RTS = " << ST_RTS << " ST_CTS = " << ST_CTS << endl;

    if (frame && isLowerMsg(frame))
    {
    	bool tmpEntirelyInError = entirelyInError(frame);
    	bool gotCollision = msgKind == COLLISION;
        lastReceiveFailed = (gotCollision || tmpEntirelyInError);
        EV << "failed ? " << lastReceiveFailed << endl;
        EV << "tmpEntirelyInError " << tmpEntirelyInError << endl;
        EV << "gotCollision " << gotCollision << endl;

        if ( ! gotCollision && ! tmpEntirelyInError)
        	scheduleReservePeriod(frame, frame->getDuration(), isForUs(frame));
    }

    FSMA_Switch(fsm)
    {
        FSMA_State(IDLE)
        {
        	EV << "BEFORE IDLE " << endl;
            FSMA_Enter(sendDownPendingRadioConfigMsg());

            FSMA_Event_Transition(Data-Ready,
                                  isUpperMsg(msg),
                                  DEFER,
                                  //ASSERT(isInvalidBackoffPeriod());
                                  invalidateAllBackoffPeriods();
                                 );
            FSMA_No_Event_Transition(Immediate-Data-Ready,
									!transmissionQueueEmpty(),
                                     DEFER,
                                     invalidateAllBackoffPeriods();
                                    );
            FSMA_Event_Transition(Receive,
                                  isLowerMsg(msg),
                                  RECEIVE,
                                 );

            EV << "AFTER IDLE " << endl;
        }
        FSMA_State(DEFER)
        {
            FSMA_Enter(sendDownPendingRadioConfigMsg());
            FSMA_Event_Transition(Wait-DIFS,
                                  (isMediumStateChange(msg) && isMediumFree()),
                                  WAITDIFS,
                                  ;);
            FSMA_No_Event_Transition(Immediate-Wait-DIFS,
                                     (isMediumFree()),
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


			FSMA_Event_Transition(Immediate-Transmit-RTS,
								isMsgAIFS(msg) && ! wiClasses[ACOfAIFSMsg(msg)].transmissionQueue.empty()
								  && getCurrentTransmission(ACOfAIFSMsg(msg))->getByteLength() >= rtsThreshold && ! wiClasses[ACOfAIFSMsg(msg)].backoff, // At the end of DIFS and no backoff !
								  WAITCTS,
								  sendRTSFrame(getCurrentTransmission(ACOfAIFSMsg(msg)));
								  cancelDIFSPeriod();
								 );
			FSMA_Event_Transition(Immediate-Transmit-Data,
								isMsgAIFS(msg) && ! wiClasses[ACOfAIFSMsg(msg)].backoff,
								  WAITACK,
								  sendDataFrame(getCurrentTransmission(ACOfAIFSMsg(msg)));
								  cancelDIFSPeriod();
								 );

            FSMA_Event_Transition(DIFS-Over,
								isMsgAIFS(msg),
                                  BACKOFF,
                                  ASSERT(wiClasses[ACOfAIFSMsg(msg)].backoff);

                                  if (isInvalidBackoffPeriod(ACOfAIFSMsg(msg)))
                                	  generateBackoffPeriod(ACOfAIFSMsg(msg), false);
                                  else
                                  if (wiClasses[ACOfAIFSMsg(msg)].backoffPeriod >= getSlotTime())
                                	  wiClasses[ACOfAIFSMsg(msg)].backoffPeriod -= getSlotTime(); // TODO: new
                                 );
            FSMA_Event_Transition(Busy,
                                  isMediumStateChange(msg) && !isMediumFree(), // sense BUSY
                                  DEFER,
                                  toBackoff(); // all classes
                                  cancelDIFSPeriod();
                                 );
            FSMA_No_Event_Transition(Immediate-Busy,
                                     !isMediumFree(), // sense BUSY
                                     DEFER,
                                     toBackoff(); // all classes
                                     cancelDIFSPeriod();
                                    );
            // radio state changes before we actually get the message, so this must be here
            FSMA_Event_Transition(Receive,
                                  isLowerMsg(msg),
                                  RECEIVE,
                                  cancelDIFSPeriod();
                                  ;);
        }
        FSMA_State(POSTBACKOFF)
		{
			FSMA_Enter(schedulePostBackoffPeriods());

			FSMA_Event_Transition(Backoff-Force-IDLE-After-Backoff,
				isEndPostBackoffMsg(msg) && allPostBackoffsFinished(),
				  IDLE,
				  resetAllStateVariables();
				 );
			FSMA_Event_Transition(POSTBACKOFF-POST-BACKOFF-STOPPED1,
									  (isUpperMsg(msg)),
									  DEFER,
									  terminatePostBackoffPeriods();
									 );
			FSMA_No_Event_Transition(POSTBACKOFF-POST-BACKOFF-STOPPED2,
									  (! transmissionQueueEmpty()),
									  DEFER,
									  terminatePostBackoffPeriods();
									 );
			FSMA_Event_Transition(POSTBACKOFF-Receive,
								  isLowerMsg(msg),
								  RECEIVE,
								  terminatePostBackoffPeriods();
								  ;);
			FSMA_Event_Transition(POSTBackoff-Busy,
								  isMediumStateChange(msg) && !isMediumFree(),
								  DEFER,
								  terminatePostBackoffPeriods();
								 );
		}
        FSMA_State(BACKOFF)
        {
            FSMA_Enter(scheduleBackoffPeriod());

            FSMA_Event_Transition(AIFS-Over-backoff,
							  isMsgAIFS(msg) && wiClasses[currentAC].backoff,
							  BACKOFF,
							  if (isInvalidBackoffPeriod(currentAC))
								  generateBackoffPeriod(currentAC, false);
							  else
							  if (wiClasses[currentAC].backoffPeriod >= getSlotTime())
								  wiClasses[currentAC].backoffPeriod -= getSlotTime(); // TODO: new
							 );

			FSMA_Event_Transition(Transmit-RTS,
								  msg == wiClasses[currentAC].endBackoff
								  && getCurrentTransmission(currentAC)->getByteLength() >= rtsThreshold,
								  WAITCTS,
								  sendRTSFrame(getCurrentTransmission(currentAC));
									updateStatsAtEndOfBackoff();
									terminateBackoffPeriod();
								 );
			FSMA_Event_Transition(Transmit-Data,
								  msg == wiClasses[currentAC].endBackoff,
								  WAITACK,
								  sendDataFrame(getCurrentTransmission(currentAC));
									updateStatsAtEndOfBackoff();
									terminateBackoffPeriod();
								 );

            FSMA_Event_Transition(AIFS-Immediate-Transmit-RTS,
								  isMsgAIFS(msg) && ! wiClasses[currentAC].transmissionQueue.empty()
								  && getCurrentTransmission(currentAC)->getByteLength() >= rtsThreshold && ! wiClasses[currentAC].backoff,
								  WAITCTS,
								  sendRTSFrame(getCurrentTransmission(currentAC));
									updateStatsAtEndOfBackoff();
									terminateBackoffPeriod();
								 );

            FSMA_Event_Transition(AIFS-Immediate-Transmit-Data,
								  isMsgAIFS(msg) && ! wiClasses[currentAC].backoff,
								  WAITACK,
								  sendDataFrame(getCurrentTransmission(currentAC));
									updateStatsAtEndOfBackoff();
									terminateBackoffPeriod();
								 );

            FSMA_Event_Transition(Backoff-Idle,
                                  msg == wiClasses[currentAC].endBackoff && transmissionQueueEmpty(),
                                  IDLE,
                                  ASSERT(false);
                                 );
            // radio state changes before we actually get the message, so this must be here
			FSMA_Event_Transition(Receive,
								  isLowerMsg(msg),
								  RECEIVE,
								  terminateBackoffPeriod();
								  ;);
            FSMA_Event_Transition(Backoff-Busy,
                                  isMediumStateChange(msg) && !isMediumFree(),
                                  DEFER,
                                  terminateBackoffPeriod();
                                 );
        }
        FSMA_State(WAITACK)
        {
            FSMA_Enter(scheduleDataTimeoutPeriod(getCurrentTransmission(currentAC)));
            FSMA_Event_Transition(Receive-ACK,
                                  isLowerMsg(msg) && isForUs(frame) && frameType == ST_ACK,
                                  POSTBACKOFF,
                                  numSent++;
                                  cancelTimeoutPeriod();
                                  finishCurrentTransmission(frame);
                                  //resetAllBackOff();
                                 );
            FSMA_Event_Transition(Transmit-Data-Failed,
                                  msg == endTimeout,
                                  IDLE,
                                  retryAllACsCurrentTransmission();
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
                                  msg == endTimeout,
                                  IDLE,
                                  retryAllACsCurrentTransmission();
                                 );
        }
        FSMA_State(WAITSIFS)
        {
            FSMA_Enter(scheduleSIFSPeriod(frame));
            FSMA_Event_Transition(Transmit-CTS,
                                  msg == endSIFS && getFrameReceivedBeforeSIFS()->getType() == ST_RTS,
                                  IDLE,
                                  sendCTSFrameOnEndSIFS();
                                  //resetStateVariables();
									);
            FSMA_Event_Transition(Transmit-DATA,
                                  msg == endSIFS && getFrameReceivedBeforeSIFS()->getType() == ST_CTS,
                                  WAITACK,
                                  sendDataFrameOnEndSIFS(getCurrentTransmission(currentAC));
                                 );
            FSMA_Event_Transition(Transmit-ACK,
                                  msg == endSIFS && isDataOrMgmtFrame(getFrameReceivedBeforeSIFS()),
                                  IDLE,
                                  sendACKFrameOnEndSIFS();
                                  //resetStateVariables();
									);
        }
        // this is not a real state
        FSMA_State(RECEIVE)
        {
        	EV << "OK BEGIN RECEIVE BEGIN" << endl;

            FSMA_No_Event_Transition(Immediate-Receive-Error,
                                     isLowerMsg(msg) && (msgKind == COLLISION || entirelyInError(frame)),
                                     IDLE,
                                     EV << "received frame contains bit errors or collision\n";
                                     numCollision++;
                                     //resetStateVariables();
                                     EV << "receive, STATE = Immediate-Receive-Error COLLS = " << numCollision << endl;
                                        );
            FSMA_No_Event_Transition(Immediate-Receive-Data,
                                     isLowerMsg(msg) && isForUs(frame) && isDataOrMgmtFrame(frame),
                                     WAITSIFS,
                                     sendUp(frame);
                                     numReceived++;
                                     EV << "receive, STATE = Immediate-Receive-Data " << endl;
                                    );
            FSMA_No_Event_Transition(Immediate-Receive-RTS,
                                     isLowerMsg(msg) && isForUs(frame) && frameType == ST_RTS,
                                     WAITSIFS,
                                     EV << "receive, STATE = Immediate-Receive-RTS " << endl;
                                    );

            FSMA_No_Event_Transition(Immediate-Promiscuous-Data,
                                     isLowerMsg(msg) && !isForUs(frame) && isDataOrMgmtFrame(frame),
                                     IDLE,
                                     nb->fireChangeNotification(NF_LINK_PROMISCUOUS, frame);
                                     //resetStateVariables();
                                         numReceivedOther++;
                                         EV << "receive, STATE = Immediate-Promiscuous-Data " << endl;
                                        );
            FSMA_No_Event_Transition(Immediate-Receive-Other,
                                     isLowerMsg(msg),
                                     IDLE,
                                     //resetStateVariables();
									 numReceivedOther++;
                                         EV << "receive, STATE = Immediate-Receive-Other " << endl;
                                        );
        }
    }

    logState();
}

/****************************************************************
 * Timing functions.
 */
simtime_t BasicIeee80211Mac::getSIFS()
{
    return 0.000016; // SIFS
}

simtime_t BasicIeee80211Mac::getSlotTime()
{
    return 0.000009; // ST, epsilon in the paper
}

simtime_t BasicIeee80211Mac::getPIFS()
{
    return getSIFS() + getSlotTime();
}

simtime_t BasicIeee80211Mac::getDIFS(int ac)
{
    return 0.000034 + (double)wiClasses[ac].delta * getSlotTime();//getSIFS() + 2 * getSlotTime();
}

simtime_t BasicIeee80211Mac::computeBackoffPeriod(Ieee80211Frame *msg, int r, int ac, bool inPostBackoff)
{
    int cw;



    EV << "generating backoff slot number for retry: " << r << endl;

	ASSERT(0 <= r && r <= wiClasses[ac].retryLimit);

	EV << " rrrrrrrrrra = " << r << endl;

	cw = (wiClasses[ac].backoffCWmin) * (1 << r) - 1;

	if (cw > wiClasses[ac].backoffCWmax)
		cw = wiClasses[ac].backoffCWmax - 1;

    //int c = intrand(cw + 1);
    static int previousC = -1;
    int c;

    while ((c= (int)uniform(0, cw + 1)) == previousC)
    {

    }

    EV << "generated backoff slot number: " << c << " , cw: " << cw << endl;

    static RawStat back = RawStat("MACCwRand", "event", 1);
    back.log(simTime().dbl(), c, this->address.str());

    previousC = c;

    if (c > 0 && ! inPostBackoff)
    	c -= 1; // DECREMENT BEFORE DIFS

    return ((double)c) * getSlotTime();
}

/****************************************************************
 * Timer functions.
 */
void BasicIeee80211Mac::scheduleSIFSPeriod(Ieee80211Frame *frame)
{
	ASSERT(endSIFS);
    EV << "scheduling SIFS period\n";
    endSIFS->setContextPointer(frame->dup());
    scheduleAt(simTime() + getSIFS(), endSIFS);
}

void BasicIeee80211Mac::scheduleDIFSPeriod()
{
	EV << "STARTING DIFS" << endl;

	bool scheduled = false;

	for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
	{
		if ( ! it->second.endAIFS->isScheduled() && ! it->second.transmissionQueue.empty())
		{
			EV << "scheduling AIFS period (" << it->second.id << ")\n";
			scheduleAt(simTime() + getDIFS(it->second.id), it->second.endAIFS);
		}

		if (it->second.endAIFS->isScheduled())
			scheduled = true;
	}

	ASSERT(scheduled);
}

int BasicIeee80211Mac::ACOfAIFSMsg(cMessage *msg)
{
	for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
	{
		if (msg == it->second.endAIFS)
			return it->second.id;
	}

	ASSERT(false);

	return -1;
}

bool BasicIeee80211Mac::isMsgAIFS(cMessage *msg)
{
	for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
	{
		if (msg == it->second.endAIFS)
			return true;
	}

	return false;
}

void BasicIeee80211Mac::cancelDIFSPeriod()
{
	for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
	{
		cancelEvent(it->second.endAIFS);
	}
}

// Should be dynamic based on the next hop!!!
double BasicIeee80211Mac::estimatedPropagationDelay()
{
	double res = propagationDelay * 1.01; // + 5%

	EV << "estimated prop delay " << res << endl;

	return res;
}

void BasicIeee80211Mac::scheduleDataTimeoutPeriod(Ieee80211DataOrMgmtFrame *frameToSend)
{
    EV << "scheduling data timeout period\n";
    scheduleAt(simTime() + phyHeaderDuration + getSIFS() + computeFrameDuration(LENGTH_ACK, basicBitrate) +
    		computeFrameDuration(frameToSend) +
    		+ estimatedPropagationDelay() * 2.0, endTimeout);
}

void BasicIeee80211Mac::cancelTimeoutPeriod()
{
    EV << "cancelling timeout period\n";
    cancelEvent(endTimeout);
}

double BasicIeee80211Mac::durationRTSCTS()
{
	return (computeFrameDuration(LENGTH_RTS, basicBitrate) + getSIFS() + computeFrameDuration(LENGTH_CTS, basicBitrate) + estimatedPropagationDelay() * 2).dbl();
}

void BasicIeee80211Mac::scheduleCTSTimeoutPeriod()
{
    scheduleAt(simTime() + durationRTSCTS(), endTimeout);
}

void BasicIeee80211Mac::scheduleReservePeriod(Ieee80211Frame *frame, simtime_t duration, bool forUs)
{
    simtime_t reserve = duration;

    if ( (! forUs || frame->getType() == ST_RTS) && reserve != 0) // No limit on reserve
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

void BasicIeee80211Mac::invalidateBackoffPeriod(int ac)
{
	EV << "invalidateBackoffPeriod " << ac << endl;
	ASSERT(classSupported(ac));
	wiClasses[ac].backoffPeriod = -1;
}

void BasicIeee80211Mac::toBackoff()
{
	for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
	{
		it->second.backoff = true;
	}
}

void BasicIeee80211Mac::invalidateAllBackoffPeriods()
{
	for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
	{
		invalidateBackoffPeriod(it->second.id);
	}
}

bool BasicIeee80211Mac::isInvalidBackoffPeriod(int ac)
{
	ASSERT(classSupported(ac));
    return wiClasses[ac].backoffPeriod == -1;
}

void BasicIeee80211Mac::generateBackoffPeriod(int ac, bool inPostBackoff)
{
	ASSERT(classSupported(ac));

	wiClasses[ac].backoffPeriod = computeBackoffPeriod(getCurrentTransmission(ac), wiClasses[ac].retryCounter, ac, inPostBackoff);

    ASSERT(wiClasses[ac].backoffPeriod >= 0);
    EV << "backoff period set to " << wiClasses[ac].backoffPeriod << endl;
}

// TO CHECK!!
void BasicIeee80211Mac::decreaseBackoffPeriod()
{
	for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
	{
		if (it->second.backoff && it->second.endBackoff->isScheduled())
		{
			simtime_t elapsedBackoffTime = simTime() - it->second.endBackoff->getSendingTime();
			it->second.backoffPeriod -= ((int)(elapsedBackoffTime / getSlotTime())) * getSlotTime();

			ASSERT(it->second.backoffPeriod >= 0);
			EV << "backoff period decreased to " << it->second.backoffPeriod << endl;


		}
		else
		{
		}
	}
}

void BasicIeee80211Mac::terminateBackoffPeriod() // in the middle, maybe
{
	cancelDIFSPeriod();
	decreaseBackoffPeriod();
	cancelBackoffPeriod();
}

int BasicIeee80211Mac::ACOfEndPostBackoff(cMessage* msg)
{
	for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
	{
		if (it->second.endPostBackoff == msg)
			return it->second.id;
	}

	ASSERT(false);

	return -1;
}

void BasicIeee80211Mac::terminatePostBackoffPeriods()
{
	for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
	{
		if (it->second.backoff && it->second.endPostBackoff->isScheduled())
		{
			simtime_t elapsedBackoffTime = simTime() - it->second.endPostBackoff->getSendingTime();
			it->second.backoffPeriod -= ((int)(elapsedBackoffTime / getSlotTime())) * getSlotTime();

			ASSERT(it->second.backoffPeriod >= 0);
			EV << "backoff period decreased to " << it->second.backoffPeriod << endl;

			if (it->second.backoffPeriod <= 0)
			{
				it->second.backoff = false;
				it->second.backoffPeriod = -1;
			}
		}
		else
		{
			it->second.backoff = false;
			it->second.backoffPeriod = -1;
		}

		// In any case, we need to cancel end post backoff
		cancelEvent(it->second.endPostBackoff);
	}
}

bool BasicIeee80211Mac::isEndPostBackoffMsg(cMessage* msg)
{
	for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
	{
		if (it->second.endPostBackoff == msg)
			return true;
	}

	return false;
}

bool BasicIeee80211Mac::allPostBackoffsFinished()
{
	for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
	{
		if (it->second.endPostBackoff->isScheduled())
		{
			return false;
		}
	}

	return true;
}

void BasicIeee80211Mac::schedulePostBackoffPeriods()
{
	// currentAC:
	generateBackoffPeriod(currentAC, true);

	scheduleAt(simTime() + wiClasses[currentAC].backoffPeriod, wiClasses[currentAC].endPostBackoff);

	// We also continue backoffs of other ACs!!!
	for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
	{
		if (it->second.id == currentAC)
			continue;

		if (it->second.backoff)
		{
			if (isInvalidBackoffPeriod(it->second.id))
			{
				generateBackoffPeriod(it->second.id, true);
			}

			EV << "backof perid schedule post back... " << it->second.backoffPeriod << " of node " << it->second.id << endl;
			ASSERT(it->second.backoffPeriod >= 0);

			scheduleAt(simTime() + wiClasses[it->second.id].backoffPeriod, wiClasses[it->second.id].endPostBackoff);
		}
	}
}

void BasicIeee80211Mac::scheduleBackoffPeriod()
{
	ASSERT(classSupported(currentAC));
    EV << "scheduling backoff period\n";
    scheduleAt(simTime() + wiClasses[currentAC].backoffPeriod, wiClasses[currentAC].endBackoff);
    EV << " backoff period = " << wiClasses[currentAC].backoffPeriod << endl;
    EV << " transmissionQueueEmpty() = " << transmissionQueueEmpty() << endl;

}

void BasicIeee80211Mac::cancelBackoffPeriod()
{
    EV << "cancelling Backoff period\n";

    for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
    {
    	cancelEvent(it->second.endBackoff);
    }
}

bool BasicIeee80211Mac::entirelyInError(Ieee80211Frame* frame)
{
	Ieee80211DataFrame* dataFrame = dynamic_cast<Ieee80211DataFrame*>(frame);

	if ( ! dataFrame)
	{
		return false;
	}

	ListEtherFrames* listFrames = dynamic_cast<ListEtherFrames*>(dataFrame->getEncapsulatedPacket());

	if (listFrames->getFramesArraySize() == 0)
		return false;

	bool allSubPacketsInError = true;

	EV << "sending ack" << endl;

	for (int i = 0; i < (int)listFrames->getFramesArraySize(); ++i)
	{
		EV << "kind = " << listFrames->getFrames(i).getKind() << endl;

		if (listFrames->getFrames(i).getKind() != BITERROR)
		{
			allSubPacketsInError = false;
			break;
		}
	}

	return allSubPacketsInError;
}

/****************************************************************
 * Frame sender functions.
 */
void BasicIeee80211Mac::sendACKFrameOnEndSIFS()
{
    Ieee80211Frame *frameToACK = (Ieee80211Frame *)endSIFS->getContextPointer();
    endSIFS->setContextPointer(NULL);

    // If all sub packets are in ERROR, there is no need to send the ack since the entire packet will be retransmitted after the timeout

    EV << "sending ack" << endl;

    sendACKFrame(check_and_cast<Ieee80211DataOrMgmtFrame*>(frameToACK));


    delete frameToACK;
}

void BasicIeee80211Mac::sendACKFrame(Ieee80211DataOrMgmtFrame *frameToACK)
{
    EV << "sending ACK frame\n";
    numAckSend++;
    sendDown(setBasicBitrate(buildACKFrame(frameToACK)));
}

void BasicIeee80211Mac::sendDataFrameOnEndSIFS(Ieee80211DataOrMgmtFrame *frameToSend)
{
    Ieee80211Frame *ctsFrame = (Ieee80211Frame *)endSIFS->getContextPointer();
    endSIFS->setContextPointer(NULL);
    sendDataFrame(frameToSend);
    delete ctsFrame;
}

void BasicIeee80211Mac::sendDataFrame(Ieee80211DataOrMgmtFrame *frameToSend)
{
    EV << "sending Data frame at " << simTime() << endl;
    sendDown(buildDataFrame(frameToSend));

    if (simTime().dbl() >= 2)
    	++numDataSent;
}

void BasicIeee80211Mac::sendRTSFrame(Ieee80211DataOrMgmtFrame *frameToSend)
{
    EV << "sending RTS frame\n";
    sendDown(setBasicBitrate(buildRTSFrame(frameToSend)));

    if (simTime().dbl() >= 2)
    	++numRTS;
}

void BasicIeee80211Mac::sendCTSFrameOnEndSIFS()
{
    Ieee80211Frame *rtsFrame = (Ieee80211Frame *)endSIFS->getContextPointer();
    endSIFS->setContextPointer(NULL);
    sendCTSFrame(check_and_cast<Ieee80211RTSFrame*>(rtsFrame));
    delete rtsFrame;
}

void BasicIeee80211Mac::sendCTSFrame(Ieee80211RTSFrame *rtsFrame)
{
    EV << "sending CTS frame\n";
    sendDown(setBasicBitrate(buildCTSFrame(rtsFrame)));
}

/****************************************************************
 * Frame builder functions.
 */
Ieee80211DataOrMgmtFrame* BasicIeee80211Mac::buildDataFrame(Ieee80211DataOrMgmtFrame *frameToSend)
{
    Ieee80211DataOrMgmtFrame *frame = (Ieee80211DataOrMgmtFrame *)frameToSend->dup();

	frame->setDuration(getSIFS() + computeFrameDuration(LENGTH_ACK, basicBitrate)
			+ estimatedPropagationDelay()
			);

    EV << "BasicIeee80211Mac::buildDataFrame 2" << endl;

    static double lastFrame = simTime().dbl();

    EV << "------------------ SENDING DOWN ------------  .. frame lenght in bits = " << frameToSend->getBitLength() << " t = " << simTime() << " last one = " << lastFrame << " dif time = " << simTime().dbl() - lastFrame << endl;

    lastFrame = simTime().dbl();

    return frame;
}

Ieee80211ACKFrame *BasicIeee80211Mac::buildACKFrame(Ieee80211DataOrMgmtFrame *frameToACK)
{
	// This long name is useful to know which packet is actually acked.
	string ackName = string("wlan-ack-") + string(frameToACK->getFullName());

	ListEtherFrames* framesInFrame = dynamic_cast<ListEtherFrames*>(frameToACK->getEncapsulatedPacket());

    Ieee80211ACKFrame *frame = new Ieee80211ACKFrame(ackName.c_str());
    frame->setReceiverAddress(frameToACK->getTransmitterAddress());
    frame->setFiwiType(frameToACK->getFiwiType());

	frame->setDuration(0);
	EV << "BasicIeee80211Mac::buildACKFrame 111 !!!" << endl;

    // Add the packet ids not received because of BER
    if (framesInFrame)
    {
    	int nbPacketsNotReceived = 0;

		for (int i = 0; i < (int)framesInFrame->getFramesArraySize(); ++i)
		{
			if (framesInFrame->getFrames(i).getKind() == BITERROR)
			{
				frame->setPacketsNotReceivedArraySize(nbPacketsNotReceived + 1);

				frame->setPacketsNotReceived(nbPacketsNotReceived, i);

				++nbPacketsNotReceived;
			}
		}
    }

    return frame;
}

Ieee80211RTSFrame *BasicIeee80211Mac::buildRTSFrame(Ieee80211DataOrMgmtFrame *frameToSend)
{
	string pktName = string("wlan-rts-") + string(frameToSend->getFullName());

    Ieee80211RTSFrame *frame = new Ieee80211RTSFrame(pktName.c_str());
    frame->setTransmitterAddress(address);
    frame->setReceiverAddress(frameToSend->getReceiverAddress());
    frame->setDuration(3 * getSIFS() + computeFrameDuration(LENGTH_CTS, basicBitrate) +
                       computeFrameDuration(frameToSend) +
                       phyHeaderDuration +
                       computeFrameDuration(LENGTH_ACK, basicBitrate)
                       + estimatedPropagationDelay() * 3
						);
    frame->setFiwiType(frameToSend->getFiwiType());

    EV << "BasicIeee80211Mac::buildRTSFrame cts len = " << frame->getBitLength() << endl;

    return frame;
}

Ieee80211CTSFrame *BasicIeee80211Mac::buildCTSFrame(Ieee80211RTSFrame *rtsFrame)
{
	string pktName = string("wlan-cts-") + string(rtsFrame->getFullName());

    Ieee80211CTSFrame *frame = new Ieee80211CTSFrame(pktName.c_str());
    frame->setReceiverAddress(rtsFrame->getTransmitterAddress());
    frame->setDuration(rtsFrame->getDuration() - getSIFS() - computeFrameDuration(LENGTH_CTS, basicBitrate)
    		- estimatedPropagationDelay()
    		);
    frame->setFiwiType(rtsFrame->getFiwiType());

    return frame;
}

Ieee80211Frame *BasicIeee80211Mac::setBasicBitrate(Ieee80211Frame *frame)
{
    ASSERT(frame->getControlInfo()==NULL);
    PhyControlInfo *ctrl = new PhyControlInfo();
    ctrl->setBitrate(basicBitrate);
    EV << "basicbitrate=" << basicBitrate << endl;
    frame->setControlInfo(ctrl);
    return frame;
}

/****************************************************************
 * Helper functions.
 */

void BasicIeee80211Mac::updateStatsAtEndOfBackoff()
{
	ASSERT(classSupported(currentAC));
	if (simTime().dbl() < 2.0 || cycleBegin < 0)
		return; // The first time we don't know the begin

	double last = max(wiClasses[currentAC].endBackoff->getSendingTime().dbl(), (cycleBegin - getDIFS(currentAC).dbl()));

	double dur = (simTime() - last).dbl();

	EV << " durrrrrrrr update " << dur << " last = " << last << endl;

	dur = (double)((int)(dur * 10000000)) / 10000000.0; // Round

	EV << "updateStatsAtEndOfBackoff dur = " << dur << " cycle begin = " << cycleBegin << " endBackoff->getSendingTime().dbl() = " << wiClasses[currentAC].endBackoff->getSendingTime().dbl() << endl;

	ASSERT(dur >= 0); // Can be 0


	FiWiGeneralConfigs::StatsTrafficClass[currentAC].sumContentionResDuration += dur;

	FiWiGeneralConfigs::wiZoneStats[zoneId()].sumContentionResDuration += dur;
}

void BasicIeee80211Mac::addCycleTransmissionAttempt(int ac)
{
	ASSERT(classSupported(ac));
	FiWiGeneralConfigs::StatsStationTraffic[ac][this->address].nbCycleTransmissionAttempts += 1;
	EV << "ADDING ATTEMPT!!" << endl;
}

void BasicIeee80211Mac::addCycleTransmissionSuccess()
{
	FiWiGeneralConfigs::StatsStationTraffic[currentAC][this->address].nbCycleTransmissionSuccesses += 1;
}

int BasicIeee80211Mac::zoneId()
{
	int z = MyUtil::getRoutingTable(this)->zoneOf(this->address);

	ASSERT(z >= 0);

	EV << "hola, my zone is " << z << endl;

	return z;
}

void BasicIeee80211Mac::beginNewCycle()
{
	if (simTime().dbl() < 2.0)
	{
		return;
	}

	if (cycleBegin <= 0)
	{
		// Record current time
		cycleBegin = simTime().dbl();

		// Keep nb cycles
		FiWiGeneralConfigs::wiZoneStats[zoneId()].wiNbCycles += 1;
	}

	// Then, add attemps to all stations in this zone having a packet waiting
	for (int i = 0; i < (int)MyUtil::getRoutingTable(this)->wiMacZones[zoneId()].size(); ++i)
	{
		// Is there a packet waiting at mac i ?
		for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
		{
			int ac = it->second.id;

			if ( ! MyUtil::getRoutingTable(this)->wiMacZones[zoneId()][i]->transmissionQueueEmpty(ac) &&
					! MyUtil::getRoutingTable(this)->wiMacZones[zoneId()][i]->wiClasses[ac].addedAttemptInCycle)
			{
				// If so, add an attempt
				MyUtil::getRoutingTable(this)->wiMacZones[zoneId()][i]->addCycleTransmissionAttempt(ac);
				MyUtil::getRoutingTable(this)->wiMacZones[zoneId()][i]->wiClasses[ac].addedAttemptInCycle = true;

				EV << "ADDING supercycle attemp for mac " << MyUtil::getRoutingTable(this)->wiMacZones[zoneId()][i]->address.str() << " to ac " << ac << endl;
			}
		}
	}
}

void BasicIeee80211Mac::resetCycleBegin()
{
	// Update the mean cycle len
	if (cycleBegin > 0)
	{
		double cycleDur = simTime().dbl() - cycleBegin;

		if (cycleDur < durationRTSCTS())
		{
			return; // skip... this is due to collisions
		}

		EV << "RESET SUPERCYCLE was.. cycleBegin = " << cycleBegin << endl;
		EV << "LAST SUPERCYCLE duration = " << cycleDur << endl;

		if (simTime().dbl() >= 2.0)
		{
			FiWiGeneralConfigs::wiZoneStats[zoneId()].sumCycleLengths += cycleDur;
			FiWiGeneralConfigs::wiZoneStats[zoneId()].nbCycleLengths += 1;
		}
	}

	cycleBegin = -1;

	for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
	{
		for (int i = 0; i < (int)MyUtil::getRoutingTable(this)->wiMacZones[zoneId()].size(); ++i)
		{
			MyUtil::getRoutingTable(this)->wiMacZones[zoneId()][i]->wiClasses[it->second.id].addedAttemptInCycle = false;
		}
	}

	// reset cycle begin
	if ( ! transmissionQueueEmpty())
		beginNewCycle();

	EV << "CUR SUPERCYCLE BEGIN = " << cycleBegin << endl;
}

void BasicIeee80211Mac::finishCurrentTransmission(Ieee80211Frame* frame)
{
    popTransmissionQueue(frame, currentAC);
    //resetAllStateVariables();
    resetStateVariables(currentAC);
    wiClasses[currentAC].backoff = true;

    resetCycleBegin();

    if (simTime().dbl() >= 2.0)
    {
		addCycleTransmissionSuccess();
		EV << "ADDING SUCC to " << this->address << endl;
    }
}

void BasicIeee80211Mac::giveUpCurrentTransmission(int ac)
{
	ASSERT(classSupported(ac));

    popTransmissionQueue(NULL, ac);
    resetStateVariables(ac);
    wiClasses[ac].backoff = true;
    wiClasses[ac].backoffPeriod = -1;

    // resetCycleBegin();
}

void BasicIeee80211Mac::retryAllACsCurrentTransmission()
{
	for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
	{
		if (it->second.retryCounter < it->second.retryLimit - 1)
		{
			retryCurrentTransmission(it->second.id);
		}
		else
		{
			giveUpCurrentTransmission(it->second.id);
		}
	}

	resetCycleBegin();
}

bool BasicIeee80211Mac::allACsRetryLimitReached()
{
	for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
	{
		if (it->second.retryCounter < it->second.retryLimit - 1)
		{
			return false;
		}
	}

	return true;
}

void BasicIeee80211Mac::retryCurrentTransmission(int ac)
{
	ASSERT(classSupported(ac));

	if ( ! wiClasses[ac].transmissionQueue.empty())
		getCurrentTransmission(ac)->setRetry(true);

    wiClasses[ac].retryCounter++;
    numRetry++;
    wiClasses[ac].backoff = true;
    wiClasses[ac].backoffPeriod = -1;
}

Ieee80211DataOrMgmtFrame *BasicIeee80211Mac::getCurrentTransmission(int ac)
{
	ASSERT(classSupported(ac));

    if (wiClasses[ac].transmissionQueue.empty())
        return NULL;

    return (Ieee80211DataOrMgmtFrame *)wiClasses[ac].transmissionQueue.front();
}

void BasicIeee80211Mac::sendDownPendingRadioConfigMsg()
{
    if (pendingRadioConfigMsg != NULL)
    {
        sendDown(pendingRadioConfigMsg);
        pendingRadioConfigMsg = NULL;
    }
}

void BasicIeee80211Mac::setMode(Mode mode)
{
    if (mode == PCF)
        error("PCF mode not yet supported");

    this->mode = mode;
}

void BasicIeee80211Mac::resetAllStateVariables()
{
	for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
	{
		resetStateVariables(it->second.id);
	}
}

void BasicIeee80211Mac::resetStateVariables(int ac)
{
	EV << "currentAC = " << ac << endl;

	ASSERT(classSupported(ac));

    wiClasses[ac].backoffPeriod = -1;
    wiClasses[ac].retryCounter = 0;
    wiClasses[ac].backoff = false;
}

void BasicIeee80211Mac::resetAllBackOff()
{
	for (std::map<int, WiTrafClass>::iterator it = wiClasses.begin(); it != wiClasses.end(); ++it)
	{
		it->second.backoff = true;
		it->second.backoffPeriod - 1;
	}
}

bool BasicIeee80211Mac::isMediumStateChange(cMessage *msg)
{
    return msg == mediumStateChange || (msg == endReserve && radioState == RadioState::IDLE);
}

bool BasicIeee80211Mac::isMediumFree()
{
    return radioState == RadioState::IDLE && !endReserve->isScheduled();
}

bool BasicIeee80211Mac::isForUs(Ieee80211Frame *frame)
{
	Ieee80211TwoAddressFrame* tmpFrame = dynamic_cast<Ieee80211TwoAddressFrame*>(frame);

	EV << "BasicIeee80211Mac::isForUs 1" << endl;

	if (frame && frame->getReceiverAddress() == address)
	{
		EV << "BasicIeee80211Mac::isForUs 2" << endl;
		return true;
	}

	if (tmpFrame)
	{
		EV << "BasicIeee80211Mac::isForUs 3" << endl;
		pair<bool, FiWiNode> res = MyUtil::getRoutingTable(this)->nextHop(tmpFrame->getTransmitterAddress(), frame->getReceiverAddress(), frame->getFiwiType());

		MyUtil::getRoutingTable(this)->print();

		EV << "BasicIeee80211Mac::isForUs 4 next hop of " << tmpFrame->getTransmitterAddress() << " to " << frame->getReceiverAddress() << " of " << frame->getFiwiType() << endl;

		if (res.first)
		{
			EV << "BasicIeee80211Mac::isForUs 5" << endl;
			// EV << "OK -> next hop = " << res.second.toString() << endl;
			return res.second.addr == address;
		}
	}

    return false;
}

bool BasicIeee80211Mac::isDataOrMgmtFrame(Ieee80211Frame *frame)
{
    return dynamic_cast<Ieee80211DataOrMgmtFrame*>(frame);
}

Ieee80211Frame *BasicIeee80211Mac::getFrameReceivedBeforeSIFS()
{
    return (Ieee80211Frame *)endSIFS->getContextPointer();
}





void BasicIeee80211Mac::popTransmissionQueue(Ieee80211Frame* frame, int ac)
{
	ASSERT(classSupported(ac));

	if ( ! wiClasses[ac].transmissionQueue.empty())
	{
		EV << "dropping frame from transmission queue\n";
		Ieee80211Frame *temp = wiClasses[ac].transmissionQueue.front();

		Ieee80211ACKFrame* ackFrame = dynamic_cast<Ieee80211ACKFrame*>(frame);
		ListEtherFrames* listFrames = dynamic_cast<ListEtherFrames*>(temp->getEncapsulatedPacket());

		EV << "woooo ackframe = " << ackFrame << " , list frames = " << listFrames << " ackFrame->getPacketsNotReceivedArraySize() =  " << ackFrame->getPacketsNotReceivedArraySize() << endl;

		if (ackFrame && listFrames && ackFrame->getPacketsNotReceivedArraySize() > 0)
		{
			EV << "  packet size = " << temp->getBitLength() << endl;

			temp->setKind(BITERROR);

			for (int i = 0; i < (int)ackFrame->getPacketsNotReceivedArraySize(); ++i)
			{
				int indexNotRcv = ackFrame->getPacketsNotReceived(i);
				EV << "blub index not rcv = " << indexNotRcv << endl;
				listFrames->getFrames(indexNotRcv).setKind(BITERROR);
			}

			wiClasses[ac].transmissionQueue.pop_front();

			sendUp(temp);
		}
		else
		{
			wiClasses[ac].transmissionQueue.pop_front();
			delete temp;
		}
	}

    requestPacket();
}

double BasicIeee80211Mac::computeFrameDuration(Ieee80211Frame *msg)
{
    return computeFrameDuration(msg->getBitLength(), bitrate);
}

double BasicIeee80211Mac::computeFrameDuration(int bits, double bitrate)
{
    return bits / bitrate;
}

void BasicIeee80211Mac::logState()
{

    EV  << "state information: state = " << fsm.getStateName()
    << ", radioState = " << radioState
    << ", nav = " << nav << endl;
}

void BasicIeee80211Mac::finish()
{
}

