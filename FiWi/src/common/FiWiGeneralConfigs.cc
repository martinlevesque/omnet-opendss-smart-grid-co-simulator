//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include <omnetpp.h>
#include "EPON_CtrlInfo.h"
#include "FiWiGeneralConfigs.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>
#include "OpenDSS.h"
#include <map>
#include <sstream>
#include <vector>
#include "DMS.h"
#include "MyUtil.h"

using namespace std;

Define_Module(FiWiGeneralConfigs);

std::map<RowAggregatorToNode, std::set<RowAggregatorToNode, RowAggregatorToNodeComparator>, RowAggregatorToNodeComparator > FiWiGeneralConfigs::aggregatorInformation;

double FiWiGeneralConfigs::eponPollingCycleTime = 0.000096;

double FiWiGeneralConfigs::PONDownstreamDelaySum = 0;
double FiWiGeneralConfigs::PONDownstreamDelayCnt = 0;
double FiWiGeneralConfigs::PONUpstreamDelaySum = 0;
double FiWiGeneralConfigs::PONUpstreamDelayCnt = 0;
double FiWiGeneralConfigs::WMNAverageDelaySum = 0;
double FiWiGeneralConfigs::WMNAverageDelayCnt = 0;
double FiWiGeneralConfigs::FiWiThroughputSum = 0;
int FiWiGeneralConfigs::aggregationTresholdBytes = 0;
double FiWiGeneralConfigs::OfferedLoadSum = 0;
std::map<int, StatClassTraffic> FiWiGeneralConfigs::StatsTrafficClass;

std::map<int, StatZone> FiWiGeneralConfigs::wiZoneStats;

std::map<int, std::map<MACAddress, StatStationTraffic> > FiWiGeneralConfigs::StatsStationTraffic;


void FiWiGeneralConfigs::initialize(int stage)
{
	if (stage != 5)
		return;

    ponVersion = par("ponVersion");
    simulationType = string(par("simulationType").stringValue());
    FiWiGeneralConfigs::eponPollingCycleTime = par("pollingCycleTime").doubleValue();
    EV << "In class FiWiGeneralConfigs::eponPollingCycleTime = " << FiWiGeneralConfigs::eponPollingCycleTime << endl;

    powerSystemAggregatorsFile = string(par("powerSystemAggregatorsFile").stringValue());
    fiwiPowerNodeMappingsFile = string(par("fiwiPowerNodeMappings").stringValue());
    dmsExpectedLoadProfileFile = string(par("dmsExpectedLoadProfileFile").stringValue());

    double V2GThresholdCoefficient = par("V2GThresholdCoefficient").doubleValue();

    OpenDSS::Instance().V2GThresholdCoefficient = V2GThresholdCoefficient;

    openDSSConfiguration = string(par("openDSSConfiguration").stringValue());

    OpenDSS::Instance().openDSSConfiguration = openDSSConfiguration;

    DMS::Instance().setReactiveControlAlgorithm(par("ReactiveControlAlgorithm").stringValue());

    // ReactiveControlSensorType
    OpenDSS::Instance().setReactiveControlSensorType(par("ReactiveControlSensorType").stringValue());
    OpenDSS::Instance().setReactiveControlSensorThreshold(par("ReactiveControlSensorThreshold").doubleValue());

    //////////////////////////////////////////////////
    // Stuff related to the power system:
    //
    if (simulationType == "withPowerSystem")
    {
		//////////////
		// DMS PROFILE
		ifstream dmsProfileFile;

		dmsProfileFile.open(dmsExpectedLoadProfileFile.c_str());

		if (dmsProfileFile.is_open())
		{
			string firstNode = "";

			while ( ! dmsProfileFile.eof())
			{
				double time;
				string powerNodeId;
				double load;

				dmsProfileFile >> time;
				dmsProfileFile >> powerNodeId;
				dmsProfileFile >> load;

				if (powerNodeId == "")
				{
					break;
				}

				EV << "DMS PROFILE.. " << time << "  node = " << powerNodeId << " load = " << (load) << endl;

				if (firstNode == "")
					firstNode = powerNodeId;

				// Times are in hour
				//const int nbDays = 50;
				//const int nbSecondsPerHour = 4;

				//for (int d = 0; d < nbDays; ++d)
				//{
					//for (int h = 0; h <= nbSecondsPerHour-1; ++h)
					//{
						// 4: 4 seconds per hour, erf should use the parameter in fiwitrafgen
						//double adjustedTime = 2 + (d * 24 * nbSecondsPerHour + time * nbSecondsPerHour + h);

						DMS::Instance().setProfile(powerNodeId, time, load);
						EV << "load="<<load<<" time = " << time << " power node = " << powerNodeId << endl;



			}

			dmsProfileFile.close();

			// UPDATE THE V2G BEGIN TIME
			if (OpenDSS::Instance().V2GThresholdCoefficient < 1)
			{
				for (int i = 0; i < 200; ++i)
				{
					// Find begin v2g:
					if (DMS::Instance().getExpectedLoadAt(firstNode, (double)i) >= OpenDSS::Instance().V2GThresholdCoefficient * 2.0 &&
							DMS::Instance().getV2gBeginAt() <= 0)
					{
						DMS::Instance().setV2gBeginAt((double)i);
					}
					else
					// Then find end v2g
					if (DMS::Instance().getExpectedLoadAt(firstNode, (double)i) < OpenDSS::Instance().V2GThresholdCoefficient * 2.0 &&
							DMS::Instance().getV2gBeginAt() > 0)
					{
						DMS::Instance().setV2gEndAt((double)i);
						break;
					}
				}
			}

			EV << "V2G BEGIN TIME = " << DMS::Instance().getV2gBeginAt() << endl;
			EV << "V2G END TIME = " << DMS::Instance().getV2gEndAt() << endl;
			EV << "V2G first node = " << firstNode << endl;
		}
		else
		{
			error("FiWiGeneralConfigs::initialize can't read dmsProfileFile");
		}

		////////////////
		// FIWI <-> POWER nodes mappings
		ifstream fiwiPowerNodeMappings;

		fiwiPowerNodeMappings.open(fiwiPowerNodeMappingsFile.c_str());

		if (fiwiPowerNodeMappings.is_open())
		{
			while ( ! fiwiPowerNodeMappings.eof())
			{
				string fiwiNode;
				string powerNode;

				fiwiPowerNodeMappings >> fiwiNode;
				fiwiPowerNodeMappings >> powerNode;

				if (fiwiNode == "" || powerNode == "")
				{
					break;
				}

				// resolveDestMACAddress(aggregator.nodeName);
				MACAddress fiwiAddr = resolveDestMACAddress(fiwiNode);
				OpenDSS::Instance().setMapping(fiwiAddr, powerNode);

				EV << "FiWiGeneralConfigs - mapped " << fiwiAddr << " to " << powerNode << endl;
			}

			fiwiPowerNodeMappings.close();


		}
		else
		{
			error("FiWiGeneralConfigs::initialize can't read fiwiPowerNodeMappings file");
		}

		////////////////
		// Map aggregators to PEVs.
		// <Aggregator>  to <Node>
		// <Network node 1> <Network node 2>
		ifstream aggregatorStreamFile;

		aggregatorStreamFile.open(powerSystemAggregatorsFile.c_str());

		if (aggregatorStreamFile.is_open())
		{
			vector<double> timesWithBaseLoadEvents;

			while ( ! aggregatorStreamFile.eof())
			{
				RowAggregatorToNode aggregator;
				aggregator.isAggregator = true;
				RowAggregatorToNode otherNode;
				otherNode.isAggregator = false;

				aggregatorStreamFile >> aggregator.nodeName;
				aggregatorStreamFile >> otherNode.nodeName;

				if (aggregator.nodeName == "" || otherNode.nodeName == "")
				{
					break;
				}

				aggregator.addr = resolveDestMACAddress(aggregator.nodeName);
				otherNode.addr = resolveDestMACAddress(otherNode.nodeName);

				EV << "Adding aggregator group " << aggregator.nodeName << " (" << aggregator.addr << ") " << otherNode.nodeName
						<< " (" << otherNode.addr << ")" << endl;

				aggregatorInformation[aggregator].insert(otherNode);
				EV << "size aggres = " << aggregatorInformation.size() << endl;
			}
		}
		else
		{
			error("FiWiGeneralConfigs::initialize can't the aggregator file conf");
		}

		aggregatorStreamFile.close();
    }
}

void FiWiGeneralConfigs::handleMessage(cMessage *msg)
{


	delete msg;
}

MACAddress FiWiGeneralConfigs::resolveDestMACAddress(const string& addr)
{
    return MyUtil::resolveDestMACAddress(this, addr);
}

void FiWiGeneralConfigs::finish ()
{
	if (simulationType == "withPowerSystem")
	{
		RawStat s = RawStat("PEVNbDrops", "event", 1);
		s.addStat(simTime().dbl(), DMS::Instance().getNbDrops(), 0);
	}

	// FiWi stats
	RawStat fiWiStats = RawStat("FiWiStats", "event", 1);
	stringstream fiWiStatsLine;
	double downstream = (PONDownstreamDelayCnt > 0) ? PONDownstreamDelaySum / PONDownstreamDelayCnt : 0;
	double upstream = (PONUpstreamDelayCnt > 0) ? PONUpstreamDelaySum / PONUpstreamDelayCnt : 0;
	double WMN = (WMNAverageDelayCnt > 0) ? WMNAverageDelaySum / WMNAverageDelayCnt : 0;
	double totalDelay = downstream + upstream + WMN;
	fiWiStatsLine << downstream << " " << upstream << " " << WMN << " " << totalDelay << " " << OfferedLoadSum / (simTime().dbl() - 2.0);
	fiWiStats.log(simTime().dbl(), FiWiThroughputSum / (simTime().dbl() - 2.0), fiWiStatsLine.str());

	// FiWi stats with QoS traffic classes
	double meanOverallOfferedLoad = 0;

	// TODO change.. must be calculated for each zone

	for (std::map<int, StatClassTraffic>::iterator it = StatsTrafficClass.begin(); it != StatsTrafficClass.end(); ++it)
	{
		// Stats for each given traffic class
		// Offered load | Throughput | Delay | mean contention res duration
		stringstream fiwiName;
		fiwiName << string("FiWi") << it->first;
		RawStat s = RawStat(fiwiName.str(), "event", 1);

		stringstream line;

		double meanOfferedLoad = (it->second.cntOfferedLoads > 0) ? it->second.sumOfferedLoads / (simTime().dbl() - 2.0) : 0;
		double meanThroughput = (it->second.cntThroughputs > 0) ? it->second.sumThroughputs / (simTime().dbl() - 2.0) : 0;
		double meanDelay = (it->second.cntDelays > 0) ? it->second.sumDelays / it->second.cntDelays : 0;

		meanOverallOfferedLoad += meanOfferedLoad;

		line << meanOfferedLoad << " " << meanThroughput << " " << meanDelay;

		s.log(simTime().dbl(), 0, line.str());
	}

	// PER STATION STATS:
	// std::map<std::string, std::map<int, StatStationTraffic> > StatsStationTraffic
	/*
	for (std::map<int, std::map<MACAddress, StatStationTraffic> >::iterator it = StatsStationTraffic.begin(); it != StatsStationTraffic.end(); ++it)
	{
		for (std::map<MACAddress, StatStationTraffic>::iterator itSTA = it->second.begin(); itSTA != it->second.end(); ++itSTA)
		{
			// Stats for each station
			// Offered load | Psucc | q_k | load | throughput | delay

			stringstream fileName;
			fileName << "STA-" << it->first << "-" << itSTA->first;

			RawStat s = RawStat(fileName.str(), "event", 1);

			double psucc = (itSTA->second.nbCycleTransmissionAttempts > 0) ? itSTA->second.nbCycleTransmissionSuccesses / itSTA->second.nbCycleTransmissionAttempts : 0;

			int z = MyUtil::getRoutingTable(this)->zoneOf(itSTA->first);

			double meanOfferedLoad = (itSTA->second.cntOfferedLoads > 0) ? itSTA->second.sumOfferedLoads / (simTime().dbl() - 2.0) : 0;
			double meanThroughput = (itSTA->second.cntThroughputs > 0) ? itSTA->second.sumThroughputs / (simTime().dbl() - 2.0) : 0;
			double meanDelay = (itSTA->second.cntDelays > 0) ? itSTA->second.sumDelays / itSTA->second.cntDelays : 0;

			ASSERT(z >= 0);

			if (z < 0)
				continue;

			double qk = (wiZoneStats[z].wiNbCycles > 0) ? itSTA->second.nbCycleTransmissionAttempts / wiZoneStats[z].wiNbCycles : 0;

			EV << "wiZoneStats[z].wiNbCycles = " << wiZoneStats[z].wiNbCycles << endl;
			EV << "itSTA->second.nbCycleTransmissionAttempts = " << itSTA->second.nbCycleTransmissionAttempts << endl;
			EV << "itSTA->second.nbCycleTransmissionSuccesses = " << itSTA->second.nbCycleTransmissionSuccesses << endl;

			stringstream line;

			line << meanOverallOfferedLoad << " " << psucc << " " << qk << " " << meanOfferedLoad << " " << meanThroughput << " " << meanDelay;

			s.log(simTime().dbl(), 0, line.str());
		}
	}
	*/



	/*
	// wiZoneStats
	for (std::map<int, StatZone>::iterator it = wiZoneStats.begin(); it != wiZoneStats.end(); ++it)
	{
		// Stats for each given WI ZONE
		// Offered load | cycle len
		stringstream fileName;
		fileName << "ZoneStats" << it->first << "";

		RawStat s = RawStat(fileName.str(), "event", 1);

		int zone = it->first;

		if (zone < 0)
			continue;

		double meanCycleLen = (it->second.nbCycleLengths > 0) ? it->second.sumCycleLengths / it->second.nbCycleLengths : 0.0;
		double meanContentionResDuration = (wiZoneStats[zone].wiNbCycles > 0) ? it->second.sumContentionResDuration / (wiZoneStats[it->first].wiNbCycles * (double)MyUtil::getRoutingTable(this)->wiMacZones[zone].size()) : 0.0;

		EV << "it->second.sumContentionResDuration = " << it->second.sumContentionResDuration << " meanContentionResDuration = " << meanContentionResDuration << endl;

		stringstream l;
		l << meanOverallOfferedLoad << " " << meanCycleLen << " " << meanContentionResDuration;

		s.log(simTime().dbl(), 0, l.str());
	}
	*/

	// VOLTAGE
	double minimumVoltage = 100000000;
	string nodeWithMinimumVoltage = "";

	for (std::map<double, PowerSystemKnowledge>::const_iterator it = OpenDSS::Instance().history.begin(); it != OpenDSS::Instance().history.end(); ++it)
	{
		for (std::map<std::string, PowerSysNode>::const_iterator itNodes = it->second.nodes.begin(); itNodes != it->second.nodes.end(); ++itNodes)
		{
			if (itNodes->second.getVoltage() < minimumVoltage && itNodes->second.getVoltage() > 0)
			{
				minimumVoltage = itNodes->second.getVoltage();
				nodeWithMinimumVoltage = itNodes->first;
			}
		}
	}

	RawStat statsVoltage("Voltage", "event", 1);

	for (std::map<double, PowerSystemKnowledge>::const_iterator it = OpenDSS::Instance().history.begin(); it != OpenDSS::Instance().history.end(); ++it)
	{
		for (std::map<std::string, PowerSysNode>::const_iterator itNodes = it->second.nodes.begin(); itNodes != it->second.nodes.end(); ++itNodes)
		{
			if (itNodes->first == nodeWithMinimumVoltage)
			{
				statsVoltage.log(it->first, itNodes->second.getVoltage(), nodeWithMinimumVoltage);
			}
		}
	}

	// LV LOAD

	RawStat statsLvLoads("LVLoads", "event", 1);

	for (std::map<double, PowerSystemKnowledge>::const_iterator it = OpenDSS::Instance().history.begin(); it != OpenDSS::Instance().history.end(); ++it)
	{
		map<string, double> lvLoads;

		// Init lv loads to 0
		set<string> lvs = OpenDSS::Instance().getLVs();
		for (std::set<std::string>::iterator itInit = lvs.begin(); itInit != lvs.end(); ++itInit)
		{
			if (*itInit == "")
				continue;

			lvLoads[*itInit] = 0;
		}

		for (std::map<std::string, PowerSysNode>::const_iterator itNodes = it->second.nodes.begin(); itNodes != it->second.nodes.end(); ++itNodes)
		{
			double load = itNodes->second.getPEVLoad() + itNodes->second.getBaseLoad();

			string lvName = OpenDSS::Instance().LVNameOf(itNodes->first);

			if (lvName == "")
				continue;

			lvLoads[lvName] += load;
		}

		for (map<string, double>::iterator itLv = lvLoads.begin(); itLv != lvLoads.end(); ++itLv)
		{
			if (itLv->first == "")
				continue;

			statsLvLoads.log(it->first, lvLoads[itLv->first], itLv->first);
		}
	}
}
