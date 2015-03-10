/*
 * DMS.cc
 *
 *  Created on: Nov 1, 2011
 *      Author: martin
 */

#include <string>
#include "DMS.h"
#include <sstream>
#include <set>

using namespace std;

DMS::DMS()
{
	v2gBeginAt = 0;
	v2gEndAt = 0;
}

DMS& DMS::Instance()
{
	static DMS s;

	s.nbDrops = 0;

	return s;
}

void DMS::setProfile(string node, double time, double load)
{
	expectedProfile[node][time] = load;
}

void DMS::updateVoltage(string nodeId, double voltage)
{
	infosGrid.nodes[nodeId].setName(nodeId);
	infosGrid.nodes[nodeId].setVoltage(voltage);
}

void DMS::updateLoad(string nodeId, double load)
{
	infosGrid.nodes[nodeId].setName(nodeId);
	infosGrid.nodes[nodeId].setBaseLoad(load);
}

double DMS::getLoad(std::string nodeId)
{
	return infosGrid.nodes[nodeId].getBaseLoad();
}

void DMS::setNbPEVsDischarging(std::string nodeId, int nb)
{
	infosGrid.nodes[nodeId].setNbPEVsDischarging(nb);
}

int DMS::getNbPEVsDischarging(std::string nodeId)
{
	return infosGrid.nodes[nodeId].getNbPEVsDischarging();
}

void DMS::updatePEVkWH(std::string nodeId, double kW)
{
	infosGrid.nodes[nodeId].setName(nodeId);
	infosGrid.nodes[nodeId].setPEVKwH(kW);
}

void DMS::updateTotalLoad(double totalLoad)
{
	infosGrid.totalLoad = totalLoad;
}

void DMS::updateTotalLosses(double totalLosses)
{
	infosGrid.totalLosses = totalLosses;
}

void DMS::setSlotInterval(double p_slotInterval)
{
	this->slotInterval = p_slotInterval;
}

// Based on the scheds
int DMS::nbPEVsAt(string nodeId, double time)
{
	int cpt = 0;

	for (int i = 0; i < (int)scheds.size(); ++i)
	{
		if (scheds[i].nodeId == nodeId && time >= scheds[i].startTime && time <= scheds[i].endTime && scheds[i].type == "charging")
		{
			++cpt;
		}
	}

	return cpt;
}

int DMS::nbPEVsDischargingAt(string nodeId, double time)
{
	int cpt = 0;

	for (int i = 0; i < (int)scheds.size(); ++i)
	{
		if (scheds[i].nodeId == nodeId  && time >= scheds[i].startTime && time <= scheds[i].endTime && scheds[i].type == "discharging")
		{
			++cpt;
		}
	}

	return cpt;
}

double DMS::baseBaseProfileLoadAt(string nodeId, double time)
{
	double load = expectedProfile[nodeId][(int)time];

	if (load <= 0)
	{
		for (int t = (int)time; t >= 0; --t)
		{
			if (expectedProfile[nodeId][(int)time] > 0)
				return expectedProfile[nodeId][(int)time];
		}
	}

	return load;
}

bool DMS::currentlyChargingAt(double time, string nodeId)
{
	for (int i = 0; i < (int)scheds.size(); ++i)
	{
		if (scheds[i].nodeId == nodeId && time >= scheds[i].startTime && time <= scheds[i].endTime && scheds[i].type == "charging")
		{
			return true;
		}
	}

	return false;
}

bool DMS::currentlySchedAtIn(double time, const std::vector<PEVSchedule>& scheds)
{
	for (int i = 0; i < (int)scheds.size(); ++i)
	{
		if (time >= scheds[i].startTime && time <= scheds[i].endTime)
		{
			return true;
		}
	}

	return false;
}

void DMS::removeSchedOf(string nodeId, string type)
{
	int ind = -1;

	vector<int> inds2Del;

	for (int i = 0; i < (int)scheds.size(); ++i)
	{
		if (scheds[i].nodeId == nodeId && type == scheds[i].type)
		{
			ind = i;
			inds2Del.push_back(ind);
		}
	}

	for (int i = 0; i < (int)inds2Del.size(); ++i)
	{
		scheds.erase(scheds.begin() + inds2Del[i]);
	}
}

bool DMS::alreadyChargingAt(string nodeId)
{
	for (int i = 0; i < (int)scheds.size(); ++i)
	{
		if (scheds[i].nodeId == nodeId && scheds[i].type == "charging")
		{
			return true;
		}
	}

	return false;
}

vector<string> DMS::execReactiveControl(double time)
{
	vector<string> nodesDeactivated;
	//static RawStat stats = RawStat("execReactiveControl", "event", 1);

	if (this->reactiveControlAlgorithm == "voltageControl")
	{
		//stats.log(time, 1, "ok algo.");
		for (std::map<std::string, PowerSysNode>::const_iterator it = infosGrid.nodes.begin(); it != infosGrid.nodes.end(); ++it)
		{
			int cpt = 0;

			//stats.log(time, 1, "node loop ");
			//stats.log(time, 1, it->first);

			while (it->second.getVoltage() < 0.95 && it->second.getVoltage() > 0)
			{
				//stats.log(time, 1, "! voltage problem !");
				if (++cpt > 1000)
				{
					break;
				}

				double minVoltage = 100000;
				string nodeId = "";

				// select node with minimum voltage currently charging.
				for (std::map<std::string, PowerSysNode>::const_iterator itCheck = infosGrid.nodes.begin(); itCheck != infosGrid.nodes.end(); ++itCheck)
				{
					if (itCheck->second.getVoltage() < minVoltage && currentlyChargingAt(time, itCheck->first))
					{
						minVoltage = itCheck->second.getVoltage();
						nodeId = itCheck->first;
					}
				}

				if (nodeId != "")
				{
					//stats.log(time, 1, "NODE PROBLEM");
					//stats.log(time, 1, nodeId);
					removeSchedOf(nodeId, "charging");
					nodesDeactivated.push_back(nodeId);
					OpenDSS::Instance().recalculateWithOpenDSSParam(infosGrid);
				}
			}
		}
	}

	return nodesDeactivated;
}

std::vector<PEVSchedule> DMS::schedulePEV(string nodeId, double currentTime, const std::string& schedulingAlgorithm, double deadline,
		double capacity, double stateOfCharge, double kWPerHour, bool wantV2G)
{
	std::vector<PEVSchedule> results;

	double kWToCharge = (1.0 - stateOfCharge) * capacity;
	double durationInSeconds = (kWToCharge / kWPerHour) * (double)OpenDSS::Instance().NbSecondsPerHour;

	static RawStat schedRes = RawStat("schedulePEVRes", "event", 1);

	// Special case for charging station schedule
	if (currentTime >= OpenDSS::Instance().pevPlannings[nodeId].arrivalTimeAtWork &&
			currentTime <= OpenDSS::Instance().pevPlannings[nodeId].departureTimeFromWork)
	{
		// Try to charge as much as possible:
		if (durationInSeconds > OpenDSS::Instance().pevPlannings[nodeId].departureTimeFromWork - currentTime)
		{
			durationInSeconds = OpenDSS::Instance().pevPlannings[nodeId].departureTimeFromWork - currentTime;
		}

		// First try to find solar slot:
		for (double t = currentTime; OpenDSS::Instance().pevPlannings[nodeId].departureTimeFromWork - t >= durationInSeconds; t += 1)
		{
			bool solarAvailableIn = true;

			for (double t2 = t; t2 <= t + durationInSeconds; t2 += 0.001)
			{
				double cur = solarAvailableAt(t2) - solarUsedAt(t2, kWPerHour);

				if (cur < 0)
					cur = 0;

				PowerSystemKnowledge infos = calcSysAt(t2, t2);

				bool constraintsRespected = isSLMValid(infos);

				if (cur < kWPerHour && constraintsRespected)
				{
					solarAvailableIn = false;
					break;
				}
			}

			if (solarAvailableIn)
			{
				PEVSchedule result;

				result.startTime = t;

				result.endTime = t + durationInSeconds;
				result.kW = kWPerHour;
				result.nodeId = nodeId;
				result.type = "chargingStation";
				scheds.push_back(result);

				vector<PEVSchedule> tmp;
				tmp.push_back(result);

				return tmp;
			}
		}

		// At work.
		vector<PEVSchedule> tmp = prettySLMSchedule(nodeId, currentTime, deadline, durationInSeconds, durationInSeconds, kWPerHour, "lossesBased");

		for (int i = 0; i < (int)tmp.size(); ++i)
		{
			tmp[i].type = "chargingStation";
			scheds.push_back(tmp[i]);
		}

		return tmp;
	}
	// End Special case for charging station schedule

	removeSchedOf(nodeId, "discharging");
	removeSchedOf(nodeId, "charging");

	// ADAPTIVE DEADLINE:
	if (currentTime < DMS::Instance().getV2gBeginAt() - 0.001)
		deadline = DMS::Instance().getV2gBeginAt() - 0.001;

	// Already charging ?
	if (currentlyChargingAt(currentTime, nodeId))
	{
		schedRes.log(simTime().dbl(), 1, string("already charging .. ") + nodeId);
		return results;
	}

	if (schedulingAlgorithm == "firstFit")
	{
		PEVSchedule result = schedulePEVFirstFit(nodeId, currentTime, deadline, durationInSeconds, kWPerHour);

		results.push_back(result);
		scheds.push_back(result);
	}
	else
	if (schedulingAlgorithm == "randomCharging")
	{
		PEVSchedule result = schedulePEVRandomCharging(nodeId, currentTime, deadline, durationInSeconds, kWPerHour);

		results.push_back(result);
		scheds.push_back(result);
	}
	else
	if (schedulingAlgorithm == "SLM")
	{
		static RawStat r = RawStat("SLMSlotLength", "event", 1);

		double schedulingLength = durationInSeconds;

		if (schedulingLength > SLMMaxSlotLengthInSeconds)
			schedulingLength = SLMMaxSlotLengthInSeconds;

		r.log(simTime().dbl(), schedulingLength, "schedulingLength");
		r.log(simTime().dbl(), durationInSeconds, "durationInSeconds");
		r.log(simTime().dbl(), SLMMaxSlotLengthInSeconds, "SLMMaxSlotLengthInSeconds");

		if (isV2GPeriod(currentTime))
		{
			results = SLMScheduleDuringV2G(nodeId, currentTime, deadline, durationInSeconds, schedulingLength, kWPerHour);
		}
		else
		{
			results = prettySLMSchedule(nodeId, currentTime, deadline, durationInSeconds, schedulingLength, kWPerHour, "lossesBased");
		}

		for (int i = 0; i < (int)results.size(); ++i)
		{
			// Local copy
			scheds.push_back(results[i]);
		}

		r.log(simTime().dbl(), 0, "-------");
	}
	else
	{
		// problem...
		schedRes.log(simTime().dbl(), 1, string("PROBLEM algo .. ") + nodeId);
	}

	return results;
}

std::vector<PEVSchedule> DMS::SLMScheduleDuringV2G(const std::string& nodeId, double currentTime, double deadline,
		double durationInSeconds, double schedulingLength, double kWPerHour)
{
	std::vector<PEVSchedule> tmpRes;

	double v2gResBegin = currentTime;
	vector<PEVSchedule> resSLMSlots;

	schedulingLength = durationInSeconds;

	if (schedulingLength > SLMMaxSlotLengthInSeconds)
		schedulingLength = SLMMaxSlotLengthInSeconds;

	vector<PEVSchedule> resultWithoutV2G = prettySLMSchedule(nodeId, v2gResBegin, deadline, durationInSeconds,
					schedulingLength, kWPerHour, "lossesBased");

	if (resultWithoutV2G.size() == 0) // Well, not possible
		return tmpRes;

	double v2gSchedulingLength = getV2gEndAt() - v2gResBegin;

	if (v2gSchedulingLength > SLMMaxSlotLengthInSeconds)
		v2gSchedulingLength = SLMMaxSlotLengthInSeconds;

	double sumSecondsV2g = 0;
	vector<PEVSchedule> currentSLMResult = resultWithoutV2G; // this vector will contain the final slm result

	// Try to do as much v2g as possible
	for (double t = v2gResBegin; t < getV2gEndAt(); t += v2gSchedulingLength + 0.001)
	{
		double curV2gLength = ((t + v2gSchedulingLength > getV2gEndAt()) ? ceil(getV2gEndAt() - t) : (v2gSchedulingLength));

		// Current v2g slot = t, t + curV2gLength

		double extraChargingTime = sumSecondsV2g + curV2gLength;

		schedulingLength = durationInSeconds + extraChargingTime;

		if (schedulingLength > SLMMaxSlotLengthInSeconds)
			schedulingLength = SLMMaxSlotLengthInSeconds;

		vector<PEVSchedule> tmpSLM = prettySLMSchedule(nodeId, t + curV2gLength + 0.001, deadline, durationInSeconds + extraChargingTime,
				schedulingLength, kWPerHour, "lossesBased");

		// step 1
		// If this new slm result contains a lower number of slots, then stops.
		if (tmpSLM.size() < currentSLMResult.size())
			break;

		currentSLMResult = tmpSLM; // Keep this new SLM then.

		// step 2: Should we add this v2g slot ???
		double averageLoadDuringV2gSlot = totalLoadAverageBetween(t, t + curV2gLength, currentTime);

		static RawStat logLoads = RawStat("AverageLoadDuringV2gSlot", "event", 1);
		logLoads.log(simTime().dbl(), averageLoadDuringV2gSlot, nodeId);

		// Is the average total load higher than the v2g th ??
		if (averageLoadDuringV2gSlot > (double)this->maximumPowerLoad * OpenDSS::Instance().V2GThresholdCoefficient)
		{
			// OK, we should v2g to decrease the load
			PEVSchedule result;

			result.startTime = t;

			result.endTime = t + curV2gLength;
			result.kW = kWPerHour;
			result.nodeId = nodeId;
			result.type = "discharging";

			// add v2g result
			tmpRes.push_back(result);

			sumSecondsV2g += curV2gLength; // add length of v2gs
		}
	}

	// Add charging slots:
	for (int i = 0; i < (int)currentSLMResult.size(); ++i)
	{
		tmpRes.push_back(currentSLMResult[i]);
	}

	return tmpRes;
}

double DMS::totalLoadAverageBetween(double t1, double t2, double currentTime)
{
	double sumLoads = 0;
	double nbLoads = 0;

	for (double t = t1; t <= t2; t += 1)
	{
		PowerSystemKnowledge sys = calcSysAt(t, currentTime);

		static RawStat logLoads = RawStat("AverageLoadDuringV2gSlotVERBOSE", "event", 1);
		logLoads.log(currentTime, t1, "t1");
		logLoads.log(currentTime, t2, "t2");
		logLoads.log(currentTime, t, "t");
		logLoads.log(currentTime, sys.totalLoad, "sys.totalLoad");
		logLoads.log(currentTime, 0, "");

		if (sys.totalLoad <= 10)
			continue;

		sumLoads += sys.totalLoad;
		nbLoads += 1;
	}

	return (nbLoads > 0) ? sumLoads / nbLoads : 0;
}

std::vector<PEVSchedule> DMS::prettySLMSchedule(const std::string& nodeId, double currentTime, double deadline,
		double durationInSeconds, double schedulingLength, double kWPerHour, const std::string& optimizationComparison)
{
	vector<PEVSchedule> tmp;

	static RawStat l = RawStat("prettySLMSchedule", "event", 1);
	l.log(simTime().dbl(), currentTime, " currentTime ");
	l.log(simTime().dbl(), deadline, " deadline");
	l.log(simTime().dbl(), durationInSeconds, " durationInSeconds");
	l.log(simTime().dbl(), schedulingLength, " schedulingLength ");
	l.log(simTime().dbl(), kWPerHour, " kWPerHour ");


	for (double curSlotLength = 0;
					curSlotLength < durationInSeconds;
					curSlotLength += ((curSlotLength + schedulingLength > durationInSeconds) ? ceil(durationInSeconds - curSlotLength) : (schedulingLength)))
	{
		double cur = ((curSlotLength + schedulingLength > durationInSeconds) ? ceil(durationInSeconds - curSlotLength) : (schedulingLength));

		l.log(simTime().dbl(), cur, " loop cur ");
		l.log(simTime().dbl(), curSlotLength, " loop curSlotLength ");
		l.log(simTime().dbl(), schedulingLength, " loop schedulingLength ");
		l.log(simTime().dbl(), durationInSeconds, " loop durationInSeconds ");
		l.log(simTime().dbl(), currentTime, " loop currentTime ");

		PEVSchedule sched = schedulePEVSLM(nodeId, currentTime, deadline, cur, kWPerHour, tmp, optimizationComparison);

		l.log(simTime().dbl(), sched.startTime, " loop sched.startTime ");

		if (sched.startTime > 0)
		{
			tmp.push_back(sched);
		}
		else
		{
			break;
		}
	}

	return tmp;
}

bool DMS::isV2GPeriod(double time)
{
	return time >= DMS::Instance().getV2gBeginAt() && time <= DMS::Instance().getV2gEndAt() && OpenDSS::Instance().V2GThresholdCoefficient < 1;
}

PEVSchedule DMS::schedulePEVRandomCharging(string nodeId, double currentTime, double deadline, double durationInSeconds, double kWPerHour)
{
	PEVSchedule result;


	result.startTime = currentTime;

	result.endTime = result.startTime + durationInSeconds;
	result.kW = kWPerHour;
	result.nodeId = nodeId;
	result.type = "charging";

	return result;
}

PEVSchedule DMS::schedulePEVV2G(std::string nodeId, double currentTime, double deadline, double duration, double kWPerHour)
{
	PEVSchedule result;

	result.startTime = currentTime;

	result.endTime = 0;
	result.kW = kWPerHour;
	result.nodeId = nodeId;
	result.type = "discharging";

	return result;
}

PEVSchedule DMS::schedulePEVFirstFit(string nodeId, double currentTime, double deadline, double durationInSeconds, double kWPerHour)
{
	//stringstream in (stringstream::in | stringstream::out);
	//in << "node = " << nodeId << " current time = " << currentTime << " deadline = " << deadline << " duration in seconds = " << durationInSeconds << " kw per hour = " << kWPerHour << endl;

	static RawStat stats = RawStat("schedulePEVFirstFit", "event", 1);

	PEVSchedule result;
	double startTime = -1;

	// Loop for all possible schedulings
	for (double tStart = currentTime; tStart + durationInSeconds <= deadline && tStart < 100; tStart += slotInterval)
	{
		bool constraintsRespected = true;
		//in << "   tstart = " << tStart << endl;

		// for tStart, we need to calculate the cost of this scheduling
		for (double tSchedule = tStart; tSchedule <= tStart + durationInSeconds && tSchedule < 100; tSchedule += slotInterval)
		{
			//in << "   		tSchedule = " << tSchedule << endl;
			// 1-
			// COPY calc avec loads base, pev load en REF

			// res = voltage, total load, total losses
			PowerSystemKnowledge infos = calcSysAt(tSchedule, currentTime);

			// 2- contrainte voltage
			for (std::map<std::string, PowerSysNode>::const_iterator it = infos.nodes.begin(); it != infos.nodes.end(); ++it)
			{
				//in << "   		voltage at " << it->first << " = " << it->second.getVoltage() << endl;
				if ( ! (it->second.getVoltage() >= 0.95 && it->second.getVoltage() <= 1.05) && it->second.getVoltage() > 0)
				{
					constraintsRespected = false;
					break;
				}
			}

			//in << "   		total power load =  " << infos.totalPowerLoad << " maximum power load = " << maximumPowerLoad << endl;
			// 3- contrainte load <= max load
			if (infos.totalLoad > (double)this->maximumPowerLoad * OpenDSS::Instance().V2GThresholdCoefficient)
			{
				constraintsRespected = false;
				break;
			}
		}

		if ( ! constraintsRespected)
			continue;

		startTime = tStart;
		break; // FIRST FIT algo.
	}

	//in << " OK FOUND result.startTime = " << result.startTime << endl;
	//in << " OK FOUND result.endTime = " << result.endTime << endl;
	//in << " OK FOUND result.kW = " << result.kW << endl;

	//stats.log(currentTime, 1, in.str());

	result.startTime = startTime;
	result.endTime = startTime + durationInSeconds;
	result.kW = kWPerHour;
	result.nodeId = nodeId;
	result.type = "charging";

	return result;
}

PowerSystemKnowledge DMS::calcSysAt(double t, double currentTime)
{
	//static RawStat logLoads = RawStat("calcSysAt", "event", 1);
	//logLoads.log(currentTime, t, "t");

	PowerSystemKnowledge infos;

	// We load the current values (a copy)
	//logLoads.log(currentTime, t, " COND 1");

	// Loop over the nodes:
	std::map<std::string, MACAddress> nodes = OpenDSS::Instance().getNodes();

	for (std::map<std::string, MACAddress>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		if (OpenDSS::Instance().isChargingStationNode(it->first))
			continue;

		double baseLoad = baseBaseProfileLoadAt(it->first, t);

		//if (t > currentTime)
		//{
		infos.nodes[it->first].setBaseLoad(baseLoad);
		//logLoads.log(currentTime, t, " COND 2");
		// }

		//logLoads.log(currentTime, 0, string(" node id =  ") + it->first);
		//logLoads.log(currentTime, infos.nodes[it->first].getBaseLoad(), " base load ");


		infos.nodes[it->first].setNbPEVs(this->nbPEVsAt(it->first, t));
		infos.nodes[it->first].setNbPEVsDischarging(this->nbPEVsDischargingAt(it->first, t));
		infos.nodes[it->first].setPEVKwH(infosGrid.nodes[it->first].getPEVKwH());

		//logLoads.log(currentTime, infos.nodes[it->first].getNbPEVs(), " nb pevs ");
		//logLoads.log(currentTime, infos.nodes[it->first].getNbPEVsDischarging(), " nb pevs discharging ");
		//logLoads.log(currentTime, infos.nodes[it->first].getPEVKwH(), " pev kwh ");

		infos.nodes[it->first].setName(it->first);
	}

	OpenDSS::Instance().recalculateWithOpenDSSParam(infos);

	return infos;
}

double DMS::solarAvailableAt(double t)
{
	double sum = 0;

	for (std::map<std::string, PowerSysNode>::iterator it = OpenDSS::Instance().infosGrid.chargingStationNodes.begin();
			it != OpenDSS::Instance().infosGrid.chargingStationNodes.end(); ++it)
	{
		sum += OpenDSS::Instance().SolarLoadAtChargingStation(it->first, t);
	}

	return sum;
}

double DMS::solarUsedAt(double t, double kWPerHour)
{
	double sum = 0;

	for (int i = 0; i < (int)scheds.size(); ++i)
	{
		if (scheds[i].type == "chargingStation" && t >= scheds[i].startTime && t <= scheds[i].endTime)
		{
			sum += kWPerHour;
		}
	}

	return sum;
}

bool DMS::isSLMValid(PowerSystemKnowledge& infos)
{
	bool constraintsRespected = true;

	// 2- contrainte voltage
	for (std::map<std::string, PowerSysNode>::const_iterator it = infos.nodes.begin(); it != infos.nodes.end(); ++it)
	{
		if ( ! (it->second.getVoltage() >= 0.95 && it->second.getVoltage() <= 1.05) && it->second.getVoltage() > 0)
		{
			constraintsRespected = false;
			break;
		}
	}

	// 3- contrainte load <= max load
	if (infos.totalLoad > (double)this->maximumPowerLoad * OpenDSS::Instance().V2GThresholdCoefficient)
	{
		constraintsRespected = false;
	}

	return constraintsRespected;
}

PEVSchedule DMS::schedulePEVSLM(string nodeId, double currentTime, double deadline, double durationInSeconds, double kWPerHour,
		const std::vector<PEVSchedule>& exceptScheds, const std::string& optimizationComparison)
{
	stringstream in (stringstream::in | stringstream::out);
	static RawStat stats = RawStat("schedulePEVSLM", "event", 1);

	PEVSchedule result;
	double startTime = -1;
	double minLosses = 1000000000;
	double minLoad = 1000000000;
	double maxUnusedSolar = -1;

	in << " begin sched slm node " << nodeId << endl;

	// Loop for all possible schedulings
	for (double tStart = currentTime; tStart + durationInSeconds <= deadline; tStart += slotInterval)
	{
		bool constraintsRespected = true;
		double sumCurLosses = 0;
		double nbCurLosses = 0;
		double sumCurLoad = 0;
		double nbCurLoad = 0;
		double sumUnusedSolar = 0;

		in << " S = " << tStart << endl;

		// for tStart, we need to calculate the cost of this scheduling
		for (double tSchedule = tStart; tSchedule <= tStart + durationInSeconds; tSchedule += slotInterval)
		{
			// This test is useful when SLM is slotted, where another schedule could be already set to nodeId at tSchedule
			if (currentlyChargingAt(tSchedule, nodeId) || currentlySchedAtIn(tSchedule, exceptScheds))
			{
				in << " 	SKIP, CHARGING, sched = " << tSchedule << " node id = " << nodeId <<  endl;
				constraintsRespected = false; // 3th constraint
				break;
			}

			in << " 	sched = " << tSchedule << endl;
			// 1-
			// COPY calc avec loads base, pev load en REF

			PowerSystemKnowledge infos = calcSysAt(tSchedule, currentTime);

			constraintsRespected = isSLMValid(infos);

			if ( ! constraintsRespected)
				break;

			// Update sum load and losses
			sumCurLoad += infos.totalLoad;
			sumCurLosses += infos.totalLosses;

			if (optimizationComparison == "solarAndLossesBased")
			{
				double cur = solarAvailableAt(tSchedule) - solarUsedAt(tSchedule, kWPerHour);

				if (cur < 0)
					cur = 0;

				sumUnusedSolar += cur;
			}

			nbCurLosses += 1;
			nbCurLoad += 1;

			in << " 	sumCurLoad = " << sumCurLoad << " node id = " << nodeId << endl;
			in << " 	sumCurLosses = " << sumCurLosses << " node id = " << nodeId << endl;
		}

		if ( ! constraintsRespected)
		{
			in << " 	CONSTRAINT NOT RESPECTED. " << " node id = " << nodeId << endl;
			continue;
		}

		sumCurLoad = (nbCurLoad > 0) ? (sumCurLoad / nbCurLoad) : 0;
		sumCurLosses = (nbCurLosses > 0) ? (sumCurLosses / nbCurLosses) : 0;

		// Optimization criteria:
		if (optimizationComparison == "lossesBased")
		{
			if (sumCurLoad < minLoad && sumCurLosses < minLosses && sumCurLoad > 0 && sumCurLosses > 0)
			{
				startTime = tStart;
				minLoad = sumCurLoad;
				minLosses = sumCurLosses;
			}
		}
		else
		if (optimizationComparison == "solarAndLossesBased")
		{
			if (sumUnusedSolar > maxUnusedSolar || (sumUnusedSolar == maxUnusedSolar && sumCurLosses < minLosses))
			{
				startTime = tStart;
				maxUnusedSolar = sumUnusedSolar;
				minLosses = sumCurLosses;
			}
		}
	}

	in << " 	FOUND FINAL startTime = " << startTime << " node id = " << nodeId << endl;

	//stats.log(simTime().dbl(), startTime, in.str());

	result.startTime = startTime;
	result.endTime = startTime + durationInSeconds;
	result.kW = kWPerHour;
	result.nodeId = nodeId;
	result.type = "charging";

	return result;
}

DMS::~DMS()
{
}
