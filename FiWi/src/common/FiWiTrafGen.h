/*
 * Copyright (C) 2003 Andras Varga; CTIE, Monash University, Australia
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __FIWI_TRAF_GEN_H
#define __FIWI_TRAF_GEN_H

#include "INETDefs.h"
#include "MACAddress.h"
#include "FiWiTrafficMatrix.h"
#include "Ieee80211Frame_m.h"
#include "EtherFrame_m.h"
#include "EtherApp_m.h"
#include <string>
#include "PEVMessages_m.h"
#include "RawStat.h"
#include <set>
#include <vector>
#include "DMS.h"

#define GENERAL_CONF_NAME_MSG_BASE_LOAD_UPDATE "general_conf_name_base_load_update"

#define FIWI_TRAF_PKT_TYPE_BEST_EFFORT 0
#define FIWI_TRAF_PKT_TYPE_VIDEO 1
#define FIWI_TRAF_PKT_TYPE_VOICE 2
#define FIWI_TRAF_PKT_TYPE_SMART_GRID_CONTROL 3
#define FIWI_TRAF_PKT_TYPE_SMART_GRID_NOTIFICATION 4

struct RowBaseLoad
{
	double time;
	std::string powerNodeId;
	double load;
};



struct BestEffortConfiguration
{
	double from;
	double to;
	double bps;
};

struct VideoConfiguration
{
	cMessage* nextMessage;
	cMessage* endSession;
};

struct VoiceConfiguration
{
	cMessage* nextMessage;
	cMessage* endSession;
};

struct DOSConfiguration
{
	cMessage* nextMessageSmartGridControl;
	cMessage* nextMessageSmartGridNotification;
};

/**
 *
 */
class FiWiTrafGen : public cSimpleModule
{
  public:
	static MACAddress dmsAddress;
	static int totalNbNodes;
	static int curIndexChangeLoad;
	static std::set<std::string> listNodes;

	static RawStat VideoStreamsDropRatio;
	static RawStat BestEffortDropRatio;
	static RawStat statsNbPEVsCharging;

	static RawStat statsSOC;

	static int HybridNbSecondsPerHour;

  protected:

	int nbTriesSched;

	std::vector<VideoConfiguration> videoConfs;
	std::vector<VoiceConfiguration> voiceConfs;
	std::vector<BestEffortConfiguration> bestEffortConfs;
	cMessage* nextBestEffortMsg;
	DOSConfiguration dosConfiguration;

	double PEVArrivalTime;
	double PEVPenetrationLevel;
	bool withChargingStations;

	double SLMMaxSlotLengthInSeconds;

	MACAddress myAddress;

	// Upstream
	RawStat DMSThroughput;
	RawStat DMSDelay;

	RawStat DMSThroughputVideo;
	RawStat DMSDelayVideo;

	RawStat DMSThroughputVoice;
	RawStat DMSDelayVoice;

	RawStat DMSThroughputBestEffort;
	RawStat DMSDelayBestEffort;

	RawStat DMSThroughputSmartGridControl;
	RawStat DMSDelaySmartGridControl;

	RawStat DMSThroughputSmartGridControlDOS;
	RawStat DMSDelaySmartGridControlDOS;

	RawStat DMSThroughputSmartGridNotification;
	RawStat DMSDelaySmartGridNotification;

	RawStat DMSThroughputSmartGridNotificationDOS;
	RawStat DMSDelaySmartGridNotificationDOS;

	// Downstream
	RawStat DownstreamThroughput;
	RawStat DownstreamDelay;

	RawStat DownstreamThroughputVideo;
	RawStat DownstreamDelayVideo;

	RawStat DownstreamThroughputVoice;
	RawStat DownstreamDelayVoice;

	RawStat DownstreamThroughputBestEffort;
	RawStat DownstreamDelayBestEffort;

	RawStat DownstreamThroughputSmartGridControl;
	RawStat DownstreamDelaySmartGridControl;

	RawStat DownstreamThroughputSmartGridNotification;
	RawStat DownstreamDelaySmartGridNotification;

	void updateReceiveStats(EtherFrame* frame, cPacket* pkt, RawStat& throughput, RawStat& delay, RawStat& throughputVideo, RawStat& delayVideo,
			RawStat& throughputVoice, RawStat& delayVoice, RawStat& throughputBestEffort, RawStat& delayBestEffort,
			RawStat& throughputSmartGridControl, RawStat& delaySmartGridControl,
			RawStat& throughputSmartGridNotification, RawStat& delaySmartGridNotification,
			RawStat& throughputSmartGridControlDOS, RawStat& delaySmartGridControlDOS,
			RawStat& throughputSmartGridNotificationDOS, RawStat& delaySmartGridNotificationDOS);

	void stopCharging(const std::string& nodeId, const std::string& from);

	void schedPEV(double arrival);
	void updateStateOfCharge(std::string nodeId);
	void updateDischarge(const std::string& nodeId);

	static RawStat PEVThroughput;
	static RawStat PEVDelay;

	static RawStat UpdateSOCEvents;

    // send parameters
    long seqNum;
    cPar *reqLength;
    int lambda;

    int localSAP;
    int remoteSAP;

    // receive statistics
    long packetsSent;
    long packetsReceived;
    cOutVector eedVector;
    cStdDev eedStats;
    std::string applicationType;

    std::string bestEffortProfile;
    double bestEffortProfileMultiplier;
    std::string videoTrafficProfile;
    std::string voipTrafficProfile;
    std::string PEVArrivalsFile;
    std::string PEVsInChargingStation;
    std::string SolarModelChargingStation;
    bool withChargingStationSolarPanels;
    std::string PEVDepartureFile;
    std::string PEVDistanceDistribution;
    std::string DMSSchedulingAlgorithm;
    bool isPEV;
    bool PEVSet;
    double DMSSlotInterval;

    bool isTriplePlayGeneratorWithONUs;

    bool isDOS;

    int nbONUs;
    static MACAddress arbitraryHost;

  protected:
    virtual void initialize(int stage);
    virtual int numInitStages() const {return 9;}
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    void initializeFiWiTraffic();
    void initializeBestEffortTraffic();
    void initializeVideoTraffic();
    bool handleVideoMessage(cMessage *msg);
    void sendVideoFrame(int frameSize);

    void extractPEVDistanceDistribution();
    void extractPEVArrivalsHome();
    void extractPEVDepartures();
    void extractPEVsChargingStation();
    void extractChargingStationSolarDistribution();

    void initializeVoiceTraffic();
    void sendVoiceFrame(int frameSize);
    bool handleVoiceMessage(cMessage *msg);

    bool handleStartChargingAtChargingStation(cMessage* msg);

    void initializeDOSAttack();
    bool handleDosMessage(cMessage *msg);
    void sendSmartGridDos(int type);

    void sendBestEffortFrame();

    EtherFrame* convertToEtherFrame(EtherAppReq* frame);

    virtual void sendPacket(TrafficPair& traf);
    virtual void receivePacket(cMessage *msg);

    void startDischarging(const std::string& nodeId);

    virtual void sendPEVNotificationPacket(MACAddress dest);
    std::string msgBasicNotificationPacket(std::string nodeId, double voltage, double load);

    virtual void sendSubstationNotificationPacket(TrafficPair traf);
    std::string msgSubstationNotificationPacket(std::string nodeId, double totalPowerLoad, double totalPowerLosses);

    void sendChargingDeadlineRequest(bool justArrivedHome, bool wantV2G, const std::string& nodeId, double p_deadline);
    std::string msgChargingDeadlineRequestPacket(ChargingDeadlineRequestMessage* msg);

    void sendChargingDeadlineResponse(MACAddress dest, std::string nodeId, std::string vehicleID, std::string customerID, const std::vector<PEVSchedule>& startTime);
 std::string msgChargingDeadlineResponsePacket(ChargingDeadlineResponseMessage* msg, const std::vector<PEVSchedule>& schedules);

    std::string msgAuthenticationPacket(PEVAuthenticationMessage* msg);
    void sendAuthentication();

    std::string msgAuthenticationResponsePacket(PEVAuthenticationResponseMessage* msg);
    void sendAuthenticationResponse(MACAddress dest, std::string nodeId, std::string vehicleID, std::string customerID);

    std::string msgStateOfChargePacket(StateOfChargeMessage* msg);
    void sendStateOfCharge();

    void setPEVPlanning(const std::string& nodeId);

    std::string msgAckPacket(PowerSysACKMessage* msg);

    void sendPEVControlMessage(MACAddress dest, std::string nodeId, std::string vehicleID, std::string customerID);
    std::string msgPEVControlMessagePacket(PEVControlMessage* msg);

    std::string getPowerSystemBaseLoadFile() const { return powerSystemBaseLoadFile; }

    void recordStatsPEVPacket(cPacket* pkt);

    MACAddress aggregatorOf(MACAddress addr);

    int TCPHeaderSize() { return 20; }
    int IPv6HeaderSize() { return 40; }
    std::vector<cMessage*> PEVArrivalMsg;
    std::vector<cMessage*> PEVStartChargingMsg;
    std::vector<cMessage*> PEVEndChargingMsg;
    std::vector<cMessage*> PEVStartDischargingMsg;
    std::vector<cMessage*> PEVEndDischargingMsg;
    std::vector<cMessage*> PEVStateOfChargeMsg;

    std::vector<cMessage*> PEVsArrivalsChargingStation;
    std::vector<cMessage*> PEVsEndChargingStation;

  private:

    void stopDischarging(const std::string& nodeId);
    void startCharging(const std::string& nodeId);

    void startChargingAtChargingStation(const std::string& nodeId);

    void updateSOCWithDistance(const std::string& nodeId, double distance);

    std::vector<TrafficPair> myTrafficMatrix;

    std::vector<RowBaseLoad> baseLoads;
    std::string powerSystemBaseLoadFile;

    std::vector<std::string> pevsAtChargingStation;

    cMessage* V2GBeginMsg;

    EtherFrame* convertToEtherFrame(EtherAppReq* frame, TrafficPair traf);

    double PEVBatteryCapacity;
    double PEVBatteryKwPerHour;
    double initialPEVStateOfCharge;
    double PEVLambdaStateOfChargeMsg;
    double PEVAverageSpeed;

    int PEVRandomDeadlineMin;
    int PEVRandomDeadlineMax;
};

#endif


