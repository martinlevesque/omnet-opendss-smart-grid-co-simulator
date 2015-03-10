/*
 * Copyright (C) 2011 Martin Lévesque <levesquem@emt.inrs.ca>
 *
 *

 *
 * This program is free software; you can redistribute it and/or
 *
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

#include <omnetpp.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "FiWiTrafGen.h"
#include "Ieee802Ctrl_m.h"
#include "EtherApp_m.h"
#include "MyUtil.h"
#include "LinklessMAC.h"
#include "EtherMacVlan.h"
#include "EPON_mac.h"
#include "FiWiRoutingTable.h"
#include <string>
#include "FiWiTrafficMatrix.h"
#include "IBasic80211Mac.h"
#include "PEVMessages_m.h"
#include <sstream>
#include "OpenDSS.h"
#include "DMS.h"
#include <fstream>
#include <set>
#include <algorithm>
#include "FiWiGeneralConfigs.h"
#include <map>

using namespace std;

RawStat FiWiTrafGen::PEVThroughput = RawStat("PEVThroughput", "time-sum", 1);
RawStat FiWiTrafGen::PEVDelay = RawStat("PEVDelay", "time-mean", 1);
RawStat FiWiTrafGen::VideoStreamsDropRatio = RawStat("VideoStreamsDropRatio", "drop-ratio", 1);
RawStat FiWiTrafGen::BestEffortDropRatio = RawStat("BestEffortDropRatio", "drop-ratio", 1);
RawStat FiWiTrafGen::statsSOC = RawStat("SOC", "event", 1);
RawStat FiWiTrafGen::UpdateSOCEvents = RawStat("UpdateSOCEvents", "event", 1);
MACAddress FiWiTrafGen::arbitraryHost;

RawStat FiWiTrafGen::statsNbPEVsCharging = RawStat("NbPEVsCharging", "event", 1);

MACAddress FiWiTrafGen::dmsAddress;

int FiWiTrafGen::HybridNbSecondsPerHour = 0;

int FiWiTrafGen::curIndexChangeLoad = 0;
int FiWiTrafGen::totalNbNodes = 342;
set<string> FiWiTrafGen::listNodes;

Define_Module (FiWiTrafGen);

void FiWiTrafGen::initialize(int stage)
{
	static int totalNbPEVs;

    // we can only initialize in the 2nd stage (stage==1), because
    // assignment of "auto" MAC addresses takes place in stage 0
    if (stage == 6)
    {
        reqLength = &par("reqLength");
        applicationType = par("applicationType").stringValue();

        if (applicationType == "")
        {
        	applicationType = "normal";
        }

        bestEffortProfile = par("bestEffortProfile").stringValue();
        bestEffortProfileMultiplier = par("bestEffortProfileMultiplier").doubleValue();

        videoTrafficProfile = par("videoTrafficProfile").stringValue();
        voipTrafficProfile = par("voipTrafficProfile").stringValue();

        PEVArrivalsFile = string(par("PEVArrivalsFile").stringValue());

        PEVsInChargingStation = string(par("PEVsInChargingStation").stringValue());
        SolarModelChargingStation = string(par("SolarModelChargingStation").stringValue());
        withChargingStationSolarPanels = par("withChargingStationSolarPanels").boolValue();

        OpenDSS::Instance().withChargingStationSolarPanels = withChargingStationSolarPanels;

        PEVDepartureFile = string(par("PEVDepartureFile").stringValue());
        PEVDistanceDistribution = string(par("PEVDistanceDistribution").stringValue());
        DMSSchedulingAlgorithm = string(par("DMSSchedulingAlgorithm").stringValue());
        DMSSlotInterval = par("DMSSlotInterval").doubleValue();

        SLMMaxSlotLengthInSeconds = par("SLMMaxSlotLengthInSeconds").doubleValue();

        PEVBatteryCapacity = par("PEVBatteryCapacity").doubleValue();
        PEVBatteryKwPerHour = par("PEVBatteryKwPerHour").doubleValue();

        initialPEVStateOfCharge = par("PEVStateOfCharge").doubleValue();
        PEVLambdaStateOfChargeMsg = par("PEVLambdaStateOfChargeMsg").doubleValue();

        powerSystemBaseLoadFile = string(par("powerSystemBaseLoadFile").stringValue());

        isTriplePlayGeneratorWithONUs = par("isTriplePlayGeneratorWithONUs").boolValue();
        PEVPenetrationLevel = par("PEVPenetrationLevel").doubleValue();

        PEVAverageSpeed = par("PEVAverageSpeed").doubleValue();

        withChargingStations = par("withChargingStations").boolValue();

        isDOS = par("isDOS").boolValue();
        dosConfiguration.nextMessageSmartGridControl = NULL;
        dosConfiguration.nextMessageSmartGridNotification = NULL;

        nbONUs = par("nbONUs").longValue();

        nbTriesSched = 0;

        HybridNbSecondsPerHour = par("HybridNbSecondsPerHour").longValue();
        OpenDSS::Instance().NbSecondsPerHour = HybridNbSecondsPerHour;

        DMS::Instance().setMaximumPowerDemand(par("maximumPowerDemand").doubleValue());

        PEVRandomDeadlineMax = par("PEVRandomDeadlineMax");
        PEVRandomDeadlineMin = par("PEVRandomDeadlineMin");

        DMS::Instance().chargingStationSchedulingAlgorithm = par("chargingStationSchedulingAlgorithm").stringValue();

        // Stats:
        // Upstream
        DMSThroughput = RawStat("DMSThroughput", "time-sum", 1);
        		DMSDelay = RawStat("DMSDelay", "time-mean", 1);
        		DMSThroughputVideo = RawStat("DMSThroughputVideo", "time-sum", 1);
        		DMSDelayVideo = RawStat("DMSDelayVideo", "time-mean", 1);
        		DMSThroughputVoice = RawStat("DMSThroughputVoice", "time-sum", 1);
        		DMSDelayVoice = RawStat("DMSDelayVoice", "time-mean", 1);
        		DMSThroughputBestEffort = RawStat("DMSThroughputBestEffort", "time-sum", 1);
        		DMSDelayBestEffort = RawStat("DMSDelayBestEffort", "time-mean", 1);
        		DMSThroughputSmartGridControl = RawStat("DMSThroughputSmartGridControl", "time-sum", 1);
        		DMSDelaySmartGridControl = RawStat("DMSDelaySmartGridControl", "time-mean", 1);
        		DMSThroughputSmartGridControlDOS = RawStat("DMSThroughputSmartGridControlDOS", "time-sum", 1);
        		DMSDelaySmartGridControlDOS = RawStat("DMSDelaySmartGridControlDOS", "time-mean", 1);
        		DMSThroughputSmartGridNotification = RawStat("DMSThroughputSmartGridNotification", "time-sum", 1);
        		DMSDelaySmartGridNotification = RawStat("DMSDelaySmartGridNotification", "time-mean", 1);
        		DMSThroughputSmartGridNotificationDOS = RawStat("DMSThroughputSmartGridNotificationDOS", "time-sum", 1);
        		DMSDelaySmartGridNotificationDOS = RawStat("DMSDelaySmartGridNotificationDOS", "time-mean", 1);

		// Downstream
		DownstreamThroughput = RawStat("DownstreamThroughput", "time-sum", 1);
		DownstreamDelay = RawStat("DownstreamDelay", "time-mean", 1);
		DownstreamThroughputVideo = RawStat("DownstreamThroughputVideo", "time-sum", 1);
		DownstreamDelayVideo = RawStat("DownstreamDelayVideo", "time-mean", 1);
		DownstreamThroughputVoice = RawStat("DownstreamThroughputVoice", "time-sum", 1);
		DownstreamDelayVoice = RawStat("DownstreamDelayVoice", "time-mean", 1);
		DownstreamThroughputBestEffort = RawStat("DownstreamThroughputBestEffort", "time-sum", 1);
		DownstreamDelayBestEffort = RawStat("DownstreamDelayBestEffort", "time-mean", 1);
		DownstreamThroughputSmartGridControl = RawStat("DownstreamThroughputSmartGridControl", "time-sum", 1);
		DownstreamDelaySmartGridControl = RawStat("DownstreamDelaySmartGridControl", "time-mean", 1);
		DownstreamThroughputSmartGridNotification = RawStat("DownstreamThroughputSmartGridNotification", "time-sum", 1);
		DownstreamDelaySmartGridNotification = RawStat("DownstreamDelaySmartGridNotification", "time-mean", 1);

        DMS::Instance().setSlotInterval(DMSSlotInterval);


        isPEV = applicationType == "residentialPEV";
        PEVSet = false;

        if (isPEV)
        {
        	++totalNbPEVs;
        }
        else
        {
        	EV << this->getName() << " NOT PEV ! " << endl;
        }

        EV << "app type = " << applicationType << endl;


        localSAP = ETHERAPP_CLI_SAP;
        remoteSAP = ETHERAPP_SRV_SAP;

        seqNum = 0;
        WATCH(seqNum);

        // statistics
        packetsSent = packetsReceived = 0;
        eedVector.setName("end-to-end delay");
        eedStats.setName("end-to-end delay");
        WATCH(packetsSent);
        WATCH(packetsReceived);

		string name = this->getFullPath();

		if (name.find(".host") != string::npos || name.find(".video") != string::npos || name.find(".voice") != string::npos ||
				name.find(".best") != string::npos)
		{
			LinklessMAC* mac = dynamic_cast<LinklessMAC*>(MyUtil::findModuleUp(this, "mac"));

			if ( ! mac)
			{
				error("FiWiTrafGen::initialize no mac ?");
			}

			// It's traf gen attached to ONU
			cModule * module = MyUtil::getNeighbourOnGate(mac, "phys$o");

			if ( ! module)
			{
				error("FiWiTrafGen::initialize err 1");
			}

			module = MyUtil::getNeighbourOnGate(module, "ethg$o");
			cModule* tmpBeforeMac = module;

			if ( ! module)
			{
				error("FiWiTrafGen::initialize err 2");
			}

			module = module->getSubmodule("mac", 0);

			if ( ! module)
			{
				// Maybe we are in a bridge
				module = tmpBeforeMac->getSubmodule("noDelaySwitch");

				if ( ! module)
				{
					error("FiWiTrafGen::initialize err 3");
				}
				else
				{
					// In bridge.
					module = MyUtil::getNeighbourOnGate(module, "single$o");

					if ( ! module)
					{
						error("FiWiTrafGen::initialize err 3.2");
					}

					module = MyUtil::getNeighbourOnGate(module, "single$o");

					if ( ! module)
					{
						error("FiWiTrafGen::initialize err 3.3");
					}

					module = module->getSubmodule("mac", 0);

					if ( ! module)
					{
						error("FiWiTrafGen::initialize err 3.4");
					}
				}
			}

			EtherMacVlan* macVlan = dynamic_cast<EtherMacVlan*>(MyUtil::findModuleUp(module, "mac"));

			if ( ! macVlan)
			{
				error("FiWiTrafGen::initialize no mac vlan ??");
			}

			module = MyUtil::findModuleUp(module, "onu_port");

			if ( ! module)
			{
				error("FiWiTrafGen::initialize err 4");
			}

			module = module->getSubmodule("epon_mac", 0);

			if ( ! module)
			{
				error("FiWiTrafGen::initialize err 5");
			}

			MACAddress addrONUConnected = dynamic_cast<EPON_mac*>(module)->getMACAddress();

			myAddress = mac->getMACAddress();

			if (arbitraryHost.isUnspecified())
			{
				EV << "arrrrrbitrary" << endl;
				arbitraryHost = myAddress;
			}

			MyUtil::getRoutingTable(this)->addAdjacent(FiWiNode(myAddress, FIWI_NODE_TYPE_PON), FiWiNode(addrONUConnected, FIWI_NODE_TYPE_ANY));
			MyUtil::getRoutingTable(this)->print();

			// Then connect OLT to ONU
			EV << "connect olt to onu, olt addr = " << MyUtil::getRoutingTable(this)->getOltAddr() << endl;
			MyUtil::getRoutingTable(this)->addAdjacent(FiWiNode(MyUtil::getRoutingTable(this)->getOltAddr(), FIWI_NODE_TYPE_PON), FiWiNode(addrONUConnected, FIWI_NODE_TYPE_PON));
			MyUtil::getRoutingTable(this)->print();

			// Connect ONU to vlan
			MyUtil::getRoutingTable(this)->addAdjacent(FiWiNode(addrONUConnected, FIWI_NODE_TYPE_PON), FiWiNode(macVlan->getMACAddress(), FIWI_NODE_TYPE_ANY));
			MyUtil::getRoutingTable(this)->print();

			// Connect vlan to traf gen
			MyUtil::getRoutingTable(this)->addAdjacent(FiWiNode(myAddress, FIWI_NODE_TYPE_ANY), FiWiNode(macVlan->getMACAddress(), FIWI_NODE_TYPE_ANY));
			MyUtil::getRoutingTable(this)->print();
		}
		else
		if (name.find(".dms.") != string::npos)
		{
			// We attach DMS to OLT
			LinklessMAC* mac = dynamic_cast<LinklessMAC*>(MyUtil::findModuleUp(this, "mac"));

			if ( ! mac)
			{
				error("FiWiTrafGen::initialize no mac ?");
			}

			myAddress = mac->getMACAddress();
			dmsAddress = myAddress;

			MyUtil::getRoutingTable(this)->addAdjacent(FiWiNode(mac->getMACAddress(), FIWI_NODE_TYPE_PON),
					FiWiNode(MyUtil::getRoutingTable(this)->getOltAddr(), FIWI_NODE_TYPE_PON));

			MyUtil::getRoutingTable(this)->setDMSAddr(mac->getMACAddress());

			MyUtil::getRoutingTable(this)->print();
		}

		else
		{

			cModule* mod = MyUtil::findModuleUp(this, "wlan");

			if ( ! mod)
			{
				error("FiWiTrafGen::initialize no mac 2 ?");
			}


			// Grab mac
			IBasic80211Mac* mac = dynamic_cast<IBasic80211Mac*>(MyUtil::findModuleUp(mod, "mac"));

			if ( ! mac)
			{
				error("FiWiTrafGen::initialize no mac 3");
			}

			myAddress = mac->getMACAddress();
		}

		FiWiTrafficMatrix* trafMatrix = MyUtil::getTrafficMatrix(this);

		if ( ! trafMatrix)
		{
			error("FiWiTrafGen::initialize - no traf matrix");
		}

		myTrafficMatrix = trafMatrix->getTrafficMatrix(myAddress);

		for (int i = 0; i < (int)myTrafficMatrix.size(); ++i)
		{
			// With random behaviour only when normal traffic gen
			myTrafficMatrix[i].withRandomBehaviour = applicationType == "normal";
		}

		EV << "nb trafs = " << myTrafficMatrix.size() << endl;

		if (applicationType == "normal" || applicationType == "substation" ||
				(isPEV && OpenDSS::Instance().getReactiveControlSensorType() == "dataRateBased"))
		{
			initializeFiWiTraffic();
		}
		else
		if (applicationType == "bestEffort")
		{
			initializeBestEffortTraffic();
		}
		else
		if (applicationType == "video")
		{
			initializeVideoTraffic();
		}
		else
		if (applicationType == "voice")
		{
			initializeVoiceTraffic();
		}

		if (isDOS)
		{
			initializeDOSAttack();
		}

		if (applicationType == "dms" && isTriplePlayGeneratorWithONUs)
		{
			initializeBestEffortTraffic();
			initializeVideoTraffic();
			initializeVoiceTraffic();
		}
		if (applicationType == "controller")
		{
					}


		if (applicationType == "dms")
		{
			DMS::Instance().setSLMMaxSlotLengthInSeconds(SLMMaxSlotLengthInSeconds);
		}

		string nodeId = OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress);
		updateStateOfCharge(nodeId);

		string test = OpenDSS::Instance().getPowerNodeFromMACAddress(myAddress);
		static bool updatedEventsBaseLoad = false;

		// for event-based, REMOVE updatedEventsBaseLoad
		bool testPEV = applicationType != "normal" && applicationType != "chargingStation" && applicationType != "none" && applicationType != "bestEffort" &&
				applicationType != "video" && applicationType != "voice" && applicationType != "voip" && applicationType != "dms" && applicationType != "controller";

		if (testPEV &&
				! updatedEventsBaseLoad)
		{
			updatedEventsBaseLoad = true; // false for event-based!
			//////////////
			// Base load:
			EV << "base load file = " << powerSystemBaseLoadFile << endl;// fix files for prettynanogrid
			ifstream baseLoadStreamFile;

			baseLoadStreamFile.open(powerSystemBaseLoadFile.c_str());

			if (baseLoadStreamFile.is_open())
			{
				vector<double> timesWithBaseLoadEvents;

				while ( ! baseLoadStreamFile.eof())
				{
					RowBaseLoad l;

					baseLoadStreamFile >> l.time;
					baseLoadStreamFile >> l.powerNodeId;
					baseLoadStreamFile >> l.load;

					if (l.powerNodeId == "")
					{
						break;
					}

					// for event-based, CHANGE
					// for event-based, CHANGE
										/*
										if (l.powerNodeId != OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress))
										{
											continue;
										}
										*/


										//l.time += (double)((int)uniform(0, 100)) / 100.0;

										listNodes.insert(l.powerNodeId);
										EV << "added base load " << l.time << " " << l.powerNodeId << " " << l.load << " nbAdded = " << listNodes.size() << endl;
										baseLoads.push_back(l);

										// Check if we must add base load event
										for (double t = 2; t <= 130; t += HybridNbSecondsPerHour)
										{
											if (find(timesWithBaseLoadEvents.begin(), timesWithBaseLoadEvents.end(), t) == timesWithBaseLoadEvents.end())
											{
												cMessage* baseLoadMessage = new cMessage(GENERAL_CONF_NAME_MSG_BASE_LOAD_UPDATE, 101);

												EV << "okkkk scheduling at " << t << endl;

												timesWithBaseLoadEvents.push_back(t);

												scheduleAt(t, baseLoadMessage);
											}
										}
									}

									baseLoadStreamFile.close();


								}
								else
								{
									error("FiWiTrafGen::initialize can't read power system base load");
								}
							}
					    }
					    else
					    if (stage == 7)
					    {
					    	EV << "totalNbPEVs = " << totalNbPEVs << endl;

					    	//////////////////
							// PEV arrivals
					    	if (isPEV)
					    	{
					    		static bool allocated;
					    		static map<string, int> nbAllocatedInLV;
					    		static vector<string> allocatedPEVs;

					    		if ( ! allocated)
					    		{
					    			allocated = true;

					    			extractPEVDistanceDistribution();
					    			extractPEVArrivalsHome();
					    			extractPEVDepartures();

									map<string, set<string> > PEVsInLV;

									set<string> lvs = OpenDSS::Instance().getLVs();

									int nbAllocatePerLV = ceil((double)(342.0/18.0) * PEVPenetrationLevel);

									EV << "totalNbPEVs = " << totalNbPEVs << " nbAllocatePerLV = " << nbAllocatePerLV << " nb lvs = " << lvs.size() << endl;

									// Loop over the power nodes:
									std::map<std::string, MACAddress> powerNodes = OpenDSS::Instance().getNodes();

									vector<string> randomizedNodes;

									vector<string> tmpNodes;

									for (std::map<std::string, MACAddress>::iterator it = powerNodes.begin(); it != powerNodes.end(); ++it)
									{
										tmpNodes.push_back(it->first);
									}

									while (randomizedNodes.size() != tmpNodes.size())
									{
										int index = (int)uniform(0, tmpNodes.size());

										if (find(randomizedNodes.begin(), randomizedNodes.end(), tmpNodes[index]) == randomizedNodes.end())
										{
											randomizedNodes.push_back(tmpNodes[index]);
										}
									}

									for (int i = 0; i < (int)randomizedNodes.size(); ++i)
									{
										string lvName = OpenDSS::Instance().LVNameOf(randomizedNodes[i]);

										EV << "lv name of " << randomizedNodes[i] << " is " << lvName << endl;

										if (nbAllocatedInLV[lvName] >= nbAllocatePerLV)
											continue;

										nbAllocatedInLV[lvName] += 1;
										allocatedPEVs.push_back(randomizedNodes[i]);
									}
					    		}

					    		string nodeId = OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress);

					    		EV << " pev = " << nodeId << " houses with pev = " << allocatedPEVs.size() << endl;

					    		if (find(allocatedPEVs.begin(), allocatedPEVs.end(), nodeId) != allocatedPEVs.end())
					    		{
					    			PEVSet = true;

									EV << "im in !" << nodeId << endl;

									setPEVPlanning(nodeId);

									schedPEV(OpenDSS::Instance().getPEVPlanningOf(nodeId).arrivalTimeAtHome);

									V2GBeginMsg = new cMessage("V2GBeginMsg", 102);
									double t = uniform(DMS::Instance().getV2gBeginAt(), DMS::Instance().getV2gBeginAt() + 1);

									scheduleAt(t, V2GBeginMsg);
					    		}
					    	}
					    }

    else
	if (stage == 8)
	{
		string nodeId = OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress);

		if (applicationType == "chargingStation")
			OpenDSS::Instance().infosGrid.chargingStationNodes[nodeId] = PowerSysNode(nodeId);

		if (applicationType == "chargingStation" && withChargingStations)
		{
			extractPEVsChargingStation();

			if (withChargingStationSolarPanels)
			{
				extractChargingStationSolarDistribution();
			}

			OpenDSS::Instance().infosGrid.chargingStationNodes[nodeId].setPEVKwH(this->PEVBatteryKwPerHour);

			EV << "new node.. = " << nodeId << endl;

			// PEVsArrivalsChargingStation

			for (std::map<std::string, PEVPlanning>::iterator it = OpenDSS::Instance().pevPlannings.begin(); it != OpenDSS::Instance().pevPlannings.end(); ++it)
			{
				for (int i = 0; i < (int)pevsAtChargingStation.size(); ++i)
				{
					if (pevsAtChargingStation[i] == it->first)
					{
						EV << "pev will arrive at " << it->second.arrivalTimeAtWork << endl;

						cMessage* m = new cMessage("pev-arrival-charging-station", 101);
						PEVsArrivalsChargingStation.push_back(m);
						scheduleAt(it->second.arrivalTimeAtWork, m);

						break;
					}
				}
			}
		}
	}
}

void FiWiTrafGen::setPEVPlanning(const std::string& nodeId)
{
	double departure = OpenDSS::Instance().randPEVDeparture(2, HybridNbSecondsPerHour);
	OpenDSS::Instance().setPEVPlanningDepartureTimeFromHomeOf(nodeId, departure);
	OpenDSS::Instance().setPEVPlanningPEVSpeedMilesPerHour(nodeId, PEVAverageSpeed);

	double distance = OpenDSS::Instance().randPEVDistanceToDo() / 2.0;
	OpenDSS::Instance().setPEVPlanningDistanceToWork(nodeId, distance);

	double timeToGoWorkInSeconds = (distance / PEVAverageSpeed) * (double)HybridNbSecondsPerHour;
	OpenDSS::Instance().setPEVPlanningPEVTimeToGoToWork(nodeId, timeToGoWorkInSeconds);

	double arrivalTimeAtWork = departure + timeToGoWorkInSeconds;
	OpenDSS::Instance().setPEVPlanningArrivalTimeAtWork(nodeId, arrivalTimeAtWork);
	OpenDSS::Instance().setPEVPlanningArrivalTimeAtWorkProcessed(nodeId, false);

	double arrivalTimeAtHome = OpenDSS::Instance().randPEVArrivalHome(arrivalTimeAtWork + timeToGoWorkInSeconds, HybridNbSecondsPerHour);
	double departureTimeFromWork = arrivalTimeAtHome - timeToGoWorkInSeconds;

	OpenDSS::Instance().setPEVPlanningDepartureTimeFromWork(nodeId, departureTimeFromWork);

	OpenDSS::Instance().setStopChargingAtChargingStationAt(nodeId, departureTimeFromWork);
	OpenDSS::Instance().setStopChargingAtChargingStationAtProcessed(nodeId, false);

	OpenDSS::Instance().setPEVPlanningArrivalTimeAtHome(nodeId, arrivalTimeAtHome);

	double deadline = (departure + uniform(this->PEVRandomDeadlineMin, this->PEVRandomDeadlineMax) * (double)OpenDSS::Instance().NbSecondsPerHour);
	OpenDSS::Instance().setPEVPlanningDeadline(nodeId, deadline);

	// Set state of charge
	OpenDSS::Instance().setStateOfChargeOf(nodeId, initialPEVStateOfCharge);
	OpenDSS::Instance().setPEVStartTime(nodeId, -1);

	static RawStat stats = RawStat("SOCSetPEVPlanning", "event", 1);
	stats.log(simTime().dbl(), arrivalTimeAtHome, "arrival time at home");
	stats.log(simTime().dbl(), deadline, "deadline");

	stats.log(simTime().dbl(), (deadline-arrivalTimeAtHome)/4, "hours");

	EV << "PEV " << nodeId << " departure = " << departure << " pev speed = " << PEVAverageSpeed << " distance  to work = " << distance
			<< " time to go to work = " << timeToGoWorkInSeconds << " arrival at work = " << arrivalTimeAtWork << " departure from work = " << departureTimeFromWork
			<< " arrival at home =  " << arrivalTimeAtHome << endl;
}

void FiWiTrafGen::extractPEVArrivalsHome()
{
	ifstream pevArrivalFile;

	pevArrivalFile.open(PEVArrivalsFile.c_str());

	if (pevArrivalFile.is_open())
	{
		double curIndex = 0;

		while ( ! pevArrivalFile.eof())
		{
			PEVArrivalHome a;
			a.minIndex = curIndex;

			pevArrivalFile >> a.hour;
			pevArrivalFile >> a.percent;

			if (a.percent <= 0 && a.hour <= 0)
			{
				break;
			}

			curIndex += a.percent;

			a.maxIndex = curIndex;

			EV << "hour " << a.hour << " , min = " << a.minIndex << " max = " << a.maxIndex << endl;

			OpenDSS::Instance().addArrivalHome(a);
		}

		pevArrivalFile.close();
	}
	else
	{
		error("FiWiGeneralConfigs::initialize can't read pevArrivalFile file");
	}
}



void FiWiTrafGen::extractPEVsChargingStation()
{
	ifstream file;

	file.open(PEVsInChargingStation.c_str());

	if (file.is_open())
	{
		while ( ! file.eof())
		{
			string pev;

			file >> pev;

			if (pev == "")
			{
				break;
			}

			EV << "pev at charging station " << pev << endl;

			pevsAtChargingStation.push_back(pev);
		}

		file.close();
	}
	else
	{
		error("FiWiTrafGen::initialize can't read pev at charging station file");
	}
}

void FiWiTrafGen::extractChargingStationSolarDistribution()
{
	ifstream file;

	file.open(SolarModelChargingStation.c_str());

	string nodeId = OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress);

	if (file.is_open())
	{
		int i = 0;
		while ( ! file.eof())
		{
			++i;

			int hour;
			double loadInKW;

			file >> hour;
			file >> loadInKW;

			if (i > 24) // 24 hours
			{
				break;
			}

			EV << "SOLAR distribution, node = " << nodeId << " hour = " << hour << " load in kW = " << loadInKW << endl;

			OpenDSS::Instance().solarDistributionAt[nodeId][hour] = loadInKW;
		}

		file.close();
	}
	else
	{
		error("FiWiTrafGen::initialize can't read pev at charging station file");
	}
}

void FiWiTrafGen::extractPEVDepartures()
{
	ifstream file;

	file.open(PEVDepartureFile.c_str());

	if (file.is_open())
	{
		double curIndex = 0;

		while ( ! file.eof())
		{
			PEVDeparture d;
			d.minIndex = curIndex;

			file >> d.hour;
			file >> d.percent;

			if (d.percent <= 0 && d.hour <= 0 && curIndex > 0)
			{
				break;
			}

			curIndex += d.percent;

			d.maxIndex = curIndex;

			EV << "pev departure conf hour " << d.hour << " , min = " << d.minIndex << " max = " << d.maxIndex << endl;

			OpenDSS::Instance().addPevDeparture(d);
		}

		file.close();
	}
	else
	{
		error("FiWiTrafGen::initialize can't read pev departures file");
	}
}

void FiWiTrafGen::extractPEVDistanceDistribution()
{
	ifstream file;

	file.open(PEVDistanceDistribution.c_str());

	if (file.is_open())
	{
		double curIndex = 0;

		while ( ! file.eof())
		{
			PEVDistance a;
			a.minIndex = curIndex;

			file >> a.distanceMin;
			file >> a.distanceMax;
			file >> a.percent;

			if (a.percent <= 0 && a.distanceMax <= 0)
			{
				break;
			}

			curIndex += a.percent;

			a.maxIndex = curIndex;

			EV << "extractPEVDistanceDistribution distance min = " << a.distanceMin << " max = " << a.distanceMax << " percent = " << a.percent << " min in " << a.minIndex
					<< " max in " << a.maxIndex << endl;

			OpenDSS::Instance().addDistanceDistribution(a);
		}

		file.close();
	}
	else
	{
		error("FiWiTrafGen::initialize can't read distance distribution file");
	}
}

void FiWiTrafGen::schedPEV(double arrival)
{
	// maximum 5 times!
	if (++nbTriesSched > 5)
		return;

	cMessage* newMsg = new cMessage((string("PEV-arrival-") + myAddress.str()).c_str(), 101);

	scheduleAt(arrival, newMsg);

	PEVArrivalMsg.push_back(newMsg);
}

void FiWiTrafGen::updateStateOfCharge(string nodeId)//for stop chargin function
{
	if (OpenDSS::Instance().getPEVStartTime(nodeId) == -1)
		return;

	// Calculate state of charge
	double kWAtBeginning = OpenDSS::Instance().getStateOfChargeOf(nodeId) * this->PEVBatteryCapacity;

	double kWDone = ((simTime().dbl() - OpenDSS::Instance().getPEVStartTime(nodeId)) / (double)OpenDSS::Instance().NbSecondsPerHour) * this->PEVBatteryKwPerHour;
	//double kWDone = 1.44;
	double soc = (kWAtBeginning + kWDone) / this->PEVBatteryCapacity;

	if (soc > 1)
		soc = 1;

	if (soc < 0)
		soc = 0;

	static RawStat stats = RawStat("SOCUpdateStateOfCharge", "event", 1);
	stats.log(simTime().dbl(), soc, nodeId);
	stats.log(simTime().dbl(), kWAtBeginning, "kWAtBeginning");
	stats.log(simTime().dbl(), kWDone, "kWDone");
	stats.log(simTime().dbl(), OpenDSS::Instance().getPEVPlanningOf(nodeId).distanceToWork, "distance to work");
	stats.log(simTime().dbl(), OpenDSS::Instance().getPEVPlanningOf(nodeId).departureTimeFromHome, "departureTimeFromHome");
	stats.log(simTime().dbl(), OpenDSS::Instance().getPEVPlanningOf(nodeId).arrivalTimeAtWork, "arrival time at work");
	stats.log(simTime().dbl(), OpenDSS::Instance().getPEVPlanningOf(nodeId).departureTimeFromWork, "departure time from work");
	stats.log(simTime().dbl(), OpenDSS::Instance().getPEVPlanningOf(nodeId).arrivalTimeAtHome, "arrivalTimeAtHome");
	stats.log(simTime().dbl(), OpenDSS::Instance().getPEVPlanningOf(nodeId).stopChargingAtChargingStationAt, "stopChargingAtChargingStationAt");
	stats.log(simTime().dbl(), OpenDSS::Instance().getPEVPlanningOf(nodeId).PEVSpeedMilesPerHour, "PEVSpeedMilesPerHour");
	stats.log(simTime().dbl(), OpenDSS::Instance().getPEVPlanningOf(nodeId).PEVTimeToGoToWork, "PEVTimeToGoToWork");
	stats.log(simTime().dbl(), OpenDSS::Instance().getPEVPlanningOf(nodeId).deadline, "deadline");
	stats.log(simTime().dbl(), 0, "");

	OpenDSS::Instance().setStateOfChargeOf(nodeId, soc);
}

void FiWiTrafGen::initializeFiWiTraffic()
{
	for (int i = 0; i < (int)myTrafficMatrix.size(); ++i)
	{
		myTrafficMatrix[i].nextPacketMsg = new cMessage("generateNextPacket");

		EV << "lambda = " << myTrafficMatrix[i].lambda << " random = " << poisson(myTrafficMatrix[i].lambda) <<  " the waittime = " << myTrafficMatrix[i].waitTime() << " SRC = " << myTrafficMatrix[i].src << ", dest = " << myTrafficMatrix[i].dest << endl;

		simtime_t d = par("startTime").doubleValue();
		scheduleAt(simTime() + d + myTrafficMatrix[i].waitTime(), myTrafficMatrix[i].nextPacketMsg);
	}
}

void FiWiTrafGen::initializeBestEffortTraffic()
{
	ifstream beProfile;

	beProfile.open(bestEffortProfile.c_str());

	if (beProfile.is_open())
	{
		while ( ! beProfile.eof())
		{
			BestEffortConfiguration c;

			beProfile >> c.from;
			beProfile >> c.to;
			beProfile >> c.bps;

			// Central generator to ONUs ?
			if (isTriplePlayGeneratorWithONUs)
			{
				c.bps = c.bps * (double)nbONUs;
			}

			if (c.bps > 0)
			{
				bestEffortConfs.push_back(c);
				EV << "best effort profile = " << c.from << " to " << c.to << " = " << c.bps << endl;
			}
		}

		beProfile.close();
	}

	nextBestEffortMsg = new cMessage("best-effort-message-sched", 101);
	scheduleAt(2 + 1.0 / MyUtil::randomPoisson(10000), nextBestEffortMsg);
}

void FiWiTrafGen::initializeVideoTraffic()
{
	ifstream profile;
	double MIN_DURATION_VIDEO_SESSION_DURATION = 0.5 * (double)HybridNbSecondsPerHour;
	double MAX_DURATION_VIDEO_SESSION_DURATION = 3.0 * (double)HybridNbSecondsPerHour;

	EV << "initializeVideoTraffic 1 " << endl;

	profile.open(videoTrafficProfile.c_str());

	if (profile.is_open())
	{
		EV << "initializeVideoTraffic 2 " << endl;
		while ( ! profile.eof())
		{
			double start;
			double end;
			int nb;

			profile >> start;
			profile >> end;
			profile >> nb;

			// Central generator to ONUs ?
			if (isTriplePlayGeneratorWithONUs)
			{
				nb = nb * nbONUs;
			}

			EV << "initializeVideoTraffic 3 " << start << " end = " << end << " nb = " << nb << endl;

			if (nb > 0)
			{
				for (int i = 0; i < nb; ++i)
				{
					VideoConfiguration c;

					double startTime = uniform(start, end);
					double randomDuration = uniform(MIN_DURATION_VIDEO_SESSION_DURATION, MAX_DURATION_VIDEO_SESSION_DURATION);
					double endTime = startTime + randomDuration;

					c.nextMessage = new cMessage("nextVideoMessage", 101);
					scheduleAt(startTime, c.nextMessage);

					c.endSession = new cMessage("endVideoSession", 101);
					scheduleAt(endTime, c.endSession);

					videoConfs.push_back(c);
					EV << "video conf = " << startTime << " to " << endTime << " = " << nb << endl;
				}
			}
		}

		profile.close();
	}
}

void FiWiTrafGen::sendVideoFrame(int frameSize)
{
	static long id = 0;
	++id;

	char msgname[300];
	sprintf(msgname, "video-%d-%ld", getId(), id);
	EV << "Generating packet  `" << msgname << "'\n";

	EtherAppReq *datapacket = new EtherAppReq(msgname, IEEE802CTRL_DATA);

	datapacket->setRequestId(id);

	long len = frameSize;
	datapacket->setByteLength(len);

	MACAddress video1 = MyUtil::resolveDestMACAddress(this, "video1");

	Ieee802Ctrl* etherctrl = new Ieee802Ctrl();
	etherctrl->setSsap(localSAP);
	etherctrl->setDsap(remoteSAP);
	etherctrl->setDest(isTriplePlayGeneratorWithONUs ? video1 : FiWiTrafGen::dmsAddress);
	etherctrl->setSrc(myAddress);
	datapacket->setControlInfo(etherctrl);

	EtherFrame* f = convertToEtherFrame(datapacket);

	f->setPktType(FIWI_TRAF_PKT_TYPE_VIDEO);

	if (isTriplePlayGeneratorWithONUs)
	{
		f->setHost(etherctrl->getDest().str().c_str());
	}
	else
	{
		f->setHost(etherctrl->getSrc().str().c_str());
	}

	f->setIsDOS(isDOS);

	FiWiTrafGen::VideoStreamsDropRatio.addStat(simTime().dbl(), 1, 0);

	send(f, "out");
}

bool FiWiTrafGen::handleVideoMessage(cMessage *msg)
{
	// MEDIUM SIZE: http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=4897311
	double MIN_FRAME_SIZE = 300;
	double MAX_FRAME_SIZE = 1000;
	double MEAN_FRAME_SIZE = ((MAX_FRAME_SIZE - MIN_FRAME_SIZE) / 2.0) + MIN_FRAME_SIZE;

	// VBR
	// Big Buck Bunny, 40 dB, 430 kbps, H.264/AV
	double DATA_RATE = 430000;
	int LAMBDA = DATA_RATE / (MEAN_FRAME_SIZE * 8.0);

	bool handled = false;
	int indexToDelete = -1;

	for (int i = 0; i < (int)videoConfs.size(); ++i)
	{
		if (msg == videoConfs[i].nextMessage)
		{
			int frameSize = (int)uniform(MIN_FRAME_SIZE, MAX_FRAME_SIZE);

			sendVideoFrame(frameSize);

			double nextMsg = simTime().dbl() + 1.0 / MyUtil::randomPoisson(LAMBDA);
			scheduleAt(nextMsg, videoConfs[i].nextMessage);

			handled = true;
		}
		else
		if (msg == videoConfs[i].endSession)
		{
			indexToDelete = i;
			handled = true;
			break;
		}
	}

	if (indexToDelete >= 0)
	{
		videoConfs.erase(videoConfs.begin() + indexToDelete);
	}

	return handled;
}

void FiWiTrafGen::initializeDOSAttack()
{
	double startDOS = 2;

	dosConfiguration.nextMessageSmartGridControl = new cMessage("dos-message-smart-grid-control", 101);
	scheduleAt(startDOS + uniform(0, 1), dosConfiguration.nextMessageSmartGridControl);

	dosConfiguration.nextMessageSmartGridNotification = new cMessage("dos-message-smart-grid-notification", 101);
	scheduleAt(startDOS + uniform(0, 1), dosConfiguration.nextMessageSmartGridNotification);

}

bool FiWiTrafGen::handleDosMessage(cMessage *msg)
{
	if (dosConfiguration.nextMessageSmartGridControl == msg)
	{
		double next = 1.0 / 10000;
		scheduleAt(simTime().dbl() + next, dosConfiguration.nextMessageSmartGridControl);

		sendSmartGridDos(FIWI_TRAF_PKT_TYPE_SMART_GRID_CONTROL);

		return true;
	}
	else
	if (dosConfiguration.nextMessageSmartGridNotification == msg)
	{
		double next = 1.0 / 10000;
		scheduleAt(simTime().dbl() + next, dosConfiguration.nextMessageSmartGridNotification);

		sendSmartGridDos(FIWI_TRAF_PKT_TYPE_SMART_GRID_NOTIFICATION);

		return true;
	}

	return false;
}

void FiWiTrafGen::sendSmartGridDos(int type)
{
	static long id = 0;
	++id;

	char msgname[300];
	sprintf(msgname, "dos-pkt-%d-%ld", getId(), id);
	EV << "Generating dos packet  `" << msgname << "'\n";

	EtherAppReq *datapacket = new EtherAppReq(msgname, IEEE802CTRL_DATA);

	datapacket->setRequestId(id);

	long len = 500;
	datapacket->setByteLength(len);

	Ieee802Ctrl* etherctrl = new Ieee802Ctrl();
	etherctrl->setSsap(localSAP);
	etherctrl->setDsap(remoteSAP);
	etherctrl->setDest(FiWiTrafGen::dmsAddress);
	etherctrl->setSrc(myAddress);
	datapacket->setControlInfo(etherctrl);

	EtherFrame* f = convertToEtherFrame(datapacket);

	f->setPktType(type);

	f->setHost(etherctrl->getSrc().str().c_str());
	f->setIsDOS(this->isDOS);

	send(f, "out");
}

void FiWiTrafGen::initializeVoiceTraffic()
{
	ifstream profile;

	double MIN_DURATION_VOICE_SESSION_DURATION = 0.25 * (double)HybridNbSecondsPerHour;
	double MAX_DURATION_VOICE_SESSION_DURATION = 1.0 * (double)HybridNbSecondsPerHour;

	EV << "initializeVoiceTraffic 1 " << endl;

	profile.open(voipTrafficProfile.c_str());

	if (profile.is_open())
	{
		EV << "initializeVoiceTraffic 2 " << endl;
		while ( ! profile.eof())
		{
			double start;
			double end;
			int nb;

			profile >> start;
			profile >> end;
			profile >> nb;

			EV << "initializeVoiceTraffic 3 " << start << " end = " << end << " nb = " << nb << endl;

			if (nb > 0)
			{
				for (int i = 0; i < nb; ++i)
				{
					VoiceConfiguration c;

					double startTime = uniform(start, end);
					double randomDuration = uniform(MIN_DURATION_VOICE_SESSION_DURATION, MAX_DURATION_VOICE_SESSION_DURATION);
					double endTime = startTime + randomDuration;

					c.nextMessage = new cMessage("nextVoiceMessage", 101);
					scheduleAt(startTime, c.nextMessage);

					c.endSession = new cMessage("endVoiceSession", 101);
					scheduleAt(endTime, c.endSession);

					voiceConfs.push_back(c);
					EV << "voice conf = " << startTime << " to " << endTime << " = " << nb << endl;
				}
			}
		}

		profile.close();
	}
}

void FiWiTrafGen::sendVoiceFrame(int frameSize)
{
	static long id = 0;
	++id;

	char msgname[300];
	sprintf(msgname, "voice-%d-%ld", getId(), id);
	EV << "Generating voice packet  `" << msgname << "'\n";

	EtherAppReq *datapacket = new EtherAppReq(msgname, IEEE802CTRL_DATA);

	datapacket->setRequestId(id);

	long len = frameSize;
	datapacket->setByteLength(len);

	MACAddress voice1 = MyUtil::resolveDestMACAddress(this, "voice1");

	Ieee802Ctrl* etherctrl = new Ieee802Ctrl();
	etherctrl->setSsap(localSAP);
	etherctrl->setDsap(remoteSAP);
	etherctrl->setDest(isTriplePlayGeneratorWithONUs ? voice1 : FiWiTrafGen::dmsAddress);
	etherctrl->setSrc(myAddress);
	datapacket->setControlInfo(etherctrl);

	EtherFrame* f = convertToEtherFrame(datapacket);

	f->setPktType(FIWI_TRAF_PKT_TYPE_VOICE);

	if (isTriplePlayGeneratorWithONUs)
	{
		f->setHost(etherctrl->getDest().str().c_str());
	}
	else
	{
		f->setHost(etherctrl->getSrc().str().c_str());
	}

	f->setIsDOS(isDOS);

	send(f, "out");
}



bool FiWiTrafGen::handleVoiceMessage(cMessage *msg)
{
	double MEAN_FRAME_SIZE = 160;

	// CBR
	// G.711, 64 Kbps, 160 bytes per packet,
	// http://www.cisco.com/en/US/tech/tk652/tk698/technologies_tech_note09186a0080094ae2.shtml
	double DATA_RATE = 64000;
	int LAMBDA = DATA_RATE / (MEAN_FRAME_SIZE * 8.0);

	bool handled = false;
	int indexToDelete = -1;

	for (int i = 0; i < (int)voiceConfs.size(); ++i)
	{
		if (msg == voiceConfs[i].nextMessage)
		{
			int frameSize = MEAN_FRAME_SIZE;

			sendVoiceFrame(frameSize);

			double nextMsg = simTime().dbl() + 1.0 / MyUtil::randomPoisson(LAMBDA);
			scheduleAt(nextMsg, voiceConfs[i].nextMessage);

			handled = true;
		}
		else
		if (msg == voiceConfs[i].endSession)
		{
			indexToDelete = i;
			handled = true;
			break;
		}
	}

	if (indexToDelete >= 0)
	{
		voiceConfs.erase(voiceConfs.begin() + indexToDelete);
	}

	return handled;
}

void FiWiTrafGen::stopCharging(const std::string& nodeId, const std::string& from)
{
	if (OpenDSS::Instance().getPEVStartTime(nodeId) == -1)
		return; // Not charging

	OpenDSS::Instance().setNbPEVsAt(nodeId, 0);

	updateStateOfCharge(nodeId);
	UpdateSOCEvents.log(simTime().dbl(), OpenDSS::Instance().getStateOfChargeOf(nodeId), nodeId + " stop charging from " + from);

	OpenDSS::Instance().setPEVStartTime(nodeId, -1);

	statsNbPEVsCharging.log(simTime().dbl(), OpenDSS::Instance().totalPEVsCharging(), this->getFullPath());

	// recall calculator
	OpenDSS::Instance().recalculateWithOpenDSS();
	OpenDSS::Instance().updatePowerSystemStats(simTime().dbl());
}

void FiWiTrafGen::startCharging(const std::string& nodeId)
{
	if (OpenDSS::Instance().getPEVStartTime(nodeId) > 0)
		return; // Already charging

	OpenDSS::Instance().setNbPEVsAt(nodeId, 1);

	statsNbPEVsCharging.log(simTime().dbl(), OpenDSS::Instance().totalPEVsCharging(), this->getFullPath());

	OpenDSS::Instance().setPEVStartTime(nodeId, simTime().dbl());
	 static RawStat L = RawStat("charging", "event", 1);
	 L.log(simTime().dbl(), 1, nodeId);
		 L.log(simTime().dbl(), 0, " startcharg ");
		 L.log(simTime().dbl(), OpenDSS::Instance().nbPEVsAt(nodeId), " startcharg ");

	// recall calculator
	OpenDSS::Instance().recalculateWithOpenDSS();

	OpenDSS::Instance().updatePowerSystemStats(simTime().dbl());
}

void FiWiTrafGen::startDischarging(const std::string& nodeId)
{
	// It's already discharging:
	if (OpenDSS::Instance().getPEVDischargeStartTime(nodeId) > 0)
		return;

	OpenDSS::Instance().setNbPEVsDischargingAt(nodeId, 1);
	OpenDSS::Instance().setPEVDischargeStartTime(nodeId, simTime().dbl());
	 static RawStat L = RawStat("discharging", "event", 1);
	 L.log(simTime().dbl(), 1, nodeId);
	 L.log(simTime().dbl(), 0, " startdischarg ");
	 L.log(simTime().dbl(), OpenDSS::Instance().nbPEVsDischargingAt(nodeId), " startdischarg ");

	OpenDSS::Instance().recalculateWithOpenDSS();
	OpenDSS::Instance().updatePowerSystemStats(simTime().dbl());
}

void FiWiTrafGen::stopDischarging(const std::string& nodeId)
{
	// Not discharging:
	if (OpenDSS::Instance().getPEVDischargeStartTime(nodeId) == -1)
		return;

	updateDischarge(nodeId);

	OpenDSS::Instance().setNbPEVsDischargingAt(nodeId, 0);
	OpenDSS::Instance().setPEVDischargeStartTime(nodeId, -1);

	OpenDSS::Instance().recalculateWithOpenDSS();
	OpenDSS::Instance().updatePowerSystemStats(simTime().dbl());
}

void FiWiTrafGen::updateDischarge(const std::string& nodeId)//for stop discharging function
{
	double kWInit = (OpenDSS::Instance().getStateOfChargeOf(nodeId)) * this->PEVBatteryCapacity;

	double kWDischarged = ((simTime().dbl() - OpenDSS::Instance().getPEVDischargeStartTime(nodeId)) / (double)OpenDSS::Instance().NbSecondsPerHour) * PEVBatteryKwPerHour;
	//double kWDischarged = 1.44;
	double kW = kWInit - kWDischarged;

	if (kW > PEVBatteryCapacity)
		kW = PEVBatteryCapacity;


	if (kW < 0)
		kW = 0;

	double soc = kW / PEVBatteryCapacity;

	static RawStat stats = RawStat("SOCUpdateDischarge", "event", 1);
	stats.log(simTime().dbl(), soc, nodeId);

	OpenDSS::Instance().setStateOfChargeOf(nodeId, soc);
	OpenDSS::Instance().setPEVDischargeStartTime(nodeId, simTime().dbl());
}

void FiWiTrafGen::startChargingAtChargingStation(const std::string& nodeId)
{
	string nodeIdChargingStation = OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress);

	OpenDSS::Instance().setPEVBatteryKwHPerHourChargingStation(nodeIdChargingStation, this->PEVBatteryKwPerHour);

	OpenDSS::Instance().setNbPEVsAtChargingStation(nodeIdChargingStation, OpenDSS::Instance().nbPEVsAtChargingStation(nodeIdChargingStation) + 1);

	OpenDSS::Instance().setPEVStartTime(nodeId, simTime().dbl());

	statsNbPEVsCharging.log(simTime().dbl(), OpenDSS::Instance().totalPEVsCharging(), this->getFullPath());

	// recall calculator
	OpenDSS::Instance().recalculateWithOpenDSS();

	OpenDSS::Instance().updatePowerSystemStats(simTime().dbl());
}

bool FiWiTrafGen::handleStartChargingAtChargingStation(cMessage* msg)
{
	for (std::map<std::string, PEVPlanning>::iterator it = OpenDSS::Instance().pevPlannings.begin(); it != OpenDSS::Instance().pevPlannings.end(); ++it)
	{
		if (it->second.msgStartChargingAtChargingStation == msg)
		{
			// START CHARGING AT CHARGING STATION:
			startChargingAtChargingStation(it->first);

			cMessage* mEnd = new cMessage("pev_end_charging", 101);

			scheduleAt(OpenDSS::Instance().pevPlannings[it->first].stopChargingAtChargingStationAt, mEnd);
			PEVsEndChargingStation.push_back(mEnd);
			return true;
		}
	}

	return false;
}

void FiWiTrafGen::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
    	EV << "void FiWiTrafGen::handleMessage recv msg " << msg << " at " << simTime().dbl() << endl;

    	if (handleVoiceMessage(msg))
    	{}
    	else
    	if (handleVideoMessage(msg))
    	{}
    	else
    	if (handleDosMessage(msg))
    	{}
    	else
    	if (msg == nextBestEffortMsg)
    	{
    		EV << this->getFullPath() << " NEXT BEST EFFORT..." << endl;
    		sendBestEffortFrame();

    		double bps = 0;

    		for (int i = 0; i < (int)bestEffortConfs.size(); ++i)
    		{
    			if (simTime().dbl() >= bestEffortConfs[i].from && simTime().dbl() < bestEffortConfs[i].to)
    			{
    				bps = bestEffortConfs[i].bps * bestEffortProfileMultiplier;
    				break;
    			}
    		}

    		int MIN_PACKET_SIZE = 1000;
			int MAX_PACKET_SIZE = 1492;
			int MEAN_PACKET_SIZE = (MAX_PACKET_SIZE - MIN_PACKET_SIZE) / 2 + MIN_PACKET_SIZE;

    		if (bps > 0)
    		{
    			double len = MEAN_PACKET_SIZE * 8.0;

    			double lambda = bps / len;

    			double waitTime = 1.0 / MyUtil::randomPoisson(lambda);
    			EV << "FIWI traf, lambda = " << lambda << " wait time = " << waitTime << endl;
    			scheduleAt(simTime() + waitTime, nextBestEffortMsg);
    		}
    	}
    	else
		if (string(msg->getName()) == string(GENERAL_CONF_NAME_MSG_BASE_LOAD_UPDATE))
		{
			bool atLeastOneLoadModification = false;
			bool shouldSendNotificationPacket = false;
			static RawStat stats = RawStat("loadUpdate", "event", 1);
			static RawStat stats2 = RawStat("loadUpdateEventAt", "event", 1);
			stats2.log(simTime().dbl(), simTime().dbl(), string(msg->getName()));
			stats2.log(simTime().dbl(), simTime().dbl(), string(GENERAL_CONF_NAME_MSG_BASE_LOAD_UPDATE));

			for (int i = 0; i < (int)baseLoads.size(); ++i)
			{
				double curTime = simTime().dbl();//ceilf(simTime().dbl() * 100) / 100;

				if ((int)baseLoads[i].time == (int)(((curTime - 2) / HybridNbSecondsPerHour)) % 24)
				{
					// FOR EVENT-BASED, USE THIS CONDITION !!!
					//  && baseLoads[i].powerNodeId == OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress)

					// Change the current load at the specified node:
					// Multiply by 2 kW, the load is a number between 0 and 1
					double currentBaseLoad = OpenDSS::Instance().getCurrentLoadOf(myAddress);
					OpenDSS::Instance().rawBaseLoadUpdate(baseLoads[i].powerNodeId, baseLoads[i].load * 2.0);
					double newBaseLoad = OpenDSS::Instance().getCurrentLoadOf(myAddress);
					EV << "FiWiTrafGen::handleMessage - changing node " << baseLoads[i].powerNodeId << " at "
							<< baseLoads[i].time << " to " << baseLoads[i].load << endl;

					if (fabs(currentBaseLoad - newBaseLoad) >= OpenDSS::Instance().getReactiveControlSensorThreshold() &&
							OpenDSS::Instance().getReactiveControlSensorType() == "eventBased")
					{
						shouldSendNotificationPacket = true;
					}

					atLeastOneLoadModification = true;
				}
			}

			if (atLeastOneLoadModification)
			{
				EV << "FiWiTrafGen::handleMessage - Recalculating with OPEN DSS.";
				stats.log(simTime().dbl(), simTime().dbl(), this->getFullPath());
				OpenDSS::Instance().recalculateWithOpenDSS();
				OpenDSS::Instance().updatePowerSystemStats(simTime().dbl());
			}

			if (shouldSendNotificationPacket)
			{
				sendPEVNotificationPacket(dmsAddress);
			}

			delete msg;
		}
    	else
    	if (handleStartChargingAtChargingStation(msg))
    	{}
    	else
    	if (find(PEVsArrivalsChargingStation.begin(), PEVsArrivalsChargingStation.end(), msg) != PEVsArrivalsChargingStation.end())
    	{
    		PEVsArrivalsChargingStation.erase(find(PEVsArrivalsChargingStation.begin(), PEVsArrivalsChargingStation.end(), msg));
    		delete msg;

    		for (std::map<std::string, PEVPlanning>::iterator it = OpenDSS::Instance().pevPlannings.begin(); it != OpenDSS::Instance().pevPlannings.end(); ++it)
    		{
    			double arrivalWork = (double)((int)(it->second.arrivalTimeAtWork * 10000.0)) / 10000.0;
    			double currentTime = (double)((int)(simTime().dbl() * 10000.0)) / 10000.0;

    			if (arrivalWork == currentTime && ! it->second.arrivalTimeAtWorkProcessed)
    			{
    				it->second.arrivalTimeAtWorkProcessed = true;

    				string nodeId = it->first;

    				updateSOCWithDistance(nodeId, it->second.distanceToWork);

					if (DMS::Instance().chargingStationSchedulingAlgorithm == "randomCharging")
					{
						startChargingAtChargingStation(nodeId);

						// schedule the end also !
						double kWToCharge = (1.0 - OpenDSS::Instance().getStateOfChargeOf(nodeId)) * this->PEVBatteryCapacity;
						double endCharge = simTime().dbl() + (kWToCharge / this->PEVBatteryKwPerHour) * (double)OpenDSS::Instance().NbSecondsPerHour;

						if (endCharge > it->second.departureTimeFromWork)
						{
							endCharge = it->second.departureTimeFromWork;
						}

						it->second.stopChargingAtChargingStationAt = endCharge;

						cMessage* mEnd = new cMessage("pev_end_charging", 101);

						scheduleAt(endCharge, mEnd);
						PEVsEndChargingStation.push_back(mEnd);
					}
					else
					if (DMS::Instance().chargingStationSchedulingAlgorithm == "solarOptimized")
					{
						// Need to send request to the DMS
						sendChargingDeadlineRequest(false, false, nodeId, OpenDSS::Instance().pevPlannings[nodeId].departureTimeFromWork);
					}
    			}
    		}
    	}
    	else
    	if (find(PEVsEndChargingStation.begin(), PEVsEndChargingStation.end(), msg) != PEVsEndChargingStation.end())
    	{
    		PEVsEndChargingStation.erase(find(PEVsEndChargingStation.begin(), PEVsEndChargingStation.end(), msg));
    		delete msg;

    		for (std::map<std::string, PEVPlanning>::iterator it = OpenDSS::Instance().pevPlannings.begin(); it != OpenDSS::Instance().pevPlannings.end(); ++it)
			{
    			double stoppingChargingAt = (double)((int)(it->second.stopChargingAtChargingStationAt * 10000.0)) / 10000.0;
    			double currentTime = (double)((int)(simTime().dbl() * 10000.0)) / 10000.0;

				if (stoppingChargingAt == currentTime && ! it->second.stopChargingAtChargingStationAtProcessed)
				{
					it->second.stopChargingAtChargingStationAtProcessed = true;

					string nodeId = it->first;

					string nodeIdChargingStation = OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress);

					OpenDSS::Instance().setNbPEVsAtChargingStation(nodeIdChargingStation, OpenDSS::Instance().nbPEVsAtChargingStation(nodeIdChargingStation) - 1);

					statsNbPEVsCharging.log(simTime().dbl(), OpenDSS::Instance().totalPEVsCharging(), this->getFullPath());

					// recall calculator
					OpenDSS::Instance().recalculateWithOpenDSS();

					OpenDSS::Instance().updatePowerSystemStats(simTime().dbl());

					this->updateStateOfCharge(nodeId);
					UpdateSOCEvents.log(simTime().dbl(), OpenDSS::Instance().getStateOfChargeOf(nodeId), nodeId + " end charging station ");
					OpenDSS::Instance().setPEVStartTime(nodeId, -1);
				}
			}
    	}
    	else
    	if (find(PEVArrivalMsg.begin(), PEVArrivalMsg.end(), msg) != PEVArrivalMsg.end())
    	{
    		static RawStat stats = RawStat("PEVArrivalMsg", "event", 1);

    		PEVArrivalTime = simTime().dbl();

    		vector<cMessage*>::iterator it = find(PEVArrivalMsg.begin(), PEVArrivalMsg.end(), msg);
			PEVArrivalMsg.erase(it);
			delete msg;

			string nodeId = OpenDSS::Instance().getPowerNodeFromMACAddress(myAddress);

			// Charging
			if ( ! withChargingStations)
			{
				updateSOCWithDistance(nodeId, OpenDSS::Instance().getPEVPlanningOf(nodeId).distanceToWork * 2.0);
			}
			else
			{
				updateSOCWithDistance(nodeId, OpenDSS::Instance().getPEVPlanningOf(nodeId).distanceToWork);
			}

			sendAuthentication();

    		stats.addStat(simTime().dbl(), 1, 0);
    	}
    	else
    	if (msg == V2GBeginMsg)
    	{
    		string nodeId = OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress);

    		if (OpenDSS::Instance().V2GThresholdCoefficient < 1 &&
    				simTime().dbl() >= OpenDSS::Instance().getPEVPlanningOf(nodeId).arrivalTimeAtHome)
    		{
    			stopCharging(nodeId, " v2gBeginMsg ");
    			stopDischarging(nodeId);

    			sendChargingDeadlineRequest(false, false, nodeId, OpenDSS::Instance().pevPlannings[nodeId].deadline);
    		}
    	}
    	else
		if (find(PEVStartDischargingMsg.begin(), PEVStartDischargingMsg.end(), msg) != PEVStartDischargingMsg.end())
		{
			PEVStartDischargingMsg.erase(find(PEVStartDischargingMsg.begin(), PEVStartDischargingMsg.end(), msg));
			delete msg;

			string nodeId = OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress);

			OpenDSS::Instance().setPEVBatteryKwHPerHour(nodeId, this->PEVBatteryKwPerHour);

			startDischarging(nodeId);
		}
		else
		if (find(PEVEndDischargingMsg.begin(), PEVEndDischargingMsg.end(), msg) != PEVEndDischargingMsg.end())
		{
			PEVEndDischargingMsg.erase(find(PEVEndDischargingMsg.begin(), PEVEndDischargingMsg.end(), msg));
			delete msg;

			string nodeId = OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress);

			stopDischarging(nodeId);
		}
		else
    	if (find(PEVStartChargingMsg.begin(), PEVStartChargingMsg.end(), msg) != PEVStartChargingMsg.end())
    	{
    		PEVStartChargingMsg.erase(find(PEVStartChargingMsg.begin(), PEVStartChargingMsg.end(), msg));
    		delete msg;

    		string nodeId = OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress);

    		if (OpenDSS::Instance().nbPEVsDischargingAt(nodeId) == 0)
    		{
				double loadBefore = OpenDSS::Instance().getCurrentLoadOf(myAddress);

				startCharging(nodeId);

				double loadAfter = OpenDSS::Instance().getCurrentLoadOf(myAddress);

				if (fabs(loadBefore - loadAfter) >= OpenDSS::Instance().getReactiveControlSensorThreshold() &&
						OpenDSS::Instance().getReactiveControlSensorType() == "eventBased")
				{
					sendPEVNotificationPacket(dmsAddress);
				}

				// schedule state of charge msg
				cMessage* soc = new cMessage("stateOfCharge", 101);
				scheduleAt(simTime(), soc);
				PEVStateOfChargeMsg.push_back(soc);
    		}
    	}
    	else
    	if (find(PEVEndChargingMsg.begin(), PEVEndChargingMsg.end(), msg) != PEVEndChargingMsg.end())
    	{
    		PEVEndChargingMsg.erase(find(PEVEndChargingMsg.begin(), PEVEndChargingMsg.end(), msg));
    		delete msg;

    		string nodeId = OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress);

    		if (OpenDSS::Instance().nbPEVsDischargingAt(nodeId) == 0)
    		{
				double loadBefore = OpenDSS::Instance().getCurrentLoadOf(myAddress);
				EV << "FiWiTrafGen::handleMessage END CHARGING" << endl;

				stopCharging(nodeId, "end charging");

				double loadAfter = OpenDSS::Instance().getCurrentLoadOf(myAddress);

				if (fabs(loadBefore - loadAfter) >= OpenDSS::Instance().getReactiveControlSensorThreshold() &&
						OpenDSS::Instance().getReactiveControlSensorType() == "eventBased")
				{
					sendPEVNotificationPacket(dmsAddress);
				}
    		}
    	}
    	else
    	if (find(PEVStateOfChargeMsg.begin(), PEVStateOfChargeMsg.end(), msg) != PEVStateOfChargeMsg.end())
    	{
    		PEVStateOfChargeMsg.erase(find(PEVStateOfChargeMsg.begin(), PEVStateOfChargeMsg.end(), msg));
    		delete msg;

    		string nodeId = OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress);

    		EV << "FiWiTrafGen::handleMessage state of charge of " << nodeId << endl;

    		sendStateOfCharge();

    		EV << "FiWiTrafGen::handleMessage current state of charge " << OpenDSS::Instance().getStateOfChargeOf(nodeId) << endl;

    		if (OpenDSS::Instance().getStateOfChargeOf(nodeId) < 1)
    		{
    			cMessage* soc = new cMessage("stateOfCharge", 102);
    			scheduleAt(simTime() + (1.0 / PEVLambdaStateOfChargeMsg), soc);
    			PEVStateOfChargeMsg.push_back(soc);
    		}
    	}
    	else
    	{
			for (int i = 0; i < (int)myTrafficMatrix.size(); ++i)
			{
				if (msg == myTrafficMatrix[i].nextPacketMsg)
				{
					// Electric node:
					if (applicationType == "residentialPEV" || applicationType == "basicElectricNode")
					{
						EV << "PEV sending message ! " << endl;
						sendPEVNotificationPacket(myTrafficMatrix[i].dest);
					}
					else
					if (applicationType == "substation")
					{
						EV << "substation sending message ! " << endl;
						sendSubstationNotificationPacket(myTrafficMatrix[i]);
					}
					else
					// Normal (generic) traffic
					if (applicationType == "normal")
					{
						if (FiWiGeneralConfigs::aggregationTresholdBytes > 0)
						{
							int nb = FiWiGeneralConfigs::aggregationTresholdBytes / reqLength->longValue();

							if (nb < 1)
								nb = 1;

							EV << "tititest, nb=" << nb << endl;

							for (int n = 0; n < nb; ++n)
							{
								sendPacket(myTrafficMatrix[i]);
							}
						}
						else
						{
							EV << "tititest2, nb=" << endl;
							sendPacket(myTrafficMatrix[i]);
						}
					}

					EV << "poisson = " << " lambda = " << myTrafficMatrix[i].lambda << " " << " myTrafficMatrix[i].waitTime() = " << myTrafficMatrix[i].waitTime() << endl;
					EV << "poisson = " << " lambda = " << myTrafficMatrix[i].lambda << " " << " myTrafficMatrix[i].waitTime() = " << myTrafficMatrix[i].waitTime() << endl;
					EV << "poisson = " << " lambda = " << myTrafficMatrix[i].lambda << " " << " myTrafficMatrix[i].waitTime() = " << myTrafficMatrix[i].waitTime() << endl;
					EV << "poisson = " << " lambda = " << myTrafficMatrix[i].lambda << " " << " myTrafficMatrix[i].waitTime() = " << myTrafficMatrix[i].waitTime() << endl;
					EV << "poisson = " << " lambda = " << myTrafficMatrix[i].lambda << " " << " myTrafficMatrix[i].waitTime() = " << myTrafficMatrix[i].waitTime() << endl;
					EV << "poisson = " << " lambda = " << myTrafficMatrix[i].lambda << " " << " myTrafficMatrix[i].waitTime() = " << myTrafficMatrix[i].waitTime() << endl;
					EV << "poisson = " << " lambda = " << myTrafficMatrix[i].lambda << " " << " myTrafficMatrix[i].waitTime() = " << myTrafficMatrix[i].waitTime() << endl;
					EV << "poisson = " << " lambda = " << myTrafficMatrix[i].lambda << " " << " myTrafficMatrix[i].waitTime() = " << myTrafficMatrix[i].waitTime() << endl;
					EV << "poisson = " << " lambda = " << myTrafficMatrix[i].lambda << " " << " myTrafficMatrix[i].waitTime() = " << myTrafficMatrix[i].waitTime() << endl;
					EV << "poisson = " << " lambda = " << myTrafficMatrix[i].lambda << " " << " myTrafficMatrix[i].waitTime() = " << myTrafficMatrix[i].waitTime() << endl;

					scheduleAt(simTime() + myTrafficMatrix[i].waitTime(), msg);
				}
			}
    	}
    }
    else
    {
    	EtherFrame* frame = dynamic_cast<EtherFrame*>(msg);

    	if (frame->getDest() == this->myAddress)
    		receivePacket(msg);
    	else
    		delete msg;
    }
}

void FiWiTrafGen::sendPacket(TrafficPair& traf)
{
    seqNum++;

    char msgname[30];
    sprintf(msgname, "req-%d-%ld", getId(), seqNum);
    EV << "Generating packet (" << reqLength->longValue() << " bytes) `" << msgname << "'\n";

    EV << "DEST -> " << traf.dest << endl;

    EtherAppReq *datapacket = new EtherAppReq(msgname, IEEE802CTRL_DATA);

    datapacket->setRequestId(seqNum);

    long len = reqLength->longValue();
    datapacket->setByteLength(len);

    Ieee802Ctrl* etherctrl = new Ieee802Ctrl();
    etherctrl->setSsap(localSAP);
    etherctrl->setDsap(remoteSAP);
    etherctrl->setDest(traf.dest);
    etherctrl->setSrc(myAddress);
    datapacket->setControlInfo(etherctrl);

    EtherFrame* f = convertToEtherFrame(datapacket, traf);
    f->setInternalTimestamp(simTime().raw());
    f->setTrafficClass(traf.trafficClass);
    EV << "SENDING TRAF CLASS " << traf.trafficClass << endl;
    f->setSentAt(simTime().raw());

    if (simTime().dbl() >= 2)
    {
    	FiWiGeneralConfigs::OfferedLoadSum += len * 8;

    	FiWiGeneralConfigs::StatsTrafficClass[traf.trafficClass].sumOfferedLoads += len * 8;
    	FiWiGeneralConfigs::StatsTrafficClass[traf.trafficClass].cntOfferedLoads += 1;

    	FiWiGeneralConfigs::StatsStationTraffic[traf.trafficClass][myAddress].sumOfferedLoads += len * 8;
    	FiWiGeneralConfigs::StatsStationTraffic[traf.trafficClass][myAddress].cntOfferedLoads += 1;
    }

    send(f, "out");
    packetsSent++;
}

void FiWiTrafGen::updateReceiveStats(EtherFrame* frame, cPacket* pkt, RawStat& throughput, RawStat& delay, RawStat& throughputVideo, RawStat& delayVideo,
		RawStat& throughputVoice, RawStat& delayVoice, RawStat& throughputBestEffort, RawStat& delayBestEffort,
		RawStat& throughputSmartGridControl, RawStat& delaySmartGridControl,
		RawStat& throughputSmartGridNotification, RawStat& delaySmartGridNotification,
		RawStat& throughputSmartGridControlDOS, RawStat& delaySmartGridControlDOS,
		RawStat& throughputSmartGridNotificationDOS, RawStat& delaySmartGridNotificationDOS)
{
	EV << "FiWiTrafGen::updateReceiveStats 1 " << endl;
	if (pkt)
	{
		EV << "FiWiTrafGen::updateReceiveStats 2 " << endl;
		double curTime = simTime().dbl();
		throughput.addStat(curTime, pkt->getBitLength(), 0);
		delay.addStat(curTime, (curTime - pkt->getCreationTime().dbl()), 0);
		EV << "DMS ----- RECEIVED" << endl;

		EV << "FiWiTrafGen::updateReceiveStats 3 pkt type =  " << frame->getPktType() << endl;

		if (frame->getPktType() == FIWI_TRAF_PKT_TYPE_VIDEO)
		{
			throughputVideo.addStat(curTime, pkt->getBitLength(), 0);
			delayVideo.addStat(curTime, (curTime - pkt->getCreationTime().dbl()), 0);
		}
		else
		if (frame->getPktType() == FIWI_TRAF_PKT_TYPE_VOICE)
		{
			throughputVoice.addStat(curTime, pkt->getBitLength(), 0);
			delayVoice.addStat(curTime, (curTime - pkt->getCreationTime().dbl()), 0);
		}
		else
		if (frame->getPktType() == FIWI_TRAF_PKT_TYPE_BEST_EFFORT)
		{
			throughputBestEffort.addStat(curTime, pkt->getBitLength(), 0);
			delayBestEffort.addStat(curTime, (curTime - pkt->getCreationTime().dbl()), 0);
		}
		else
		if (frame->getPktType() == FIWI_TRAF_PKT_TYPE_SMART_GRID_CONTROL && ! frame->getIsDOS())
		{
			throughputSmartGridControl.addStat(curTime, pkt->getBitLength(), 0);
			delaySmartGridControl.addStat(curTime, (curTime - pkt->getCreationTime().dbl()), 0);
		}
		else
		if (frame->getPktType() == FIWI_TRAF_PKT_TYPE_SMART_GRID_CONTROL && frame->getIsDOS())
		{
			throughputSmartGridControlDOS.addStat(curTime, pkt->getBitLength(), 0);
			delaySmartGridControlDOS.addStat(curTime, (curTime - pkt->getCreationTime().dbl()), 0);
		}
		else
		if (frame->getPktType() == FIWI_TRAF_PKT_TYPE_SMART_GRID_NOTIFICATION && ! frame->getIsDOS())
		{
			throughputSmartGridNotification.addStat(curTime, pkt->getBitLength(), 0);
			delaySmartGridNotification.addStat(curTime, (curTime - pkt->getCreationTime().dbl()), 0);
		}
		else
		if (frame->getPktType() == FIWI_TRAF_PKT_TYPE_SMART_GRID_NOTIFICATION && frame->getIsDOS())
		{
			throughputSmartGridNotificationDOS.addStat(curTime, pkt->getBitLength(), 0);
			delaySmartGridNotificationDOS.addStat(curTime, (curTime - pkt->getCreationTime().dbl()), 0);
		}
	}
}

void FiWiTrafGen::receivePacket(cMessage *msg)
{




    EV << "Received packet `" << msg->getName() << " applicationType = " << applicationType << "'\n";

    packetsReceived++;
    simtime_t lastEED = simTime() - msg->getCreationTime();
    eedVector.record(lastEED);
    eedStats.collect(lastEED);

    EtherFrame* frame = dynamic_cast<EtherFrame*>(msg);
    frame->setByteLength(1500);
    EV << "FiWiTrafGen::receivePacket - size = " << frame->getByteLength() << endl;

    simtime_t timeSent;

    timeSent.setRaw(simTime().raw() - frame->getSentAt());
    int trafficClass = frame->getTrafficClass();

    cPacket* pkt = dynamic_cast<cPacket*>(msg)->decapsulate();

    EV << "pkt size in bits = " << pkt->getBitLength() << endl;

    if (simTime().dbl() > 2.0)
    {
		// Update FiWi throughput
		FiWiGeneralConfigs::updateThroughput(pkt->getBitLength());
		// End update FiWi throughput

		// Class based
		FiWiGeneralConfigs::StatsTrafficClass[trafficClass].sumThroughputs += pkt->getBitLength();
		FiWiGeneralConfigs::StatsTrafficClass[trafficClass].cntThroughputs += 1;
		FiWiGeneralConfigs::StatsTrafficClass[trafficClass].sumDelays += timeSent.dbl();
		FiWiGeneralConfigs::StatsTrafficClass[trafficClass].cntDelays += 1;

		FiWiGeneralConfigs::StatsStationTraffic[trafficClass][frame->getSrc()].sumThroughputs += pkt->getBitLength();
		FiWiGeneralConfigs::StatsStationTraffic[trafficClass][frame->getSrc()].cntThroughputs += 1;
		FiWiGeneralConfigs::StatsStationTraffic[trafficClass][frame->getSrc()].sumDelays += timeSent.dbl();
		FiWiGeneralConfigs::StatsStationTraffic[trafficClass][frame->getSrc()].cntDelays += 1;
    }

    if (applicationType != "dms")
    {
		EV << "app type diff dms .. " << endl;
    	updateReceiveStats(frame, pkt, DownstreamThroughput, DownstreamDelay, DownstreamThroughputVideo, DownstreamDelayVideo,
    		DownstreamThroughputVoice, DownstreamDelayVoice, DownstreamThroughputBestEffort, DownstreamDelayBestEffort,
    		DownstreamThroughputSmartGridControl, DownstreamDelaySmartGridControl,
    		DownstreamThroughputSmartGridNotification, DownstreamDelaySmartGridNotification,
    		DownstreamThroughputSmartGridControl, DownstreamDelaySmartGridControl, // NO DOS
    		DownstreamThroughputSmartGridNotification, DownstreamDelaySmartGridNotification); // NO DOS
    }

    if (applicationType == "dms")
    {
    	updateReceiveStats(frame, pkt, DMSThroughput, DMSDelay, DMSThroughputVideo, DMSDelayVideo,
    			DMSThroughputVoice, DMSDelayVoice, DMSThroughputBestEffort, DMSDelayBestEffort,
    			DMSThroughputSmartGridControl, DMSDelaySmartGridControl,
    			DMSThroughputSmartGridNotification, DMSDelaySmartGridNotification,
    			DMSThroughputSmartGridControlDOS, DMSDelaySmartGridControlDOS,
    			DMSThroughputSmartGridNotificationDOS, DMSDelaySmartGridNotificationDOS);

    	if (pkt && dynamic_cast<NodeNotificationMessage*>(pkt))
    	{
    		static RawStat stats = RawStat("NodeNotificationMessageLOG", "event", 1);
    		EV << "FiWiTrafGen::receivePacket - PEVNotificationMessage" << endl;

    		// Notification packet !
    		NodeNotificationMessage* notificationMsg = dynamic_cast<NodeNotificationMessage*>(pkt);

    		EV << "pretty length = " << notificationMsg->getByteLength() << endl;

    		// Update local knowledge:
    		DMS::Instance().updateVoltage(notificationMsg->getNodeId(), notificationMsg->getVoltage());
    		DMS::Instance().updateLoad(notificationMsg->getNodeId(), notificationMsg->getLoad());


    		vector<string> nodesToDeactivate =  DMS::Instance().execReactiveControl(simTime().dbl());

    		stats.log(simTime().dbl(), 1, "after check to deactivate...");

    		for (int i = 0; i < (int)nodesToDeactivate.size(); ++i)
    		{
    			stats.log(simTime().dbl(), 1, nodesToDeactivate[i]);
    			MACAddress addr = OpenDSS::Instance().getMACAddressFromPowerNode(nodesToDeactivate[i]);
    			stats.log(simTime().dbl(), 1, addr.str());
    			sendPEVControlMessage(addr, nodesToDeactivate[i], "", "");
    		}
    	}
    	else
    	if (pkt && dynamic_cast<SubstationNotificationMessage*>(pkt))
    	{
    		EV << "FiWiTrafGen::receivePacket - SubstationNotificationMessage" << endl;

    		SubstationNotificationMessage* notificationMsg = dynamic_cast<SubstationNotificationMessage*>(pkt);

    		DMS::Instance().updateTotalLoad(notificationMsg->getTotalPowerLoad());
    		DMS::Instance().updateTotalLosses(notificationMsg->getTotalPowerLosses());

    		EV << "FiWiTrafGen::receivePacket - SubstationNotificationMessage total load = " << notificationMsg->getTotalPowerLoad() << " total losses = " << notificationMsg->getTotalPowerLosses() << endl;
    	}
    	else
    	if (pkt && dynamic_cast<ChargingDeadlineRequestMessage*>(pkt))
    	{
    		static RawStat stats = RawStat("ChargingDeadlineRequestMessageDMS", "event", 1);
    		EV << "FiWiTrafGen::receivePacket - ChargingDeadlineRequestMessage" << endl;

    		EtherFrame* notifFrame = dynamic_cast<EtherFrame*>(msg);

    		if (notifFrame)
    		{
    			EV << "OK for frame" << endl;
				ChargingDeadlineRequestMessage* notificationMsg = dynamic_cast<ChargingDeadlineRequestMessage*>(pkt);

				double kWPerHour = notificationMsg->getBatteryKwPerHour();

				DMS::Instance().updatePEVkWH(notificationMsg->getNodeId(), kWPerHour);
				DMS::Instance().updateLoad(notificationMsg->getNodeId(), notificationMsg->getBaseLoad());

				// We add 1 second for the transmission of the packet.
				vector<PEVSchedule> schedules = DMS::Instance().schedulePEV(notificationMsg->getNodeId(), simTime().dbl() + 1, DMSSchedulingAlgorithm,
						notificationMsg->getDeadline(), notificationMsg->getBatteryCapacity(),
						notificationMsg->getBatteryStateOfCharge(), kWPerHour, notificationMsg->getWantV2G());

				// if sched time == -1 -> CAN'T SCHEDULE.

				for (int i = 0; i < (int)schedules.size(); ++i)
				{
					stats.log(simTime().dbl(), schedules[i].startTime, notificationMsg->getNodeId() + string(" start time "));
					stats.log(simTime().dbl(), schedules[i].endTime, notificationMsg->getNodeId() + string(" end time "));
					stats.log(simTime().dbl(), 0, notificationMsg->getNodeId() + string(" TYPE -> ") + schedules[i].type);
					stats.log(simTime().dbl(), notificationMsg->getBatteryStateOfCharge(), notificationMsg->getNodeId() + string(" soc! "));
				}

				sendChargingDeadlineResponse(notifFrame->getSrc(), notificationMsg->getNodeId(), notificationMsg->getVehicleID(),
						notificationMsg->getCustomerID(), schedules);
    		}
    		else
    		{
    			EV << "not OK for frame" << endl;
    		}
    	}
    	else
    	if (pkt && dynamic_cast<StateOfChargeMessage*>(pkt))
    	{
    		EV << "FiWiTrafGen::receivePacket - StateOfChargeMessage" << endl;

    		static RawStat stats = RawStat("StateOfChargeMessageDMS", "event", 1);
    		stats.addStat(simTime().dbl(), 1, 0);
    	}
    	else
    	if (pkt && dynamic_cast<PEVAuthenticationMessage*>(pkt))
    	{
    		EV << "FiWiTrafGen::receivePacket - PEVAuthenticationMessage" << endl;
    		static RawStat stats = RawStat("PEVAuthenticationMessageDMS", "event", 1);

    		PEVAuthenticationMessage* notificationMsg = dynamic_cast<PEVAuthenticationMessage*>(pkt);

    		stats.addStat(simTime().dbl(), 1, 0);

    		EtherFrame* notifFrame = dynamic_cast<EtherFrame*>(msg);

    		sendAuthenticationResponse(notifFrame->getSrc(), notificationMsg->getNodeId(),
    				notificationMsg->getVehicleID(), notificationMsg->getCustomerID());
    	}
    }
    else
    if (applicationType == "chargingStation")
    {
    	recordStatsPEVPacket(pkt);

		if (pkt && dynamic_cast<ChargingDeadlineResponseMessage*>(pkt))
		{

			ChargingDeadlineResponseMessage* notificationMsg = dynamic_cast<ChargingDeadlineResponseMessage*>(pkt);

			string nodeId = notificationMsg->getNodeId();

			if (notificationMsg->getResultsArraySize() > 1)
			{
				error("Too many results in charging station dms response message");
			}

			for (int i = 0; i < (int)notificationMsg->getResultsArraySize(); ++i)
			{
				// Multiple scheduling slots are possible
				if (notificationMsg->getResults(i).getStatus() == 1)
				{
					// Schedule start:
					double startTime = notificationMsg->getResults(i).getStartTime();
					OpenDSS::Instance().pevPlannings[nodeId].msgStartChargingAtChargingStation = new cMessage("startDisAndCharging", 103);

					// Save the result:
					OpenDSS::Instance().pevPlannings[nodeId].stopChargingAtChargingStationAt = notificationMsg->getResults(i).getEndTime();

					scheduleAt(startTime, OpenDSS::Instance().pevPlannings[nodeId].msgStartChargingAtChargingStation);
				}
			}
		}
    }
    else
    if (applicationType == "residentialPEV")
    {
    	recordStatsPEVPacket(pkt);

    	if (pkt && dynamic_cast<ChargingDeadlineResponseMessage*>(pkt))
		{
    		static RawStat stats = RawStat("ChargingDeadlineResponseMessageDelayPEV", "event", 1);

			EV << "FiWiTrafGen::receivePacket - ChargingDeadlineResponseMessage" << endl;

			// Notification packet !
			double pevChargingDelay = simTime().dbl() - PEVArrivalTime;
			stats.log(simTime().dbl(), pevChargingDelay, this->getFullPath());

			ChargingDeadlineResponseMessage* notificationMsg = dynamic_cast<ChargingDeadlineResponseMessage*>(pkt);

			// Clear current charging events:
			PEVStartChargingMsg.clear();
			PEVStartDischargingMsg.clear();
			PEVEndChargingMsg.clear();
			PEVEndDischargingMsg.clear();
			PEVStateOfChargeMsg.clear();

			string nodeId = OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress);

			stopCharging(nodeId, " charging deadline response msg ");
			stopDischarging(nodeId);

			for (int i = 0; i < (int)notificationMsg->getResultsArraySize(); ++i)
			{
				// Multiple scheduling slots are possible
				if (notificationMsg->getResults(i).getStatus() == 1)
				{
					// Schedule start:
					double startTime = notificationMsg->getResults(i).getStartTime();
					cMessage* msg = new cMessage("startDisAndCharging", 103);

					if (notificationMsg->getResults(i).getType() == string("charging"))
						PEVStartChargingMsg.push_back(msg);
					else
						PEVStartDischargingMsg.push_back(msg);

					scheduleAt(startTime, msg);

					// Schedule end:
					double endTime = notificationMsg->getResults(i).getEndTime();

					cMessage* mEnd = new cMessage("endDisAndCharging", 101);

					if (notificationMsg->getResults(i).getType() == string("charging"))
						PEVEndChargingMsg.push_back(mEnd);
					else
						PEVEndDischargingMsg.push_back(mEnd);

					scheduleAt(endTime, mEnd);
				}
			}
		}
    	else
    	if (pkt && dynamic_cast<PEVAuthenticationResponseMessage*>(pkt))
    	{
    		static RawStat stats = RawStat("PEVAuthenticationResponseMessagePEV", "event", 1);
    		EV << "FiWiTrafGen::receivePacket - PEVAuthenticationResponseMessage" << endl;
    		stats.addStat(simTime().dbl(), 1, 0);

    		string nodeId = OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress);

    		sendChargingDeadlineRequest(true, false, nodeId, OpenDSS::Instance().pevPlannings[nodeId].deadline);
    	}
    	else
    	if (pkt && dynamic_cast<PEVControlMessage*>(pkt))
    	{
    		static RawStat stats = RawStat("PEVControlMessageRcv", "event", 1);

    		// Need to deactivate the PEV.
    		string nodeId = OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress);

    		stats.log(simTime().dbl(), 1, nodeId);

    		updateStateOfCharge(nodeId);
    		UpdateSOCEvents.log(simTime().dbl(), OpenDSS::Instance().getStateOfChargeOf(nodeId), nodeId + " control message ");

			// RESCHED!
			schedPEV(simTime().dbl() + 2);

			OpenDSS::Instance().setNbPEVsAt(nodeId, 0);

			statsNbPEVsCharging.log(simTime().dbl(), OpenDSS::Instance().totalPEVsCharging(), this->getFullPath());

			// recall calculator
			OpenDSS::Instance().recalculateWithOpenDSS();

			OpenDSS::Instance().updatePowerSystemStats(simTime().dbl());

			// Unsched all ENDS.
			for (int i = 0; i < (int)PEVEndChargingMsg.size(); ++i)
			{
				cancelEvent(PEVEndChargingMsg[i]);
			}
    	}
    	else
    	if (pkt && dynamic_cast<PowerSysACKMessage*>(pkt))
    	{
    		EV << "FiWiTrafGen::receivePacket - PowerSysACKMessage" << endl;

    	}
    }

    Ieee802Ctrl* ctl = dynamic_cast<Ieee802Ctrl*>(frame->removeControlInfo());

    if (ctl)
    {
    	delete ctl;
    }

    ctl = dynamic_cast<Ieee802Ctrl*>(pkt->removeControlInfo());

    if (ctl)
    	delete ctl;

    delete frame;
    delete pkt; // Encapsulated packet
}

void FiWiTrafGen::recordStatsPEVPacket(cPacket* pkt)
{
	if (pkt)
	{
		double curTime = simTime().dbl();
		PEVThroughput.addStat(curTime, pkt->getBitLength(), 0);
		// DMSDelay.addStat(simTime.dbl(), (simTime().dbl() - pkt->getCreationTime().dbl()));
		PEVDelay.addStat(curTime, (curTime - pkt->getCreationTime().dbl()), 0);
	}
}

void FiWiTrafGen::finish()
{
    recordScalar("packets sent", packetsSent);
    recordScalar("packets rcvd", packetsReceived);
    recordScalar("end-to-end delay mean", eedStats.getMean());
    recordScalar("end-to-end delay stddev", eedStats.getStddev());
    recordScalar("end-to-end delay min", eedStats.getMin());
    recordScalar("end-to-end delay max", eedStats.getMax());

    if (PEVSet)
    {
		string nodeId = OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress);

		// If it's currently charging and not finished, update state of charge.
		if (OpenDSS::Instance().nbPEVsAt(nodeId) == 1 && OpenDSS::Instance().getPEVStartTime(nodeId) > 0)
			updateStateOfCharge(nodeId);

		statsSOC.log(simTime().dbl(), OpenDSS::Instance().getStateOfChargeOf(nodeId), nodeId);
    }
}

EtherFrame *FiWiTrafGen::convertToEtherFrame(EtherAppReq* frame, TrafficPair traf)
{
    // create a matching ethernet frame
    EtherFrame *ethframe = new EthernetIIFrame(frame->getName()); //TODO option to use EtherFrameWithSNAP instead
    ethframe->setDest((dynamic_cast<Ieee802Ctrl*>(frame->getControlInfo()))->getDest());
    ethframe->setSrc((dynamic_cast<Ieee802Ctrl*>(frame->getControlInfo()))->getSrc());
    //XXX set ethertype

    ethframe->setKind(FIWI_NODE_TYPE_ANY);

    EV << "KIND OF PACKETTTTTTT" << ethframe->getKind() << endl;

    ethframe->encapsulate(frame);

    ethframe->setControlInfo(frame->getControlInfo()->dup());

    ethframe->setIsDOS(isDOS);

    // done
    return ethframe;
}

EtherFrame *FiWiTrafGen::convertToEtherFrame(EtherAppReq* frame)
{
    // create a matching ethernet frame
    EtherFrame *ethframe = new EthernetIIFrame(frame->getName()); //TODO option to use EtherFrameWithSNAP instead
    ethframe->setDest((dynamic_cast<Ieee802Ctrl*>(frame->getControlInfo()))->getDest());
    ethframe->setSrc((dynamic_cast<Ieee802Ctrl*>(frame->getControlInfo()))->getSrc());

    ethframe->setKind(FIWI_NODE_TYPE_ANY);

    ethframe->encapsulate(frame);

    ethframe->setControlInfo(frame->getControlInfo()->dup());

    // done
    return ethframe;
}

void FiWiTrafGen::sendBestEffortFrame()
{
	static long id = 0;
	++id;

	char msgname[300];
	sprintf(msgname, "best-effort-%d-%ld", getId(), id);
	EV << "Generating packet  `" << msgname << " from " << this->getFullPath() << "'\n";

	EtherAppReq *datapacket = new EtherAppReq(msgname, IEEE802CTRL_DATA);

	datapacket->setRequestId(id);

	int MIN_PACKET_SIZE = 1000;
	int MAX_PACKET_SIZE = 1492;

	double length = uniform(MIN_PACKET_SIZE, MAX_PACKET_SIZE);

	datapacket->setByteLength((int)length);

	MACAddress best1 = MyUtil::resolveDestMACAddress(this, "best1");

	Ieee802Ctrl* etherctrl = new Ieee802Ctrl();
	etherctrl->setSsap(localSAP);
	etherctrl->setDsap(remoteSAP);
	etherctrl->setDest(isTriplePlayGeneratorWithONUs ? best1 : FiWiTrafGen::dmsAddress);
	etherctrl->setSrc(myAddress);
	datapacket->setControlInfo(etherctrl);

	EtherFrame* f = convertToEtherFrame(datapacket);

	f->setPktType(FIWI_TRAF_PKT_TYPE_BEST_EFFORT);

	if (isTriplePlayGeneratorWithONUs)
	{
		f->setHost(etherctrl->getDest().str().c_str());
	}
	else
	{
		f->setHost(etherctrl->getSrc().str().c_str());
	}

	f->setIsDOS(isDOS);

	FiWiTrafGen::BestEffortDropRatio.addStat(simTime().dbl(), 1, 0);

	send(f, "out");
}

///////////////////////////////////////////////////////////////////////////////////////
// Power system stuff
//

string FiWiTrafGen::msgBasicNotificationPacket(string nodeId, double voltage, double load)
{
	stringstream inAll (stringstream::in | stringstream::out);
	stringstream inContent (stringstream::in | stringstream::out);

	inContent << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	inContent << "<BasicNodeNotification xmlns=\"http://zigbee.org/sep\">\n";
	inContent << "<NodeID>" << nodeId << "</NodeID>\n";
	inContent << "<Voltage>" << voltage << "</Voltage>\n";
	inContent << "<Load>" << load << "</Load>\n";
	inContent << "</BasicNodeNotification>\n";

	inAll << "POST /distribution_network/basic_node_notification/" << nodeId << " HTTP/1.1\n";
	inAll << "Host: 3ffe:1900:4545:3:200:f8ff:fe21:67cf\n";
	inAll << "Content-Type: application/xml\n";
	inAll << "Cookie : session_id= d131dd02c5e6eec4693d9a0698aff95c";
	inAll << "Content-Length: " << inContent.str().size() << "\n";
	inAll << "\n";
	inAll << inContent.str();

	return inAll.str();
}

void FiWiTrafGen::sendPEVNotificationPacket(MACAddress dest)
{
	static long notificationPacketId = 0;
	++notificationPacketId;

	char msgname[300];
	sprintf(msgname, "pev-notification-%d-%ld", getId(), notificationPacketId);
	EV << "Generating packet (" << reqLength->longValue() << " bytes) `" << msgname << "'\n";

	Ieee802Ctrl* etherctrl = new Ieee802Ctrl();
	etherctrl->setSsap(localSAP);
	etherctrl->setDsap(remoteSAP);
	etherctrl->setDest(dest);
	etherctrl->setSrc(myAddress);

	NodeNotificationMessage* datapacket = new NodeNotificationMessage(msgname, IEEE802CTRL_DATA);

	//
	string nodeId = OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress);
	double voltage = OpenDSS::Instance().getCurrentVoltageOf(this->myAddress);
	double load = OpenDSS::Instance().getCurrentLoadOf(this->myAddress);

	datapacket->setNodeId(nodeId.c_str());
	datapacket->setVoltage(voltage);
	datapacket->setLoad(load);

	string httpMessage = msgBasicNotificationPacket(nodeId, voltage, load);

	long lengthMessage = httpMessage.size() + this->TCPHeaderSize() + this->IPv6HeaderSize();
	datapacket->setByteLength(lengthMessage);

	datapacket->setControlInfo(etherctrl);

	EtherFrame *ethframe = new EthernetIIFrame(msgname);
	ethframe->setDest((dynamic_cast<Ieee802Ctrl*>(datapacket->getControlInfo()))->getDest());
	ethframe->setSrc((dynamic_cast<Ieee802Ctrl*>(datapacket->getControlInfo()))->getSrc());
	ethframe->setKind(FIWI_NODE_TYPE_ANY);
	ethframe->setPktType(FIWI_TRAF_PKT_TYPE_SMART_GRID_NOTIFICATION);
	ethframe->setHost(ethframe->getSrc().str().c_str());

	ethframe->setTrafficClass(1);

	ethframe->setIsDOS(isDOS);

	ethframe->encapsulate(datapacket);

	ethframe->setControlInfo(datapacket->getControlInfo()->dup());

	send(ethframe, "out");
}

string FiWiTrafGen::msgSubstationNotificationPacket(string nodeId, double totalPowerLoad, double totalPowerLosses)
{
	stringstream inAll (stringstream::in | stringstream::out);
	stringstream inContent (stringstream::in | stringstream::out);

	inContent << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	inContent << "<SubstationNotification xmlns=\"http://zigbee.org/sep\">\n";
	inContent << "<NodeID>" << nodeId << "</NodeID>\n";
	inContent << "<TotalPowerLoad>" << totalPowerLoad << "</TotalPowerLoad>\n";
	inContent << "<TotalPowerLosses>" << totalPowerLosses << "</TotalPowerLosses>\n";
	inContent << "</SubstationNotification>\n";

	inAll << "POST /distribution_network/substation_notification/" << nodeId << " HTTP/1.1\n";
	inAll << "Host: 3ffe:1900:4545:3:200:f8ff:fe21:67cf\n";
	inAll << "Content-Type: application/xml\n";
	inAll << "Cookie : session_id=d131dd02c5e6eec4693d9a0698aff95c";
	inAll << "Content-Length: " << inContent.str().size() << "\n";
	inAll << "\n";
	inAll << inContent.str();

	return inAll.str();
}

void FiWiTrafGen::sendSubstationNotificationPacket(TrafficPair traf)
{
	static long substationNotificationPacketId = 0;
	++substationNotificationPacketId;

	char msgname[300];
	sprintf(msgname, "substation-notification-%d-%ld", getId(), substationNotificationPacketId);
	EV << "Generating packet  `" << msgname << "'\n";

	Ieee802Ctrl* etherctrl = new Ieee802Ctrl();
	etherctrl->setSsap(localSAP);
	etherctrl->setDsap(remoteSAP);
	etherctrl->setDest(traf.dest);
	etherctrl->setSrc(myAddress);

	SubstationNotificationMessage* datapacket = new SubstationNotificationMessage(msgname, IEEE802CTRL_DATA);

	string nodeId = "substation"; // not a real ID since we have only one substation.
	double totalPowerLoad = OpenDSS::Instance().getTotalPowerLoad();
	double totalPowerLosses = OpenDSS::Instance().getTotalPowerLosses();

	datapacket->setNodeId(nodeId.c_str());
	datapacket->setTotalPowerLoad(totalPowerLoad);
	datapacket->setTotalPowerLosses(totalPowerLosses);

	string httpMessage = msgSubstationNotificationPacket(nodeId, totalPowerLoad, totalPowerLosses);

	long lengthMessage = httpMessage.size() + this->TCPHeaderSize() + this->IPv6HeaderSize();
	datapacket->setByteLength(lengthMessage);

	datapacket->setControlInfo(etherctrl);

	EtherFrame *ethframe = new EthernetIIFrame(msgname);
	ethframe->setDest((dynamic_cast<Ieee802Ctrl*>(datapacket->getControlInfo()))->getDest());
	ethframe->setSrc((dynamic_cast<Ieee802Ctrl*>(datapacket->getControlInfo()))->getSrc());
	ethframe->setKind(FIWI_NODE_TYPE_ANY);
	ethframe->setPktType(FIWI_TRAF_PKT_TYPE_SMART_GRID_NOTIFICATION);
	ethframe->setHost(ethframe->getSrc().str().c_str());

	ethframe->setTrafficClass(1);

	ethframe->setIsDOS(isDOS);

	ethframe->encapsulate(datapacket);

	ethframe->setControlInfo(datapacket->getControlInfo()->dup());

	send(ethframe, "out");
}

string FiWiTrafGen::msgChargingDeadlineRequestPacket(ChargingDeadlineRequestMessage* msg)
{
	stringstream inAll (stringstream::in | stringstream::out);
	stringstream inContent (stringstream::in | stringstream::out);

	inContent << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	inContent << "<ChargingDeadline xmlns=\"http://zigbee.org/sep\">\n";
	inContent << "<Vehicule>\n";
	inContent << "<NodeID>" << msg->getNodeId() << "</NodeID>\n";
	inContent << "<CustomerID>" << msg->getCustomerID() << "</CustomerID>\n";
	inContent << "<VehiculeID>" << msg->getVehicleID() << "</VehiculeID>\n";
	inContent << "</Vehicule>\n";
	inContent << "<Scheduling>\n";
	inContent << "<Deadline>" << msg->getDeadline() << "</Deadline>\n";
	inContent << "</Scheduling>\n";
	inContent << "<Battery>\n";
	inContent << "<Capacity>" << msg->getBatteryCapacity() << "</Capacity>\n";
	inContent << "<DepthOfDischarge>" << msg->getBatteryDepthOfDischarge() << "</DepthOfDischarge>\n";
	inContent << "<KwPerHour>" << msg->getBatteryKwPerHour() << "</KwPerHour>\n";
	inContent << "<StateOfCharge>" << msg->getBatteryStateOfCharge() << "</StateOfCharge>\n";
	inContent << "</Battery>\n";
	inContent << "</ChargingDeadline>\n";

	inAll << "POST /distribution_network/charging_deadline/" << msg->getNodeId() << " HTTP/1.1\n";
	inAll << "Host: 3ffe:1900:4545:3:200:f8ff:fe21:67cf\n";
	inAll << "Content-Type: application/xml\n";
	inAll << "Cookie : session_id=d131dd02c5e6eec4693d9a0698aff95c";
	inAll << "Content-Length: " << inContent.str().size() << "\n";
	inAll << "\n";
	inAll << inContent.str();

	return inAll.str();
}

void FiWiTrafGen::updateSOCWithDistance(const std::string& nodeId, double distance)
{
	double distanceInMiles = distance;
	double kwHSpent = distanceInMiles * 0.288; // 0.288 = 0.26 / 0.9

	double initSOC = OpenDSS::Instance().getStateOfChargeOf(nodeId);

	double kwInBattery = (initSOC * PEVBatteryCapacity) - kwHSpent;

	if (kwInBattery < 0)
		kwInBattery = 0;

	double newSOC = kwInBattery / PEVBatteryCapacity;

	if (newSOC > 1)
		newSOC = 1;

	if (newSOC < 0)
		newSOC = 0;

	static RawStat stats = RawStat("SOCUpdateWithDistance", "event", 1);
	stats.log(simTime().dbl(), newSOC, nodeId);
	stats.log(simTime().dbl(), initSOC, "initSOC");
	stats.log(simTime().dbl(), kwInBattery, "kwInBattery");
	stats.log(simTime().dbl(), PEVBatteryCapacity, "PEVBatteryCapacity");
	stats.log(simTime().dbl(), OpenDSS::Instance().getPEVPlanningOf(nodeId).distanceToWork, "distance to work");
	stats.log(simTime().dbl(), OpenDSS::Instance().getPEVPlanningOf(nodeId).departureTimeFromHome, "departureTimeFromHome");
	stats.log(simTime().dbl(), OpenDSS::Instance().getPEVPlanningOf(nodeId).arrivalTimeAtWork, "arrival time at work");
	stats.log(simTime().dbl(), OpenDSS::Instance().getPEVPlanningOf(nodeId).departureTimeFromWork, "departure time from work");
	stats.log(simTime().dbl(), OpenDSS::Instance().getPEVPlanningOf(nodeId).arrivalTimeAtHome, "arrivalTimeAtHome");
	stats.log(simTime().dbl(), OpenDSS::Instance().getPEVPlanningOf(nodeId).stopChargingAtChargingStationAt, "stopChargingAtChargingStationAt");
	stats.log(simTime().dbl(), OpenDSS::Instance().getPEVPlanningOf(nodeId).PEVSpeedMilesPerHour, "PEVSpeedMilesPerHour");
	stats.log(simTime().dbl(), OpenDSS::Instance().getPEVPlanningOf(nodeId).PEVTimeToGoToWork, "PEVTimeToGoToWork");
	stats.log(simTime().dbl(), OpenDSS::Instance().getPEVPlanningOf(nodeId).deadline, "deadline");
	stats.log(simTime().dbl(), 0, "");

	OpenDSS::Instance().setStateOfChargeOf(nodeId, newSOC);
}

void FiWiTrafGen::sendChargingDeadlineRequest(bool justArrivedHome, bool wantV2G,const std::string& nodeId, double p_deadline)
{
	// FULL
	if (OpenDSS::Instance().getStateOfChargeOf(nodeId) >= 1 && ! wantV2G)
	{
		return;
	}

	static long chargingDeadlineMsgid = 0;
	++chargingDeadlineMsgid;

	char msgname[300];
	sprintf(msgname, "charging-deadline-request-%d-%ld", getId(), chargingDeadlineMsgid);
	EV << "Generating packet  `" << msgname << "'\n";

	Ieee802Ctrl* etherctrl = new Ieee802Ctrl();
	etherctrl->setSsap(localSAP);
	etherctrl->setDsap(remoteSAP);

	MACAddress dest = aggregatorOf(myAddress);

	etherctrl->setDest(dest);
	etherctrl->setSrc(myAddress);

	ChargingDeadlineRequestMessage* datapacket = new ChargingDeadlineRequestMessage(msgname, IEEE802CTRL_DATA);

	OpenDSS::Instance().setPEVBatteryKwHPerHour(nodeId, this->PEVBatteryKwPerHour);

	datapacket->setNodeId(nodeId.c_str());
	datapacket->setBatteryCapacity(this->PEVBatteryCapacity);
	datapacket->setBatteryKwPerHour(this->PEVBatteryKwPerHour);
	datapacket->setBatteryDepthOfDischarge(0);

	datapacket->setBatteryStateOfCharge(OpenDSS::Instance().getStateOfChargeOf(nodeId)); // 0..1
	datapacket->setCustomerID("customer id");
	datapacket->setVehicleID("vehicule id");
	datapacket->setBaseLoad(OpenDSS::Instance().getCurrentLoadOf(myAddress));
	datapacket->setWantV2G(wantV2G);

	double deadline = p_deadline;

	datapacket->setDeadline(deadline);

	string httpMessage = msgChargingDeadlineRequestPacket(datapacket);

	long lengthMessage = httpMessage.size() + this->TCPHeaderSize() + this->IPv6HeaderSize();
	datapacket->setByteLength(lengthMessage);

	datapacket->setControlInfo(etherctrl);

	EtherFrame *ethframe = new EthernetIIFrame(msgname);
	ethframe->setDest((dynamic_cast<Ieee802Ctrl*>(datapacket->getControlInfo()))->getDest());
	ethframe->setSrc((dynamic_cast<Ieee802Ctrl*>(datapacket->getControlInfo()))->getSrc());
	ethframe->setKind(FIWI_NODE_TYPE_ANY);
	ethframe->setPktType(FIWI_TRAF_PKT_TYPE_SMART_GRID_CONTROL);
	ethframe->setHost(ethframe->getSrc().str().c_str());

	ethframe->setTrafficClass(1);

	ethframe->setIsDOS(isDOS);

	ethframe->encapsulate(datapacket);

	ethframe->setControlInfo(datapacket->getControlInfo()->dup());

	send(ethframe, "out");
}
string FiWiTrafGen::msgStateOfChargePacket(StateOfChargeMessage* msg)
{
	stringstream inAll (stringstream::in | stringstream::out);
	stringstream inContent (stringstream::in | stringstream::out);

	inContent << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	inContent << "<StateOfCharge xmlns=\"http://zigbee.org/sep\">\n";
	inContent << "<Vehicule>\n";
	inContent << "<NodeID>" << msg->getNodeId() << "</NodeID>\n";
	inContent << "<CustomerID>" << msg->getCustomerID() << "</CustomerID>\n";
	inContent << "<VehiculeID>" << msg->getVehicleID() << "</VehiculeID>\n";
	inContent << "</Vehicule>\n";
	inContent << "<Battery>\n";
	inContent << "<Capacity>" << msg->getBatteryCapacity() << "</Capacity>\n";
	inContent << "<DepthOfDischarge>" << msg->getBatteryDepthOfDischarge() << "</DepthOfDischarge>\n";
	inContent << "<KwPerHour>" << msg->getBatteryKwPerHour() << "</KwPerHour>\n";
	inContent << "<StateOfCharge>" << msg->getBatteryStateOfCharge() << "</StateOfCharge>\n";
	inContent << "</Battery>\n";
	inContent << "</StateOfCharge>\n";

	inAll << "POST /distribution_network/state_of_charge/" << msg->getNodeId() << " HTTP/1.1\n";
	inAll << "Host: 3ffe:1900:4545:3:200:f8ff:fe21:67cf\n";
	inAll << "Content-Type: application/xml\n";
	inAll << "Cookie : session_id=d131dd02c5e6eec4693d9a0698aff95c";
	inAll << "Content-Length: " << inContent.str().size() << "\n";
	inAll << "\n";
	inAll << inContent.str();

	return inAll.str();
}

void FiWiTrafGen::sendStateOfCharge()
{
	static long stateOfChargeMsgId = 0;
	++stateOfChargeMsgId;

	char msgname[300];
	sprintf(msgname, "state-of-charge-%d-%ld", getId(), stateOfChargeMsgId);
	EV << "Generating packet  `" << msgname << "'\n";

	Ieee802Ctrl* etherctrl = new Ieee802Ctrl();
	etherctrl->setSsap(localSAP);
	etherctrl->setDsap(remoteSAP);

	MACAddress dest = aggregatorOf(myAddress);

	etherctrl->setDest(dest);
	etherctrl->setSrc(myAddress);

	StateOfChargeMessage* datapacket = new StateOfChargeMessage(msgname, IEEE802CTRL_DATA);

	string nodeId = OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress);

	datapacket->setNodeId(nodeId.c_str());
	datapacket->setBatteryCapacity(this->PEVBatteryCapacity);
	datapacket->setBatteryKwPerHour(this->PEVBatteryKwPerHour);
	datapacket->setBatteryDepthOfDischarge(0);

	updateStateOfCharge(nodeId);
	UpdateSOCEvents.log(simTime().dbl(), OpenDSS::Instance().getStateOfChargeOf(nodeId), nodeId + " SOC send msg ");

	datapacket->setBatteryStateOfCharge(OpenDSS::Instance().getStateOfChargeOf(nodeId)); // 0..1
	datapacket->setCustomerID("customer id");
	datapacket->setVehicleID("vehicule id");

	string httpMessage = msgStateOfChargePacket(datapacket);

	long lengthMessage = httpMessage.size() + this->TCPHeaderSize() + this->IPv6HeaderSize();
	datapacket->setByteLength(lengthMessage);

	datapacket->setControlInfo(etherctrl);

	EtherFrame *ethframe = new EthernetIIFrame(msgname);
	ethframe->setDest((dynamic_cast<Ieee802Ctrl*>(datapacket->getControlInfo()))->getDest());
	ethframe->setSrc((dynamic_cast<Ieee802Ctrl*>(datapacket->getControlInfo()))->getSrc());
	ethframe->setKind(FIWI_NODE_TYPE_ANY);
	ethframe->setPktType(FIWI_TRAF_PKT_TYPE_SMART_GRID_NOTIFICATION);
	ethframe->setHost(ethframe->getSrc().str().c_str());

	ethframe->setTrafficClass(1);

	ethframe->setIsDOS(isDOS);

	ethframe->encapsulate(datapacket);

	ethframe->setControlInfo(datapacket->getControlInfo()->dup());

	send(ethframe, "out");
}

string FiWiTrafGen::msgAuthenticationPacket(PEVAuthenticationMessage* msg)
{
	stringstream inAll (stringstream::in | stringstream::out);
	stringstream inContent (stringstream::in | stringstream::out);

	inContent << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	inContent << "<Authentication xmlns=\"http://zigbee.org/sep\">\n";
	inContent << "<Vehicule>\n";
	inContent << "<NodeID>" << msg->getNodeId() << "</NodeID>\n";
	inContent << "<CustomerID>" << msg->getCustomerID() << "</CustomerID>\n";
	inContent << "<VehiculeID>" << msg->getVehicleID() << "</VehiculeID>\n";
	inContent << "</Vehicule>\n";
	inContent << "</Authentication>\n";

	inAll << "POST /distribution_network/authentication/" << msg->getNodeId() << " HTTP/1.1\n";
	inAll << "Host: 3ffe:1900:4545:3:200:f8ff:fe21:67cf\n";
	inAll << "Content-Type: application/xml\n";
	inAll << "Content-Length: " << inContent.str().size() << "\n";
	inAll << "\n";
	inAll << inContent.str();

	return inAll.str();
}

void FiWiTrafGen::sendAuthentication()
{
	static long authenticationMsgId = 0;
	++authenticationMsgId;

	char msgname[300];
	sprintf(msgname, "authentication-%d-%ld", getId(), authenticationMsgId);
	EV << "Generating packet  `" << msgname << "'\n";

	Ieee802Ctrl* etherctrl = new Ieee802Ctrl();
	etherctrl->setSsap(localSAP);
	etherctrl->setDsap(remoteSAP);

	MACAddress dest = aggregatorOf(myAddress);

	etherctrl->setDest(dest);
	etherctrl->setSrc(myAddress);

	PEVAuthenticationMessage* datapacket = new PEVAuthenticationMessage(msgname, IEEE802CTRL_DATA);

	string nodeId = OpenDSS::Instance().getPowerNodeFromMACAddress(this->myAddress);

	datapacket->setNodeId(nodeId.c_str());
	datapacket->setCustomerID("customer id");
	datapacket->setVehicleID("vehicule id");

	string httpMessage = msgAuthenticationPacket(datapacket);

	long lengthMessage = httpMessage.size() + this->TCPHeaderSize() + this->IPv6HeaderSize();
	datapacket->setByteLength(lengthMessage);

	datapacket->setControlInfo(etherctrl);

	EtherFrame *ethframe = new EthernetIIFrame(msgname);
	ethframe->setDest((dynamic_cast<Ieee802Ctrl*>(datapacket->getControlInfo()))->getDest());
	ethframe->setSrc((dynamic_cast<Ieee802Ctrl*>(datapacket->getControlInfo()))->getSrc());
	ethframe->setKind(FIWI_NODE_TYPE_ANY);
	ethframe->setPktType(FIWI_TRAF_PKT_TYPE_SMART_GRID_CONTROL);
	ethframe->setHost(ethframe->getSrc().str().c_str());

	ethframe->setTrafficClass(1);

	ethframe->setIsDOS(isDOS);

	ethframe->encapsulate(datapacket);

	ethframe->setControlInfo(datapacket->getControlInfo()->dup());

	send(ethframe, "out");
}

string FiWiTrafGen::msgAuthenticationResponsePacket(PEVAuthenticationResponseMessage* msg)
{
	stringstream inAll (stringstream::in | stringstream::out);
	stringstream inContent (stringstream::in | stringstream::out);

	inContent << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	inContent << "<AuthenticationResponse xmlns=\"http://zigbee.org/sep\">\n";
	inContent << "<Vehicule>\n";
	inContent << "<NodeID>" << msg->getNodeId() << "</NodeID>\n";
	inContent << "<CustomerID>" << msg->getCustomerID() << "</CustomerID>\n";
	inContent << "<VehiculeID>" << msg->getVehicleID() << "</VehiculeID>\n";
	inContent << "</Vehicule>\n";
	inContent << "</AuthenticationResponse>\n";

	inAll << "POST /distribution_network/authentication_response/" << msg->getNodeId() << " HTTP/1.1\n";
	inAll << "Host: 3ffe:1900:4545:3:200:f8ff:fe21:67cf\n";
	inAll << "Content-Type: application/xml\n";
	inAll << "Set-cookie : session_id=d131dd02c5e6eec4693d9a0698aff95c";
	inAll << "Content-Length: " << inContent.str().size() << "\n";
	inAll << "\n";
	inAll << inContent.str();

	return inAll.str();
}

void FiWiTrafGen::sendAuthenticationResponse(MACAddress dest, string nodeId, string vehicleID, string customerID)
{
	static long authenticationResponseMsgid = 0;
	++authenticationResponseMsgid;

	char msgname[300];
	sprintf(msgname, "authentication-response-%d-%ld", getId(), authenticationResponseMsgid);
	EV << "Generating packet  `" << msgname << "'\n";

	Ieee802Ctrl* etherctrl = new Ieee802Ctrl();
	etherctrl->setSsap(localSAP);
	etherctrl->setDsap(remoteSAP);

	etherctrl->setDest(dest);
	etherctrl->setSrc(myAddress);

	PEVAuthenticationResponseMessage* datapacket = new PEVAuthenticationResponseMessage(msgname, IEEE802CTRL_DATA);

	datapacket->setNodeId(nodeId.c_str());
	datapacket->setCustomerID(customerID.c_str());
	datapacket->setVehicleID(vehicleID.c_str());

	string httpMessage = msgAuthenticationResponsePacket(datapacket);

	long lengthMessage = httpMessage.size() + this->TCPHeaderSize() + this->IPv6HeaderSize();
	datapacket->setByteLength(lengthMessage);

	datapacket->setControlInfo(etherctrl);

	EtherFrame *ethframe = new EthernetIIFrame(msgname);
	ethframe->setDest((dynamic_cast<Ieee802Ctrl*>(datapacket->getControlInfo()))->getDest());
	ethframe->setSrc((dynamic_cast<Ieee802Ctrl*>(datapacket->getControlInfo()))->getSrc());
	ethframe->setKind(FIWI_NODE_TYPE_ANY);
	ethframe->setPktType(FIWI_TRAF_PKT_TYPE_SMART_GRID_CONTROL);
	ethframe->setHost(ethframe->getDest().str().c_str());

	ethframe->setTrafficClass(1);

	ethframe->setIsDOS(isDOS);

	ethframe->encapsulate(datapacket);

	ethframe->setControlInfo(datapacket->getControlInfo()->dup());

	send(ethframe, "out");
}

string FiWiTrafGen::msgPEVControlMessagePacket(PEVControlMessage* msg)
{
	stringstream inAll (stringstream::in | stringstream::out);
	stringstream inContent (stringstream::in | stringstream::out);

	inContent << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	inContent << "<PEVControl xmlns=\"http://zigbee.org/sep\">\n";
	inContent << "<Vehicule>\n";
	inContent << "<NodeID>" << msg->getNodeId() << "</NodeID>\n";
	inContent << "<CustomerID>" << msg->getCustomerID() << "</CustomerID>\n";
	inContent << "<VehiculeID>" << msg->getVehicleID() << "</VehiculeID>\n";
	inContent << "</Vehicule>\n";
	inContent << "</PEVControlMessage>\n";

	inAll << "POST /distribution_network/pev_control/" << msg->getNodeId() << " HTTP/1.1\n";
	inAll << "Host: 3ffe:1900:4545:3:200:f8ff:fe21:67cf\n";
	inAll << "Content-Type: application/xml\n";
	inAll << "Set-cookie : session_id=d131dd02c5e6eec4693d9a0698aff95c";
	inAll << "Content-Length: " << inContent.str().size() << "\n";
	inAll << "\n";
	inAll << inContent.str();

	return inAll.str();
}

void FiWiTrafGen::sendPEVControlMessage(MACAddress dest, string nodeId, string vehicleID, string customerID)
{
	static long pevControlMsgid = 0;
	++pevControlMsgid;

	char msgname[300];
	sprintf(msgname, "pev-control-message-%d-%ld", getId(), pevControlMsgid);
	EV << "Generating packet  `" << msgname << "'\n";

	Ieee802Ctrl* etherctrl = new Ieee802Ctrl();
	etherctrl->setSsap(localSAP);
	etherctrl->setDsap(remoteSAP);

	etherctrl->setDest(dest);
	etherctrl->setSrc(myAddress);

	PEVControlMessage* datapacket = new PEVControlMessage(msgname, IEEE802CTRL_DATA);

	datapacket->setNodeId(nodeId.c_str());
	datapacket->setCustomerID(customerID.c_str());
	datapacket->setVehicleID(vehicleID.c_str());

	string httpMessage = msgPEVControlMessagePacket(datapacket);

	long lengthMessage = httpMessage.size() + this->TCPHeaderSize() + this->IPv6HeaderSize();
	datapacket->setByteLength(lengthMessage);

	datapacket->setControlInfo(etherctrl);

	EtherFrame *ethframe = new EthernetIIFrame(msgname);
	ethframe->setDest((dynamic_cast<Ieee802Ctrl*>(datapacket->getControlInfo()))->getDest());
	ethframe->setSrc((dynamic_cast<Ieee802Ctrl*>(datapacket->getControlInfo()))->getSrc());
	ethframe->setKind(FIWI_NODE_TYPE_ANY);
	ethframe->setPktType(FIWI_TRAF_PKT_TYPE_SMART_GRID_CONTROL);
	ethframe->setHost(ethframe->getDest().str().c_str());

	ethframe->setTrafficClass(1);

	ethframe->setIsDOS(isDOS);

	ethframe->encapsulate(datapacket);

	ethframe->setControlInfo(datapacket->getControlInfo()->dup());

	send(ethframe, "out");
}

string FiWiTrafGen::msgChargingDeadlineResponsePacket(ChargingDeadlineResponseMessage* msg, const std::vector<PEVSchedule>& schedules)
{
	stringstream inAll (stringstream::in | stringstream::out);
	stringstream inContent (stringstream::in | stringstream::out);

	inContent << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	inContent << "<ChargingDeadlineResponse xmlns=\"http://zigbee.org/sep\">\n";
	inContent << "<Vehicule>\n";
	inContent << "<NodeID>" << msg->getNodeId() << "</NodeID>\n";
	inContent << "<CustomerID>" << msg->getCustomerID() << "</CustomerID>\n";
	inContent << "<VehiculeID>" << msg->getVehicleID() << "</VehiculeID>\n";
	inContent << "</Vehicule>\n";

	//for (int i = 0; i < (int)schedules.size(); ++i)
	//{
			inContent << "<Scheduling>\n";
			inContent << "<StartTime>xxxx-xx-xx xx:xx:xx</StartTime>\n";
			inContent << "<EndTime>xxxx-xx-xx xx:xx:xx</EndTime>\n";
			inContent << "<Status>0</Status>\n";
		inContent << "</Scheduling>\n";
	//}
	inContent << "</ChargingDeadlineResponse>\n";

	inAll << "POST /distribution_network/charging_deadline_response/" << msg->getNodeId() << " HTTP/1.1\n";
	inAll << "Host: 3ffe:1900:4545:3:200:f8ff:fe21:67cf\n";
	inAll << "Content-Type: application/xml\n";
	inAll << "Cookie : session_id=d131dd02c5e6eec4693d9a0698aff95c";
	inAll << "Content-Length: " << inContent.str().size() << "\n";
	inAll << "\n";
	inAll << inContent.str();

	return inAll.str();
}

void FiWiTrafGen::sendChargingDeadlineResponse(MACAddress dest, std::string nodeId, std::string vehicleID, std::string customerID, const std::vector<PEVSchedule>& schedules)
{
	static long chargingDeadlineResponseMsgid = 0;
++chargingDeadlineResponseMsgid;

char msgname[300];
sprintf(msgname, "charging-deadline-response-%d-%ld", getId(), chargingDeadlineResponseMsgid);
EV << "Generating packet  `" << msgname << "'\n";

Ieee802Ctrl* etherctrl = new Ieee802Ctrl();
etherctrl->setSsap(localSAP);
etherctrl->setDsap(remoteSAP);

etherctrl->setDest(dest);
etherctrl->setSrc(myAddress);

ChargingDeadlineResponseMessage* datapacket = new ChargingDeadlineResponseMessage(msgname, IEEE802CTRL_DATA);

datapacket->setNodeId(nodeId.c_str());
datapacket->setCustomerID(customerID.c_str());
datapacket->setVehicleID(vehicleID.c_str());

datapacket->setResultsArraySize(schedules.size());

for (int i = 0; i < (int)schedules.size(); ++i)
{
	ChargingDeadlineResponseRow s;
	s.setStartTime(schedules[i].startTime);
	s.setStatus(schedules[i].startTime > 0);
	s.setEndTime(schedules[i].endTime);

	s.setType(schedules[i].type.c_str());

	datapacket->setResults(i, s);
}

string httpMessage = msgChargingDeadlineResponsePacket(datapacket, schedules);

long lengthMessage = httpMessage.size() + this->TCPHeaderSize() + this->IPv6HeaderSize();
datapacket->setByteLength(lengthMessage);

datapacket->setControlInfo(etherctrl);

EtherFrame *ethframe = new EthernetIIFrame(msgname);
ethframe->setDest((dynamic_cast<Ieee802Ctrl*>(datapacket->getControlInfo()))->getDest());
ethframe->setSrc((dynamic_cast<Ieee802Ctrl*>(datapacket->getControlInfo()))->getSrc());
ethframe->setKind(FIWI_NODE_TYPE_ANY);
ethframe->setPktType(FIWI_TRAF_PKT_TYPE_SMART_GRID_CONTROL);
ethframe->setHost(ethframe->getDest().str().c_str());

ethframe->setTrafficClass(1);

ethframe->setIsDOS(isDOS);

ethframe->encapsulate(datapacket);

ethframe->setControlInfo(datapacket->getControlInfo()->dup());

send(ethframe, "out");
}

/*
 * HTTP/1.1 204 No Content
Set-Cookie : session_id={session_hash}
 */

string FiWiTrafGen::msgAckPacket(PowerSysACKMessage* msg)
{
	string s = "HTTP/1.1 204 No Content\nCookie : session_id=d131dd02c5e6eec4693d9a0698aff95c;";

	return s;
}

MACAddress FiWiTrafGen::aggregatorOf(MACAddress addr)
{
	RowAggregatorToNode nodeToFind;
	nodeToFind.addr = addr;

	EV << "size aggres = " << FiWiGeneralConfigs::aggregatorInformation.size() << endl;

	for (std::map<RowAggregatorToNode, std::set<RowAggregatorToNode, RowAggregatorToNodeComparator> >::const_iterator it = FiWiGeneralConfigs::aggregatorInformation.begin();
			it != FiWiGeneralConfigs::aggregatorInformation.end();
			++it)
	{
		EV << "looping hehe cur = " << it->first.addr << endl;
		if (find(it->second.begin(), it->second.end(), nodeToFind) != it->second.end())
		{

			EV << "FOUND looping hehe cur = " << it->first.addr << endl;

			// Found aggregator !
			return it->first.addr;
		}

	}

	return addr;
}
