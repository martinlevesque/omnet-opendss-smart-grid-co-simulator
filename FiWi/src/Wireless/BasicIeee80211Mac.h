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

#ifndef BASIC_IEEE_80211_MAC_H
#define BASIC_IEEE_80211_MAC_H

// uncomment this if you do not want to log state machine transitions
#define FSM_DEBUG

#include <list>
#include "WirelessMacBase.h"
#include "IPassiveQueue.h"
#include "Ieee80211Frame_m.h"
#include "Ieee80211Consts.h"
#include "NotificationBoard.h"
#include "RadioState.h"
#include "FSMA.h"
#include "IBasic80211Mac.h"
#include <vector>
#include "BasicIeee80211MgmtAP.h"
#include "RawStat.h"
#include <map>

#define IEEE80211MAC_REQUEST_FOR_PACKET 101

struct WiTrafClass
{
	typedef std::list<Ieee80211DataOrMgmtFrame*> Ieee80211DataOrMgmtFrameList;

	int id;
	int backoffCWmin;
	int backoffCWmax;
	int delta;
	int retryLimit;
	int retryCounter;
	bool backoff;
	bool addedAttemptInCycle;

	cMessage* endBackoff;
	cMessage* endPostBackoff;
	cMessage* endAIFS;

	/** Messages received from upper layer and to be transmitted later */
	Ieee80211DataOrMgmtFrameList transmissionQueue;

	/** Remaining backoff period in seconds */
	simtime_t backoffPeriod;
};

/**
 * IEEE 802.11b Media Access Control Layer.
 *
 * Various comments in the code refer to the Wireless LAN Medium Access
 * Control (MAC) and Physical Layer(PHY) Specifications
 * ANSI/IEEE Std 802.11, 1999 Edition (R2003)
 *
 * For more info, see the NED file.
 *
 * TODO: support fragmentation
 * TODO: PCF mode
 * TODO: CF period
 * TODO: pass radio power to upper layer
 * TODO: transmission complete notification to upper layer
 * TODO: STA TCF timer syncronization, see Chapter 11 pp 123
 *
 * Parts of the implementation have been taken over from the
 * Mobility Framework's Mac80211 module.
 *
 * @ingroup macLayer
 */
class INET_API BasicIeee80211Mac : public WirelessMacBase, public INotifiable, public IBasic80211Mac
{


    static RawStat MACFrameLogs;
    static double cycleBegin; // this is used in analytical models... contention resolution followed by transmission OR collision.

    /**
     * This is used to populate fragments and identify duplicated messages. See spec 9.2.9.
     */
    struct Ieee80211ASFTuple
    {
        MACAddress address;
        int sequenceNumber;
        int fragmentNumber;
    };

    typedef std::list<Ieee80211ASFTuple*> Ieee80211ASFTupleList;

    std::map<int, WiTrafClass> wiClasses;

  protected:


    int currentAC;

    int zoneId();
    bool MappingAccessCategory(Ieee80211DataOrMgmtFrame *frame);
    bool classSupported(int ac);
    bool transmissionQueueEmpty();
    bool transmissionQueueEmpty(int ac);

    BasicIeee80211MgmtAP* managementModule;

    /**
     * @name Configuration parameters
     * These are filled in during the initialization phase and not supposed to change afterwards.
     */
    //@{
    /** MAC address */
    MACAddress address;

    /** The bitrate is used to send data and mgmt frames; be sure to use a valid 802.11 bitrate */
    double bitrate;

    double phyHeaderDuration;

    /** The basic bitrate (1 or 2 Mbps) is used to transmit control frames */
    double basicBitrate;

    /** Maximum number of frames in the queue; should be set in the omnetpp.ini */
    int maxQueueSize;

    /**
     * The minimum length of MPDU to use RTS/CTS mechanism. 0 means always, extremely
     * large value means never. See spec 9.2.6 and 361.
     */
    int rtsThreshold;

    /** Messages longer than this threshold will be sent in multiple fragments. see spec 361 */
    static const int fragmentationThreshold = 2346;
    //@}

  public:

    virtual MACAddress getMACAddress() { return address; }
    void addCycleTransmissionAttempt(int ac);
    void addCycleTransmissionSuccess();

    void beginNewCycle();

    /**
     * @name Ieee80211Mac state variables
     * Various state information checked and modified according to the state machine.
     */
    //@{
    // don't forget to keep synchronized the C++ enum and the runtime enum definition
    /** the 80211 MAC state machine */
    enum State
    {
        IDLE,
        DEFER,
        WAITDIFS,
        BACKOFF,
        WAITACK,
        WAITCTS,
        WAITSIFS,
        RECEIVE,
        POSTBACKOFF,
    };
  protected:
    cFSM fsm;

  public:
    /** 80211 MAC operation modes */
    enum Mode
    {
        DCF,  ///< Distributed Coordination Function
        PCF,  ///< Point Coordination Function
    };
  protected:
    Mode mode;

    /** Sequence number to be assigned to the next frame */
    int sequenceNumber;

    bool lastReceiveFailed;

    /** True during network allocation period. This flag is present to be able to watch this state. */
    bool nav;



    /** Physical radio (medium) state copied from physical layer */
    RadioState::State radioState;
    // Use to distinguish the radio module that send the event
    int radioModule;

    int getRadioModuleId() {return radioModule;}




    /**
     * A list of last sender, sequence and fragment number tuples to identify
     * duplicates, see spec 9.2.9.
     * TODO: this is not yet used
     */
    Ieee80211ASFTupleList asfTuplesList;

    /**
     * The last change channel message received and not yet sent to the physical layer, or NULL.
     * The message will be sent down when the state goes to IDLE or DEFER next time.
     */
    cMessage *pendingRadioConfigMsg;
    //@}

  protected:
    /** @name Timer messages */
    //@{
    /** End of the Short Inter-Frame Time period */
    cMessage *endSIFS;

    /** End of the Data Inter-Frame Time period */
    cMessage *endDIFS;

    /** Timeout after the transmission of an RTS, a CTS, or a DATA frame */
    cMessage *endTimeout;

    /** End of medium reserve period (NAV) when two other nodes were communicating on the channel */
    cMessage *endReserve;

    /** Radio state change self message. Currently this is optimized away and sent directly */
    cMessage *mediumStateChange;
    //@}

  protected:
    /** @name Statistics */
    //@{
    long numRetry;
    long numSentWithoutRetry;
    long numGivenUp;
    long numCollision;
    long numSent;
    long numReceived;
    long numAckSend;
    long numReceivedOther;

    long numRTS;
    long numCTSTimeout;
    long numDataSent;
    long numACKTimeout;
    long numBackoff;
    double sumBackoffs;
    long numCancelBackoff;
    long numDIFSBegin;
    long numDIFSEnd;

    double propagationDelay;

    //@}

  public:
    /**
     * @name Construction functions
     */
    //@{
    BasicIeee80211Mac();
    virtual ~BasicIeee80211Mac();
    //@}

  protected:
    /**
     * @name Initialization functions
     */
    //@{
    /** @brief Initialization of the module and its variables */
    virtual int numInitStages() const {return 2;}
    virtual void initialize(int);
    virtual void registerInterface();
    virtual void initializeQueueModule();
    virtual void finish();

    double durationRTSCTS();

    double estimatedPropagationDelay();

    void resetAllBackOff();

    //@}

  protected:
    /**
     * @name Message handing functions
     * @brief Functions called from other classes to notify about state changes and to handle messages.
     */
    //@{
    /** @brief Called by the NotificationBoard whenever a change occurs we're interested in */
    virtual void receiveChangeNotification(int category, const cPolymorphic * details);

    /** @brief Handle commands (msg kind+control info) coming from upper layers */
    virtual void handleCommand(cMessage *msg);

    /** @brief Handle timer self messages */
    virtual void handleSelfMsg(cMessage *msg);

    /** @brief Handle messages from upper layer */
    virtual void handleUpperMsg(cPacket *msg);

    /** @brief Handle messages from lower (physical) layer */
    virtual void handleLowerMsg(cPacket *msg);

    /** @brief Handle all kinds of messages and notifications with the state machine */
    virtual void handleWithFSM(cMessage *msg);
    //@}

  protected:
    /**
     * @name Timing functions
     * @brief Calculate various timings based on transmission rate and physical layer charactersitics.
     */
    //@{
    virtual simtime_t getSIFS();
    virtual simtime_t getSlotTime();
    virtual simtime_t getDIFS(int ac);
    bool isMsgAIFS(cMessage *msg);
    int ACOfAIFSMsg(cMessage *msg);
    virtual simtime_t getPIFS();
    virtual simtime_t computeBackoffPeriod(Ieee80211Frame *msg, int r, int ac, bool inPostBackoff);

    bool entirelyInError(Ieee80211Frame* frame);

    //@}

  protected:
    /**
     * @name Timer functions
     * @brief These functions have the side effect of starting the corresponding timers.
     */
    //@{
    virtual void scheduleSIFSPeriod(Ieee80211Frame *frame);

    virtual void scheduleDIFSPeriod();
    virtual void cancelDIFSPeriod();

    virtual void scheduleDataTimeoutPeriod(Ieee80211DataOrMgmtFrame *frame);
    virtual void cancelTimeoutPeriod();

    virtual void scheduleCTSTimeoutPeriod();

    /** @brief Schedule network allocation period according to 9.2.5.4. */
    virtual void scheduleReservePeriod(Ieee80211Frame *frame, simtime_t duration, bool forUs);

    /** @brief Generates a new backoff period based on the contention window. */
    virtual void invalidateBackoffPeriod(int ac);
    virtual bool isInvalidBackoffPeriod(int ac);
    virtual void generateBackoffPeriod(int ac, bool inPostBackoff);
    void invalidateAllBackoffPeriods();
    void toBackoff();
    virtual void decreaseBackoffPeriod();
    virtual void scheduleBackoffPeriod();
    virtual void cancelBackoffPeriod();

    void schedulePostBackoffPeriods();
    bool isEndPostBackoffMsg(cMessage* msg);
    bool allPostBackoffsFinished();

    void terminatePostBackoffPeriods();
    int ACOfEndPostBackoff(cMessage* m);

    void terminateBackoffPeriod();
    //@}

  protected:
    /**
     * @name Frame transmission functions
     */
    //@{
    virtual void sendACKFrameOnEndSIFS();
    virtual void sendACKFrame(Ieee80211DataOrMgmtFrame *frame);
    virtual void sendRTSFrame(Ieee80211DataOrMgmtFrame *frameToSend);
    virtual void sendCTSFrameOnEndSIFS();
    virtual void sendCTSFrame(Ieee80211RTSFrame *rtsFrame);
    virtual void sendDataFrameOnEndSIFS(Ieee80211DataOrMgmtFrame *frameToSend);
    virtual void sendDataFrame(Ieee80211DataOrMgmtFrame *frameToSend);
    //@}

  protected:
    /**
     * @name Frame builder functions
     */
    //@{
    virtual Ieee80211DataOrMgmtFrame *buildDataFrame(Ieee80211DataOrMgmtFrame *frameToSend);
    virtual Ieee80211ACKFrame *buildACKFrame(Ieee80211DataOrMgmtFrame *frameToACK);
    virtual Ieee80211RTSFrame *buildRTSFrame(Ieee80211DataOrMgmtFrame *frameToSend);
    virtual Ieee80211CTSFrame *buildCTSFrame(Ieee80211RTSFrame *rtsFrame);

    //@}

    /**
     * @brief Attaches a PhyControlInfo to the frame which will cause it to be sent at
     * basicBitrate not bitrate (e.g. 2Mbps instead of 11Mbps). Used with ACK, CTS, RTS.
     */
    virtual Ieee80211Frame *setBasicBitrate(Ieee80211Frame *frame);

  protected:
    /**
     * @name Utility functions
     */
    //@{

    void resetCycleBegin();

    virtual void finishCurrentTransmission(Ieee80211Frame* frame);
    virtual void giveUpCurrentTransmission(int ac);
    virtual void retryCurrentTransmission(int ac);
    void retryAllACsCurrentTransmission();
    bool allACsRetryLimitReached();

    void updateStatsAtEndOfBackoff();

    /** @brief Send down the change channel message to the physical layer if there is any. */
    virtual void sendDownPendingRadioConfigMsg();

    /** @brief Change the current MAC operation mode. */
    virtual void setMode(Mode mode);

    /** @brief Returns the current frame being transmitted */
    virtual Ieee80211DataOrMgmtFrame *getCurrentTransmission(int ac);

    /** @brief Reset backoff, backoffPeriod and retryCounter for IDLE state */
    virtual void resetStateVariables(int ac);

    void resetAllStateVariables();

    /** @brief Used by the state machine to identify medium state change events.
        This message is currently optimized away and not sent through the kernel. */
    virtual bool isMediumStateChange(cMessage *msg);

    /** @brief Tells if the medium is free according to the physical and virtual carrier sense algorithm. */
    virtual bool isMediumFree();

    /** @brief Returns true if message destination address is ours */
    virtual bool isForUs(Ieee80211Frame *msg);

    /** @brief Checks if the frame is a data or management frame */
    virtual bool isDataOrMgmtFrame(Ieee80211Frame *frame);

    /** @brief Returns the last frame received before the SIFS period. */
    virtual Ieee80211Frame *getFrameReceivedBeforeSIFS();

    /** @brief Deletes frame at the front of queue. */
    virtual void popTransmissionQueue(Ieee80211Frame* frame, int ac);

    /**
     * @brief Computes the duration (in seconds) of the transmission of a frame
     * over the physical channel. 'bits' should be the total length of the MAC frame
     * in bits, but excluding the physical layer framing (preamble etc.)
     */
    virtual double computeFrameDuration(Ieee80211Frame *msg);
    virtual double computeFrameDuration(int bits, double bitrate);

    /** @brief Logs all state information */
    virtual void logState();

    int sizeOfQueueInBytes(const std::vector<EtherFrame>& frames);

    void requestPacket();

    //@}
};

#endif

