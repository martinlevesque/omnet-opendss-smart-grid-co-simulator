/*
 * DMS.h
 *
 *  Created on: Nov 1, 2011
 *      Author: martin
 */

#include <string>
#include <vector>
#include "OpenDSS.h"
#include <set>

#ifndef DMS_H_
#define DMS_H_

struct PEVSchedule
{
	double startTime; // In seconds
	double endTime;   // In seconds
	std::string nodeId;
	double kW;
	std::string type;
};

class DMS
{
public:
	static DMS& Instance();

	virtual ~DMS();

	void updateVoltage(std::string nodeId, double voltage);
	void updateLoad(std::string nodeId, double load);

	double getLoad(std::string nodeId);

	void setNbPEVsDischarging(std::string nodeId, int nb);
	int getNbPEVsDischarging(std::string nodeId);

	void updatePEVkWH(std::string nodeId, double kW);

	bool alreadyChargingAt(std::string nodeId);

	bool currentlyChargingAt(double time, std::string nodeId);

	void updateTotalLoad(double totalLoad);
	void updateTotalLosses(double totalLosses);
	std::vector<PEVSchedule> schedulePEV(std::string nodeId, double currentTime, const std::string& schedulingAlgorithm, double deadline,
			double capacity, double stateOfCharge, double kWPerHour, bool wantV2G);

	void removeSchedOf(std::string nodeId, std::string type);

	// duration, kWPerHou, deadline
	PEVSchedule schedulePEVFirstFit(std::string nodeId, double currentTime, double deadline, double duration, double kWPerHour);
	PEVSchedule schedulePEVSLM(std::string nodeId, double currentTime, double deadline, double duration, double kWPerHour,
			const std::vector<PEVSchedule>& scheds, const std::string& optimizationComparison);

	PEVSchedule schedulePEVRandomCharging(std::string nodeId, double currentTime, double deadline, double duration, double kWPerHour);

	PEVSchedule schedulePEVV2G(std::string nodeId, double currentTime, double deadline, double duration, double kWPerHour);

	void setSlotInterval(double p_slotInterval);

	void setProfile(std::string node, double time, double load);

	std::vector<std::string> execReactiveControl(double time);

	void setMaximumPowerDemand(double load)
	{
		maximumPowerLoad = load;
	}

	void setReactiveControlAlgorithm(std::string p_algo)
	{
		reactiveControlAlgorithm = p_algo;
	}

	std::string getReactiveControlAlgorithm()
	{
		return reactiveControlAlgorithm;
	}

	int getNbDrops()
	{
		return nbDrops;
	}

	void setSLMMaxSlotLengthInSeconds(double v)
	{
		SLMMaxSlotLengthInSeconds = v;
	}

	double getV2gBeginAt()
	{
		return v2gBeginAt;
	}

	void setV2gBeginAt(double at)
	{
		v2gBeginAt = at;
	}

	double solarAvailableAt(double t);
	double solarUsedAt(double t, double kWPerHour);

	double getV2gEndAt()
	{
		return v2gEndAt;
	}

	void setV2gEndAt(double at)
	{
		v2gEndAt = at;
	}

	double getExpectedLoadAt(const std::string& nodeId, double time)
	{
		return expectedProfile[nodeId][time];
	}

	bool isV2GPeriod(double time);

	double maximumPowerLoad;

	std::string chargingStationSchedulingAlgorithm;

private:
	DMS();

	bool isSLMValid(PowerSystemKnowledge& infos);

	bool currentlySchedAtIn(double time, const std::vector<PEVSchedule>& scheds);

	std::vector<PEVSchedule> prettySLMSchedule(const std::string& nodeId, double currentTime, double deadline,
			double durationInSeconds, double schedulingLength, double kWPerHour, const std::string& optimizationComparison);

	PowerSystemKnowledge calcSysAt(double t, double currentTime);
	std::vector<PEVSchedule> SLMScheduleDuringV2G(const std::string& nodeId, double currentTime, double deadline,
			double durationInSeconds, double schedulingLength, double kWPerHour);
	double totalLoadAverageBetween(double t1, double t2, double currentTime);

	int nbPEVsAt(std::string nodeId, double time);
	int nbPEVsDischargingAt(std::string nodeId, double time);
	double baseBaseProfileLoadAt(std::string nodeId, double time);

	double v2gBeginAt;
	double v2gEndAt;

	PowerSystemKnowledge infosGrid;
	std::vector<PEVSchedule> scheds;
	double slotInterval;

	double SLMMaxSlotLengthInSeconds;

	std::map<std::string, std::map<double, double> > expectedProfile;

	int nbDrops;
	std::string reactiveControlAlgorithm;
};

#endif /* DMS_H_ */
