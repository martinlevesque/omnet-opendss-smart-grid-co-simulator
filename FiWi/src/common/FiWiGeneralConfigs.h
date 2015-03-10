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

#ifndef __FIWI_GENERAL_CONFIGS_H__
#define __FIWI_GENERAL_CONFIGS_H__

#include <omnetpp.h>
#include <map>
#include <string>
#include <vector>
#include <list>
#include <string>
#include "MACAddress.h"



struct RowAggregatorToNode
{
	std::string nodeName; // Omnet name
	MACAddress addr;
	bool isAggregator;

	bool operator==(const RowAggregatorToNode& other) const
	{
		return addr == other.addr;
	}
};

struct RowAggregatorToNodeComparator
{
  bool operator() (const RowAggregatorToNode& n1, const RowAggregatorToNode& n2) const
  {
	  return n1.addr < n2.addr;
  }
};

struct StatClassTraffic
{
	StatClassTraffic()
	{
		sumDelays = 0;
		cntDelays = 0;
		sumThroughputs = 0;
		cntThroughputs = 0;
		sumOfferedLoads = 0;
		cntOfferedLoads = 0;
		sumContentionResDuration = 0;

	}

	double sumDelays;
	double cntDelays;
	double sumThroughputs;
	double cntThroughputs;
	double sumOfferedLoads;
	double cntOfferedLoads;
	double sumContentionResDuration;
};

struct StatStationTraffic
{
	StatStationTraffic()
	{
		sumDelays = 0;
		cntDelays = 0;
		sumThroughputs = 0;
		cntThroughputs = 0;
		sumOfferedLoads = 0;
		cntOfferedLoads = 0;
		nbCycleTransmissionAttempts = 0;
		nbCycleTransmissionSuccesses = 0;
	}

	double nbCycleTransmissionAttempts;
	double nbCycleTransmissionSuccesses;
	double sumDelays;
	double cntDelays;
	double sumThroughputs;
	double cntThroughputs;
	double sumOfferedLoads;
	double cntOfferedLoads;
};

struct StatZone
{
	StatZone()
	{
		sumCycleLengths = 0;
		nbCycleLengths = 0;
		sumContentionResDuration = 0;
		wiNbCycles = 0;
	}

	double sumCycleLengths;
	double nbCycleLengths;

	double sumContentionResDuration;
	double wiNbCycles;
};

/**
 *
 */
class FiWiGeneralConfigs : public cSimpleModule
{
public:
	static double eponPollingCycleTime;
	static bool DEBUG_MODE() { return true; }

	int getPonVersion() { return ponVersion; }
	std::string getSimulationType() { return simulationType; }

	// FiWi stats
	static double PONDownstreamDelaySum;
	static double PONDownstreamDelayCnt;
	static double PONUpstreamDelaySum;
	static double PONUpstreamDelayCnt;
	static double WMNAverageDelaySum;
	static double WMNAverageDelayCnt;
	static double FiWiThroughputSum;
	static double OfferedLoadSum;

	static std::map<int, StatZone> wiZoneStats; // FOR EACH ZONE

	static std::map<int, StatClassTraffic> StatsTrafficClass;

	static std::map<int, std::map<MACAddress, StatStationTraffic> > StatsStationTraffic;

	static int aggregationTresholdBytes;

	static void updatePONDownstreamDelay(double delay)
	{
		PONDownstreamDelaySum += delay;
		++PONDownstreamDelayCnt;
	}

	static void updatePONUpstreamDelay(double delay)
	{
		PONUpstreamDelaySum += delay;
		++PONUpstreamDelayCnt;
	}

	static void updateWMNAverageDelay(double delay)
	{
		WMNAverageDelaySum += delay;
		++WMNAverageDelayCnt;
	}

	static void updateThroughput(double bits)
	{
		FiWiThroughputSum += bits;
	}
	// End FiWi stats

	static std::map<RowAggregatorToNode, std::set<RowAggregatorToNode, RowAggregatorToNodeComparator>, RowAggregatorToNodeComparator > aggregatorInformation;

protected:



	virtual void initialize(int stage);
	virtual int numInitStages() const {return 6;}

	int ponVersion;
	std::string simulationType;

	std::string openDSSConfiguration;

	std::string powerSystemAggregatorsFile;
	std::string fiwiPowerNodeMappingsFile;
	std::string PEVArrivalsFile;
	std::string dmsExpectedLoadProfileFile;


    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    MACAddress resolveDestMACAddress(const std::string& addr);
};



#endif
