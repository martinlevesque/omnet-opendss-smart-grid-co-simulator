/*
 * Copyright (C) 2003 Andras Varga; CTIE, Monash University, Australia
 *
 * This program is free software; you can redistribute it and/or
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

#ifndef __FIWI_TRAFFIC_MATRIX_H
#define __FIWI_TRAFFIC_MATRIX_H

//#include "INETDefs.h"
//#include "MACAddress.h"
#include <string>
#include <map>
#include <vector>

struct TrafficPair
{
	MACAddress src;
	MACAddress dest;
	double lambda;
	cMessage* nextPacketMsg;
	bool withRandomBehaviour;
	int trafficClass;

	double waitTime()
	{
		double wait;

		double randPoisson = 0;

		if (withRandomBehaviour)
		{
			randPoisson = (double)exponential(1.0 / (double)lambda);
			EV << "waitTime-- with random.. rand poisson = " << randPoisson << " lambda = " << lambda << endl;
		}
		else
		{
			randPoisson = lambda;
		}

		if (randPoisson <= 0)
		{
			randPoisson = 1;
		}

		wait = (double)randPoisson;

		return wait;
	}
};

/**
 *
 */
class FiWiTrafficMatrix : public cSimpleModule
{
public:
	std::vector<TrafficPair> getTrafficMatrix(MACAddress addr) { return trafficMatrix[addr]; }

	int getReqLength() { return reqLength; }

  protected:
    virtual void initialize(int stage);
    virtual int numInitStages() const {return 6;}
    virtual void handleMessage(cMessage *msg);
    virtual void finish();


  private:

    MACAddress resolveDestMACAddress(const std::string& addr);

    std::map<MACAddress, std::vector<TrafficPair> > trafficMatrix;
    std::string trafFile;
    int reqLength;
};

#endif


