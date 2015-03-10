/*
 * OpenDSS.h
 *
 *  Created on: Oct 25, 2011
 *      Author: martin
 */

#include <omnetpp.h>
#include <map>
#include <set>
#include <string>
#include "PowerSysNode.h"
#include "MACAddress.h"
#include "RawStat.h"

#ifndef OPENDSS_H_
#define OPENDSS_H_

struct PowerSystemKnowledge
{
	double totalLosses;
	double totalLossesVar;
	double totalLineLosses;
	double totalLineLossesVar;
	double totalLoad;
	double totalLoadVar;
	double totalPower;
	double totalPowerVar;

	double totalBaseLoad();

	std::map<std::string, PowerSysNode> chargingStationNodes;

	std::map<std::string, PowerSysNode> nodes;
};

struct PEVPlanning
{
	double departureTimeFromHome;
	double PEVSpeedMilesPerHour;
	double distanceToWork;
	double PEVTimeToGoToWork;
	double arrivalTimeAtWork;
	bool arrivalTimeAtWorkProcessed;
	double departureTimeFromWork;
	double stopChargingAtChargingStationAt;
	bool stopChargingAtChargingStationAtProcessed;
	double arrivalTimeAtHome;
	double deadline;

	cMessage* msgStartChargingAtChargingStation;
};

struct PEVArrivalHome
{
	double hour;
	double percent;
	int minIndex;
	int maxIndex;
};

struct PEVDeparture
{
	double hour;
	double percent;
	int minIndex;
	int maxIndex;
};

struct PEVDistance
{
	double distanceMin;
	double distanceMax;
	double percent;
	int minIndex;
	int maxIndex;
};

class OpenDSS
{
public:
	static OpenDSS& Instance();

	double currentBaseLoadAt(const std::string& p_nodeName);

	void rawBaseLoadUpdate(const std::string& p_nodeName, double p_load);
	void recalculateWithOpenDSS();
	void recalculateWithOpenDSSParam(PowerSystemKnowledge& infos);

	void incrementNbPEVsAt(const std::string& nodeName);
	void decrementNbPEVsAt(const std::string& nodeName);

	void setNbPEVsAt(const std::string& nodeName, int nb);

	int nbPEVsAt(const std::string& nodeName);

	int nbPEVsDischargingAt(const std::string& nodeName);
	void setNbPEVsDischargingAt(const std::string& nodeName, int nb);

	double SolarLoadAtChargingStation(const std::string& nodeId, double time);

	double PEVkWPerHour(const std::string& nodeName)
	{
		return infosGrid.nodes[nodeName].getPEVKwH();
	}



	void setPEVBatteryKwHPerHourChargingStation(const std::string& nodeName, double kwhPerHour);

	void setNbPEVsAtChargingStation(const std::string& nodeName, int nb);
	int nbPEVsAtChargingStation(const std::string& nodeName);

	void setPEVBatteryKwHPerHour(const std::string& nodeName, double kwhPerHour);

	double getStateOfChargeOf(const std::string& nodeName);
	void setStateOfChargeOf(const std::string& nodeName, double soc);

	std::string printNodes();
	void setMapping(MACAddress addr, std::string powerNodeId);

	std::string getPowerNodeFromMACAddress(MACAddress addr);
	MACAddress getMACAddressFromPowerNode(std::string nodeId);
	double getCurrentVoltageOf(MACAddress addr);

	double getTotalPowerLosses();
	double getTotalPowerLossesVar();
	double getTotalPowerLineLosses();
	double getTotalPowerLineLossesVar();
	double getTotalPowerLoad();
	double getTotalPowerLoadVar();
	double getTotalPower();
	double getTotalPowerVar();

	int totalPEVsCharging();

	double getCurrentLoadOf(MACAddress addr);

	void setPEVStartTime(const std::string& nodeId, double time);
	double getPEVStartTime(const std::string& nodeId);

	void setPEVDischargeStartTime(const std::string& nodeId, double time);
	double getPEVDischargeStartTime(const std::string& nodeId);

	void updatePowerSystemStats(double time);

	int NbSecondsPerHour;

	std::map<double, PowerSystemKnowledge> history;

	void setReactiveControlSensorType(std::string p_type)
	{
		reactiveControlSensorType = p_type;
	}

	std::string getReactiveControlSensorType()
	{
		return reactiveControlSensorType;
	}

	void setReactiveControlSensorThreshold(double p_th)
	{
		reactiveControlSensorThreshold = p_th;
	}

	double getReactiveControlSensorThreshold()
	{
		return reactiveControlSensorThreshold;
	}

	std::set<std::string> getLVs();
	std::string LVNameOf(const std::string& nodeName);

	std::map<std::string, MACAddress> getNodes() { return powerNodeToMacAddress; }

	bool isChargingStationNode(const std::string& nodeId);

	std::vector<PEVArrivalHome> getArrivalsHome() { return arrivalsHome; }
	std::vector<PEVArrivalHome> getPevDepartures() { return arrivalsHome; }

	void addPevDeparture(PEVDeparture d)
	{
		pevDepartures.push_back(d);
	}

	void addArrivalHome(PEVArrivalHome a)
	{
		arrivalsHome.push_back(a);
	}

	void addDistanceDistribution(PEVDistance d)
	{
		pevDistanceDistribution.push_back(d);
	}

	double randPEVArrivalHome(double minTime, double secondsPerHour);
	double randPEVDeparture(double minTime, double secondsPerHour);

	double totalV2GLoad();

	double randPEVDistanceToDo();

	void setPEVPlanningDepartureTimeFromHomeOf(const std::string& nodeId, double v)
	{
		pevPlannings[nodeId].departureTimeFromHome = v;
	}

	void setPEVPlanningPEVSpeedMilesPerHour(const std::string& nodeId, double v)
	{
		pevPlannings[nodeId].PEVSpeedMilesPerHour = v;
	}

	void setPEVPlanningDistanceToWork(const std::string& nodeId, double v)
	{
		pevPlannings[nodeId].distanceToWork = v;
	}

	void setPEVPlanningPEVTimeToGoToWork(const std::string& nodeId, double v)
	{
		pevPlannings[nodeId].PEVTimeToGoToWork = v;
	}

	void setPEVPlanningArrivalTimeAtWork(const std::string& nodeId, double v)
	{
		pevPlannings[nodeId].arrivalTimeAtWork = v;
	}

	void setPEVPlanningArrivalTimeAtWorkProcessed(const std::string& nodeId, bool v)
	{
		pevPlannings[nodeId].arrivalTimeAtWorkProcessed = v;
	}

	void setPEVPlanningDepartureTimeFromWork(const std::string& nodeId, double v)
	{
		pevPlannings[nodeId].departureTimeFromWork = v;
	}

	void setPEVPlanningArrivalTimeAtHome(const std::string& nodeId, double v)
	{
		pevPlannings[nodeId].arrivalTimeAtHome = v;
	}

	void setStopChargingAtChargingStationAt(const std::string& nodeId, double v)
	{
		pevPlannings[nodeId].stopChargingAtChargingStationAt = v;
	}

	void setStopChargingAtChargingStationAtProcessed(const std::string& nodeId, bool v)
	{
		pevPlannings[nodeId].stopChargingAtChargingStationAtProcessed = v;
	}

	void setPEVPlanningDeadline(const std::string& nodeId, double v)
	{
		pevPlannings[nodeId].deadline = v;
	}

	const PEVPlanning& getPEVPlanningOf(const std::string& nodeId)
	{
		return pevPlannings[nodeId];
	}

	std::string openDSSConfiguration;

	std::map<std::string, PEVPlanning> pevPlannings;

	// nodeId -> hour -> load in kW
	std::map<std::string, std::map<int, double> > solarDistributionAt;
	bool withChargingStationSolarPanels;

	PowerSystemKnowledge infosGrid;

	double V2GThresholdCoefficient;

private:
	OpenDSS();

	RawStat PowerSysTotalLosses;
	RawStat PowerSysTotalLossesVar;
	RawStat PowerSysTotalLineLosses;
	RawStat PowerSysTotalLineLossesVar;
	RawStat PowerSysTotalLoad;
	RawStat PowerSysTotalLoadVar;
	RawStat PowerSysTotalPower;
	RawStat PowerSysTotalPowerVar;
	RawStat ChargingStationVoltage;
	RawStat ChargingStationLoad;
	RawStat V2GLoad;
	RawStat SolarLoad;

	std::vector<PEVArrivalHome> arrivalsHome;
	std::vector<PEVDeparture> pevDepartures;
	std::vector<PEVDistance> pevDistanceDistribution;



	std::string reactiveControlSensorType;
	double reactiveControlSensorThreshold;



	std::map<MACAddress, std::string> macAddressToPowerNode;
	std::map<std::string, MACAddress> powerNodeToMacAddress;

	std::string genParametersStr(const PowerSystemKnowledge& infos);

};

#endif /* OPENDSS_H_ */
