//
// Copyright (C) 2006 Andras Varga and Levente Meszaros
// Copyright (C) 2009 Lukáš Hlůže   lukas@hluze.cz (802.11e)
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

#ifndef IEEE_80211eg_MAC_H
#define IEEE_80211eg_MAC_H

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
#include "TCPSegment.h"
#include "IPDatagram.h"
#include "ICMPMessage.h"
#include "UDPPacket.h"

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
class INET_API Ieee80211egMac : public WirelessMacBase, public INotifiable
{
    typedef std::list<Ieee80211DataOrMgmtFrame*> Ieee80211DataOrMgmtFrameList;

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

  protected:
    /**
     * @name Configuration parameters
     * These are filled in during the initialization phase and not supposed to change afterwards.
     */
    //@{
    /** MAC address */
    MACAddress address;
    char opMode;
    /** The bitrate is used to send data and mgmt frames; be sure to use a valid 802.11 bitrate */
    double bitrate;

    /** The basic bitrate (1 or 2 Mbps) is used to transmit control frames */
    double basicBitrate;

    /** Maximum number of frames in the queue; should be set in the omnetpp.ini */
    int maxQueueSize;

    /**
     * The minimum length of MPDU to use RTS/CTS mechanism. 0 means always, extremely
     * large value means never. See spec 9.2.6 and 361.
     */
    int rtsThreshold;

    /**
     * Maximum number of transmissions for a message.
     * This includes the initial transmission and all subsequent retransmissions.
     * Thus a value 0 is invalid and a value 1 means no retransmissions.
     * See: dot11ShortRetryLimit on page 484.
     *   'This attribute shall indicate the maximum number of
     *    transmission attempts of a frame, the length of which is less
     *    than or equal to dot11RTSThreshold, that shall be made before a
     *    failure condition is indicated. The default value of this
     *    attribute shall be 7'
     */
    int transmissionLimit;

    /**
     * Arbitration interframe space number.
     * The duration AIFS[AC] is a duration derived from the value AIFSN[AC] by the relation
     * AIFS[AC] = AIFSN[AC] × aSlotTime + aSIFSTime.
     * See spec 7.3.2.29
     *
     */
    int AIFSN[4];
    /** Default access catagory */
    int defaultAC;

    /** Slot time 9us(fast slot time 802.11g only) 20us(802.11b / 802.11g backward compatible)*/
    simtime_t ST;

    double PHY_HEADER_LENGTH;
    /** Minimum contention window. */
    int cwMinData;
    int cwMin[4];

    /** Maximum contention window. */
    int cwMaxData;
    int cwMax[4];

    /** Contention window size for broadcast messages. */
    int cwMinBroadcast;

    /** Messages longer than this threshold will be sent in multiple fragments. see spec 361 */
    static const int fragmentationThreshold = 2346;
    //@}

  public:
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
        WAITAIFS,
        BACKOFF,
        WAITACK,
        WAITBROADCAST,
        WAITCTS,
        WAITSIFS,
        RECEIVE,
    };
  protected:
    cFSM fsm;
    bool fixFSM;
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

    /**
     * Indicates that the last frame received had bit errors in it or there was a
     * collision during receiving the frame. If this flag is set, then the MAC
     * will wait EIFS - DIFS + AIFS  instead of AIFS period of time in WAITAIFS state.
     */
    bool lastReceiveFailed;

    /** True if backoff is enabled */
    bool backoff[4];

    /** True during network allocation period. This flag is present to be able to watch this state. */
    bool nav;

    /** True if we are in txop bursting packets. */
    bool txop;

    /** TXOP parametr */
    simtime_t TXOP[4];

    /** Indicates which queue is acite. Depends on access category. */
    int currentAC;

    /** Remember currentAC. We need this to figure out internal colision. */
    int oldcurrentAC;

    /** XXX Remember for which AC we wait for ACK. */
    //int ACKcurrentAC;

    /** Remaining backoff period in seconds */
    simtime_t backoffPeriod[4];

    /**
     * Number of frame retransmission attempts, this is a simpification of
     * SLRC and SSRC, see 9.2.4 in the spec
     */
    int retryCounter[4];

    /** Physical radio (medium) state copied from physical layer */
    RadioState::State radioState;
    // Use to distinguish the radio module that send the event
    int radioModule;

    int getRadioModuleId() {return radioModule;}

    /** Messages received from upper layer and to be transmitted later */
    Ieee80211DataOrMgmtFrameList transmissionQueue[4];

    Ieee80211DataOrMgmtFrame *fr;

    /**
    * A list of last sender, sequence and fragment number tuples to identify
    * duplicates, see spec 9.2.9.
    * TODO: this is not yet used
    */
    Ieee80211ASFTupleList asfTuplesList;

    /** Passive queue module to request messages from */
    IPassiveQueue *queueModule;

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

    /** End of the Arbitration Inter-Frame Time period */
    cMessage *endAIFS[4];

    /** End of the TXOP time limit period */
    cMessage *endTXOP;

    /** End of the backoff period */
    cMessage *endBackoff[4];

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
    long numRetry[4];
    long numSentWithoutRetry[4];
    long numGivenUp[4];
    long numCollision;
    long numInternalCollision;
    long numSent[4];
    long numBites;
    long numSentTXOP;
    long numReceived;
    long numSentBroadcast;
    long numReceivedBroadcast;
    long numDropped[4];
    long numReceivedOther;
    long numAckSend;
    cOutVector stateVector;
    simtime_t  last;
    long bites[4];
    simtime_t minjitter[4];
    simtime_t maxjitter[4];
    cOutVector jitter[4];
    cOutVector macDelay[4];
    cOutVector radioStateVector;
    cOutVector throughput[4];
    //@}

  public:
    /**
     * @name Construction functions
     */
    //@{
    Ieee80211egMac();
    virtual ~Ieee80211egMac();
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
    virtual simtime_t getDIFS(int category=-1);
    virtual simtime_t getAIFS(int AccessCategory);
    virtual simtime_t getEIFS();
    virtual simtime_t getPIFS();
    virtual simtime_t computeBackoffPeriod(Ieee80211Frame *msg, int r);
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

    virtual void scheduleAIFSPeriod();
    virtual void rescheduleAIFSPeriod(int AccessCategory);
    virtual void cancelAIFSPeriod();
//XXX    virtual void checkInternalColision();

    virtual void scheduleDataTimeoutPeriod(Ieee80211DataOrMgmtFrame *frame);
    virtual void scheduleBroadcastTimeoutPeriod(Ieee80211DataOrMgmtFrame *frame);
    virtual void cancelTimeoutPeriod();

    virtual void scheduleCTSTimeoutPeriod();

    /** @brief Schedule network allocation period according to 9.2.5.4. */
    virtual void scheduleReservePeriod(Ieee80211Frame *frame);

    /** @brief Generates a new backoff period based on the contention window. */
    virtual void invalidateBackoffPeriod();
    virtual bool isInvalidBackoffPeriod();
    virtual void generateBackoffPeriod();
    virtual void decreaseBackoffPeriod();
    virtual void scheduleBackoffPeriod();
    virtual void cancelBackoffPeriod();
    virtual void finishReception();
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
    virtual void sendBroadcastFrame(Ieee80211DataOrMgmtFrame *frameToSend);
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
    virtual Ieee80211DataOrMgmtFrame *buildBroadcastFrame(Ieee80211DataOrMgmtFrame *frameToSend);
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
    virtual void finishCurrentTransmission();
    virtual void giveUpCurrentTransmission();
    virtual void retryCurrentTransmission();
    virtual bool transmissionQueueEmpty();

    /** @brief Mapping to access categories. */
    virtual int MappingAccessCategory(Ieee80211DataOrMgmtFrame *frame);

    /** @brief Send down the change channel message to the physical layer if there is any. */
    virtual void sendDownPendingRadioConfigMsg();

    /** @brief Change the current MAC operation mode. */
    virtual void setMode(Mode mode);

    /** @brief Returns the current frame being transmitted */
    virtual Ieee80211DataOrMgmtFrame *getCurrentTransmission();

    /** @brief Reset backoff, backoffPeriod and retryCounter for IDLE state */
    virtual void resetStateVariables();

    /** @brief Used by the state machine to identify medium state change events.
        This message is currently optimized away and not sent through the kernel. */
    virtual bool isMediumStateChange(cMessage *msg);

    /** @brief Tells if the medium is free according to the physical and virtual carrier sense algorithm. */
    virtual bool isMediumFree();

    /** @brief Returns true if message is a broadcast message */
    virtual bool isBroadcast(Ieee80211Frame *msg);

    /** @brief Returns true if message destination address is ours */
    virtual bool isForUs(Ieee80211Frame *msg);

    /** @brief Returns true if message source address is ours */
    virtual bool isSentByUs(Ieee80211Frame *msg);

    /** @brief Checks if the frame is a data or management frame */
    virtual bool isDataOrMgmtFrame(Ieee80211Frame *frame);

    /** @brief Checks if the message is endAIFS and set currentAC */
    virtual bool isMsgAIFS(cMessage *msg);

    /** @brief Returns the last frame received before the SIFS period. */
    virtual Ieee80211Frame *getFrameReceivedBeforeSIFS();

    /** @brief Deletes frame at the front of queue. */
    virtual void popTransmissionQueue();

    /**
     * @brief Computes the duration (in seconds) of the transmission of a frame
     * over the physical channel. 'bits' should be the total length of the MAC frame
     * in bits, but excluding the physical layer framing (preamble etc.)
     */
    virtual double computeFrameDuration(Ieee80211Frame *msg);
    virtual double computeFrameDuration(int bits, double bitrate);

    /** @brief Logs all state information */
    virtual void logState();

    /** @brief Produce a readable name of the given MAC operation mode */
    const char *modeName(int mode);
    virtual void resetAllBackOff()
    {
    	backoff[0]=backoff[1]=backoff[2]=backoff[3]=true;
    	backoffPeriod[0] = backoffPeriod[1]=backoffPeriod[2]=backoffPeriod[3]=-1;
    }

    virtual void resetCurrentBackOff()
    {
    	backoff[currentAC]=true;
    	backoffPeriod[currentAC] =-1;
    }
    //@}
};

#endif

