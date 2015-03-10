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

#ifndef __MY_WEIGHTED_FAIR_QUEUE_H__
#define __MY_WEIGHTED_FAIR_QUEUE_H__

#include <omnetpp.h>
#include <map>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <queue>
#include "MACAddress.h"
#include "MyAbstractQueue.h"

struct WeightedFairQueuePacket
{
	cPacket* pkt;
	double arrivalTime;
};

struct WeightedFairQueuePktTypeObj
{
	double priority;
	int indexMin;
	int indexMax;

	WeightedFairQueuePktTypeObj(double p)
	{
		priority = p;
	}

	WeightedFairQueuePktTypeObj()
	{
		priority = 0;
	}
};

/**
 *
 */
class MyWeightedFairQueue : public MyAbstractQueue
{
public:
	MyWeightedFairQueue();

protected:
	int queueSizeInBytes;
    std::map<int, WeightedFairQueuePktTypeObj> pktTypePriorities;

    int sizeInBytes;

	// Queues per packet type
	std::map<int, std::vector<WeightedFairQueuePacket> > queues;
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
