/*
 * RawStat.h
 *
 *  Created on: Nov 5, 2011
 *      Author: martin
 */

#include <string>
#include <vector>

#ifndef RAWSTAT_H_
#define RAWSTAT_H_

struct RawStatEntry
{
	RawStatEntry(double t, double v1, double v2)
	{
		time = t;
		value1 = v1;
		value2 = v2;
	}

	double time;
	double value1;
	double value2;
};

class RawStat
{
public:
	RawStat()
	{}

	RawStat(const std::string& name, const std::string& type, double batchLengthInSeconds);
	virtual ~RawStat();

	void addStat(double time, double value1, double value2);
	void log(double time, double value, std::string msg);

private:

	void writeLine(double time, double value, std::string msg);

	std::string name;
	std::string type;
	double batchLengthInSeconds;

	std::vector<RawStatEntry> currentBatch;
};

#endif /* RAWSTAT_H_ */
