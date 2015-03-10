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

#include "StatsCollector.h"
#include <map>
#include <vector>
#include <math.h>
#include <iostream>


using namespace std;

StatsCollector::StatsCollector()
{
	granularity = 2;
}

StatResult StatsCollector::result(const std::string& name, double time)
{
	terminateStatsOf(name, time);

	StatResult result;

	double xbar = mean(stats[name].batches);
	double dev = standardDeviation(stats[name].batches);
	double n = (double)stats[name].batches.size();

	result.min = (n == 0) ? 0 : xbar - 1.96 * (dev / sqrt(n));
	result.max = (n == 0) ? 0 : xbar + 1.96 * (dev / sqrt(n));
	result.mu = (n == 0) ? 0 : (result.max - result.min) / 2.0 + result.min;

	return result;
}

void StatsCollector::setStatsWrittenOf(const std::string& name, bool isWritten)
{
	statsWritten[name] = isWritten;
}

bool StatsCollector::getStatsWrittenOf(const std::string& name)
{
	if (statsWritten.find(name) != statsWritten.end() && statsWritten[name])
	{
		return true;
	}

	return false;
}

double StatsCollector::mean(const std::vector<double>& values)
{
	double sum = 0;
	int nb = 0;

	for (int i = 0; i < (int)values.size(); ++i)
	{
		sum += values[i];
		++nb;
	}

	if (nb == 0)
		return 0;

	return sum / (double)nb;
}

double StatsCollector::standardDeviation(const std::vector<double>& values)
{
	double mu = mean(values);

	cout << " mu = " << mu << endl;

	double sum = 0;
	int nb = 0;

	for (int i = 0; i < (int)values.size(); ++i)
	{
		double cur = values[i] - mu;

		cur = cur * cur;

		sum += cur;
		++nb;
	}

	if (sum == 0)
		return 0;

	return sqrt(sum / (double)nb);
}

StatsCollector::~StatsCollector()
{
	// TODO Auto-generated destructor stub
}

StatsCollector& StatsCollector::Instance()
{
	static StatsCollector s;

	return s;
}

void StatsCollector::terminateStatsOf(const std::string& name, double time)
{
	if (stats[name].nb > 0 && time - stats[name].timeBegin > 0)
	{
		double x_i = 0;

		if (statInfos[name] == STAT_TYPE_MEAN)
			x_i = stats[name].sum / (double)stats[name].nb;
		else
		if (statInfos[name] == STAT_TYPE_PER_SECOND)
			x_i = stats[name].sum / (double)(time - stats[name].timeBegin);

		stats[name].batches.push_back(x_i);

		stats[name].nb = 0;
		stats[name].sum = 0.0;
	}
}

void StatsCollector::addStat(const std::string& name, StatType type, double time, double value)
{
	if (stats[name].nb == 0)
	{
		stats[name].timeBegin = time;
	}

	stats[name].sum += value;
	++stats[name].nb;

	statInfos[name] = type;

	if (stats[name].nb >= granularity)
	{
		terminateStatsOf(name, time);
	}
}
