/*
 * RawStat.cc
 *
 *  Created on: Nov 5, 2011
 *      Author: martin
 */

#include <string>
#include <fstream>
#include <iomanip>

#include "RawStat.h"

using namespace std;

RawStat::RawStat(const std::string& name, const std::string& type, double batchLengthInSeconds)
{
	this->name = name;
	this->type = type;
	this->batchLengthInSeconds = batchLengthInSeconds;
}

void RawStat::log(double time, double value, string msg)
{
	writeLine(time, value, msg);
}

void RawStat::addStat(double time, double value1, double value2)
{
	if (type == "event")
	{
		writeLine(time, value1, "");
	}
	else
	if (type == "time-sum" || type == "time-mean")
	{
		currentBatch.push_back(RawStatEntry(time, value1, value2));

		if (currentBatch.size() > 0)
		{
			double firstTime = currentBatch[0].time;

			if (time - firstTime >= batchLengthInSeconds)
			{
				double sum = 0.0;

				for (int i = 0; i < (int)currentBatch.size(); ++i)
					sum += currentBatch[i].value1;

				if (type == "time-mean")
					writeLine(time, sum / (double)currentBatch.size(), "");
				else
				if (type == "time-sum")
					writeLine(time, sum, "");

				currentBatch.clear();
			}
		}
	}
	else
	if (type == "drop-ratio")
	{
		currentBatch.push_back(RawStatEntry(time, value1, value2));

		if (currentBatch.size() > 0)
		{
			double firstTime = currentBatch[0].time;

			if (time - firstTime >= batchLengthInSeconds)
			{
				double nbReceived = 0.0;
				double nbDropped = 0.0;

				for (int i = 0; i < (int)currentBatch.size(); ++i)
				{
					nbReceived += currentBatch[i].value1;
					nbDropped += currentBatch[i].value2;
				}

				double ratio = 0;

				if (nbReceived > 0)
				{
					ratio = nbDropped / nbReceived;
				}

				writeLine(time, ratio, "");

				currentBatch.clear();
			}
		}
	}
}

void RawStat::writeLine(double time, double value, string msg)
{
	fstream filestr;
	filestr.precision(8);
	filestr.open ((name + string(".txt")).c_str(), fstream::in | fstream::out | fstream::app);

	filestr << time << " " << value << " " << msg <<  endl;

	filestr.close();
}

RawStat::~RawStat()
{
	// TODO Auto-generated destructor stub
}
