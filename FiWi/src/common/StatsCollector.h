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

#include <map>
#include <string>
#include <vector>

#ifndef STATSCOLLECTOR_H_
#define STATSCOLLECTOR_H_

struct StatResult
{
	double mu;
	double min;
	double max;
};

enum StatType
{
	STAT_TYPE_PER_SECOND,
	STAT_TYPE_MEAN
};

struct StatValue
{
	double sum;
	long nb;
	std::vector<double> batches;
	double timeBegin;

	StatValue()
	{
		sum = 0;
		nb = 0;
		timeBegin = 0;
	}
};

class StatsCollector
{
public:

	static StatsCollector& Instance();

	virtual ~StatsCollector();

	void addStat(const std::string& name, StatType type, double time, double value);

	StatResult result(const std::string& name, double time);

	void setStatsWrittenOf(const std::string& name, bool isWritten);
	bool getStatsWrittenOf(const std::string& name);

private:
	int granularity;

	double mean(const std::vector<double>& values);
	double standardDeviation(const std::vector<double>& values);
	void terminateStatsOf(const std::string& name, double time);

	// name -> time (sec) -> values
	std::map<std::string, StatValue> stats;
	std::map<std::string, StatType> statInfos;
	std::map<std::string, bool> statsWritten;

	StatsCollector();



};

#endif /* STATSCOLLECTOR_H_ */
