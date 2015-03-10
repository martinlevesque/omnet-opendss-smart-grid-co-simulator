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

#ifndef __FIWI_ADMISSION_CONTROL_H__
#define __FIWI_ADMISSION_CONTROL_H__

#include <omnetpp.h>
#include <map>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <queue>
//#include "MACAddress.h"

#define ADMISSION_CONTROL_GREEN 1
#define ADMISSION_CONTROL_RED 0
#define ADMISSION_CONTROL_STATUS_IDLE 0
#define ADMISSION_CONTROL_STATUS_TRANSMITTING 1

struct BucketInfos
{
	int lambda;
	long nbBytesAvailable;
	int capacity;
	int indexMin;
	int indexMax;
	cMessage* nextTokenMsg;
	double fairnessStatus;
	double fairnessAlphaGreen;
	double fairnessAlphaRed;

	BucketInfos()
	{
		fairnessStatus = 1; // 1 -> best status.
	}

	void updateFairnessStatus(int color);
};

struct AdmissionControlPacket
{
	cPacket* pkt;
	int color; // green = 1, red = 0, highest is the better
	double priority; // highest is the better, host priority
	double arrivalTime;
};

struct myclass {
    bool operator()(const AdmissionControlPacket& p1, const AdmissionControlPacket& p2)
    {
    	// Color priority
    	if (p1.color > p2.color)
    		return true;

    	// Host priority
    	if (p1.color == p2.color && p1.priority > p2.priority)
    	    return true;

    	// Arrival time priority, oldest first
    	if (p1.color == p2.color && p1.priority == p2.priority && p1.arrivalTime < p2.arrivalTime)
    		return true;

    	return false;
    }
} CompareAdmissionControlPacket;

struct TokenForPktTypeObj
{
	int nbTokens;
	int indexMin;
	int indexMax;

	TokenForPktTypeObj()
	{
		nbTokens = 0;
	}

};

struct AdmissionStats
{
	double nbPktsArrived;
	double nbPktsDropped;
	double nbPktsEarlyDropped;
	double nbPktsReceived;
	double sumDelays;
	double totalNbInSystem;

	AdmissionStats()
	{
		nbPktsArrived = 0;
		nbPktsDropped = 0;
		nbPktsEarlyDropped = 0;
		nbPktsReceived = 0;
		sumDelays = 0;
		totalNbInSystem = 0;
	}
};

/**
 *
 */
class FiWiAdmissionControl : public cSimpleModule
{
public:

protected:

	std::string cfgFile;
	int bytesPerToken;
	double mu;
	int statusService;
	int queueSizeInBytes;
	double fairnessThreshold;
    double fairnessAlphaGreen;
    double fairnessAlphaRed;

	AdmissionControlPacket packetToSend;

	cMessage* transmissionMsg;

	// Queues per packet type
	std::map<std::string, std::vector<AdmissionControlPacket> > queues;

	std::map<std::string, std::map<std::string, BucketInfos> > configs;
	std::set<std::string> hosts;
	std::set<std::string> pktTypes;
	std::map<std::string, TokenForPktTypeObj> tokensForPktType;
	std::map<std::string, std::map<std::string, std::map<int, AdmissionStats> > > stats;

	virtual void initialize(int stage);
	virtual int numInitStages() const {return 6;}

    virtual void handleMessage(cMessage *msg);
    virtual void finish();
    void updateNbInSystem();

    void processAnotherPacket();
    bool queuesEmpty();
    bool queueFullWith(int nbBytesPkt, const std::string& pktType);
};



#endif
