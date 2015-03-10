// Martin Lévesque <levesquem@emt.inrs.ca>
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

#ifndef __FIWI_FAST_ADMISSION_CONTROL_H__
#define __FIWI_FAST_ADMISSION_CONTROL_H__

#include <omnetpp.h>
#include <map>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <queue>
//#include "MACAddress.h"
#include "MyAbstractQueue.h"

#define FAST_ADMISSION_CONTROL_GREEN 1
#define FAST_ADMISSION_CONTROL_RED 0
#define FAST_ADMISSION_CONTROL_STATUS_IDLE 0
#define FAST_ADMISSION_CONTROL_STATUS_TRANSMITTING 1

struct FastBucketInfos
{
	int lambda;
	double nbBytesAvailable;
	int capacity;
	int indexMin;
	int indexMax;
	double lastTokenUpdate;
	double fairnessStatus;
	double fairnessAlphaGreen;
	double fairnessAlphaRed;

	FastBucketInfos()
	{
		fairnessStatus = 1; // 1 -> best status.
		lastTokenUpdate = 0;
	}

	void updateFairnessStatus(int color);
	void updateTokens(int bytesPerToken);
};

struct FastAdmissionControlPacket
{
	cPacket* pkt;
	int color; // green = 1, red = 0, highest is the better
	double priority; // highest is the better, host priority
	double arrivalTime;
};



struct FastTokenForPktTypeObj
{
	double priority;
	int indexMin;
	int indexMax;

	FastTokenForPktTypeObj(double p)
	{
		priority = p;
	}

	FastTokenForPktTypeObj()
	{
		priority = 0;
	}
};

/**
 *
 */
class FiWiFastAdmissionControl : public MyAbstractQueue
{
public:
	FiWiFastAdmissionControl();

	void setConfFile(const std::string& f) { cfgFile = f; };

protected:

	std::string cfgFile;
	int bytesPerToken;
	int queueSizeInBytes;
	double fairnessThreshold;
    double fairnessAlphaGreen;
    double fairnessAlphaRed;
    std::map<int, FastTokenForPktTypeObj> pktTypePriorities;

    int sizeInBytes;

	// Queues per packet type
	std::map<int, std::vector<FastAdmissionControlPacket> > queues;

	std::map<std::string, std::map<int, FastBucketInfos> > configs;
	std::set<std::string> hosts;
	std::set<int> pktTypes;

	virtual void initialize(cModule* thisModule);

    virtual void enqueue(cPacket* pkt);

    virtual uint32_t length();
	virtual uint32_t lengthInBytes();
	virtual bool isFull() { return false; }
	virtual bool isEmpty() { return queuesEmpty(); }
	virtual void remove(cPacket* pkt);
	virtual cPacket* front(int maximumPacketSize);

    //virtual void processAnotherPacket();
    virtual bool queuesEmpty();
    virtual bool queueFullWith(int nbBytesPkt, int pktType);
};

#endif
