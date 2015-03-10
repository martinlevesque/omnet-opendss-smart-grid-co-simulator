// Author: Martin Lévesque <levesquem@emt.inrs.ca>
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

#include <fstream>
#include <string>
#include <vector>
#include "FiWiMessages_m.h"
#include <algorithm>
#include <fstream>
#include <utility>

#include "MyWeightedFairQueue.h"
#include "FiWiTrafGen.h"
#include "MyUtil.h"

using namespace std;

struct myclass {
    bool operator()(const WeightedFairQueuePacket& p1, const WeightedFairQueuePacket& p2)
    {
    	if (p1.arrivalTime < p2.arrivalTime)
    		return true;

    	return false;
    }
} WeightedFairQueueCompare;


void MyWeightedFairQueue::initialize(cModule* thisModule)
{
	queueSizeInBytes = this->limit;
}

MyWeightedFairQueue::MyWeightedFairQueue()
{
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_SMART_GRID_CONTROL] = WeightedFairQueuePktTypeObj(0.4);
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_SMART_GRID_CONTROL].indexMin = 0;
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_SMART_GRID_CONTROL].indexMax = 40;
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_SMART_GRID_NOTIFICATION] = WeightedFairQueuePktTypeObj(0.3);
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_SMART_GRID_NOTIFICATION].indexMin = 40;
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_SMART_GRID_NOTIFICATION].indexMax = 70;
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_BEST_EFFORT] = WeightedFairQueuePktTypeObj(0.02);
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_BEST_EFFORT].indexMin = 70;
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_BEST_EFFORT].indexMax = 71;
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_VIDEO] = WeightedFairQueuePktTypeObj(0.14);
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_VIDEO].indexMin = 71;
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_VIDEO].indexMax = 86;
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_VOICE] = WeightedFairQueuePktTypeObj(0.14);
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_VOICE].indexMin = 86;
	pktTypePriorities[FIWI_TRAF_PKT_TYPE_VOICE].indexMax = 100;

	pktTypes.insert(FIWI_TRAF_PKT_TYPE_SMART_GRID_CONTROL);
	pktTypes.insert(FIWI_TRAF_PKT_TYPE_SMART_GRID_NOTIFICATION);
	pktTypes.insert(FIWI_TRAF_PKT_TYPE_BEST_EFFORT);
	pktTypes.insert(FIWI_TRAF_PKT_TYPE_VIDEO);
	pktTypes.insert(FIWI_TRAF_PKT_TYPE_VOICE);

	sizeInBytes = 0;
}

bool MyWeightedFairQueue::queueFullWith(int nbBytesPkt, int pktType)
{
	int nbBytes = 0;

	for (int i= 0; i < (int)queues[pktType].size(); ++i)
	{
		nbBytes += queues[pktType][i].pkt->getByteLength();
	}

	double sizeQueue = queueSizeInBytes / (double)pktTypePriorities.size();

	return nbBytes + nbBytesPkt > sizeQueue;
}

bool MyWeightedFairQueue::queuesEmpty()
{
	for (std::map<int, WeightedFairQueuePktTypeObj>::iterator it = this->pktTypePriorities.begin(); it != this->pktTypePriorities.end(); ++it)
	{
		int packetType = it->first;

		if ( ! queues[packetType].empty())
			return false;
	}

	return true;
}

void MyWeightedFairQueue::remove(cPacket* pkt)
{
	for (std::map<int, WeightedFairQueuePktTypeObj>::iterator it = this->pktTypePriorities.begin(); it != this->pktTypePriorities.end(); ++it)
	{
		int packetType = it->first;

		if ( ! queues[packetType].empty() && queues[packetType].begin()->pkt == pkt)
		{
			queues[packetType].erase(queues[packetType].begin()); //.pop();
			sort(queues[packetType].begin(), queues[packetType].end(), WeightedFairQueueCompare);

			sizeInBytes -= pkt->getByteLength();

			return;
		}
	}
}

cPacket* MyWeightedFairQueue::front(int maxPacketSize)
{
	EV << "WeightedFairQueue::front 1 " << endl;
	if (queuesEmpty())
		return NULL;

	EV << "WeightedFairQueue::front 2 " << endl;

	bool atLeastOnePacketMin = false;

	for (std::map<int, WeightedFairQueuePktTypeObj>::iterator it = this->pktTypePriorities.begin(); it != this->pktTypePriorities.end(); ++it)
	{
		int packetType = it->first;

		if ( ! queues[packetType].empty() && queues[packetType][0].pkt->getByteLength() <= maxPacketSize)
		{
			atLeastOnePacketMin = true;
			break;
		}
	}

	if ( ! atLeastOnePacketMin)
	{
		return NULL;
	}

	// At least one packet somewhere, let's get the next packet randomly:
	bool foundPacket = false;

	do
	{
		int indexRand = (int)uniform(0, 100);

		EV << "FiWiAdmissionControl::front 3 random index " << indexRand << endl;

		for (std::map<int, WeightedFairQueuePktTypeObj>::iterator it = this->pktTypePriorities.begin(); it != this->pktTypePriorities.end(); ++it)
		{
			int packetType = it->first;

			// Not empty and random matching queue indexes
			if ( ! queues[packetType].empty() && indexRand >= it->second.indexMin && indexRand < it->second.indexMax &&
					queues[packetType][0].pkt->getByteLength() <= maxPacketSize)
			{
				foundPacket = true;
				WeightedFairQueuePacket pktToSend = queues[packetType][0];
				queues[packetType].erase(queues[packetType].begin()); //.pop();
				sort(queues[packetType].begin(), queues[packetType].end(), WeightedFairQueueCompare);

				return pktToSend.pkt;
			}
		}
	}
	while( ! foundPacket);

	return NULL;
}

uint32_t MyWeightedFairQueue::length()
{
	uint32_t cnt = 0;

	for (std::map<int, WeightedFairQueuePktTypeObj>::iterator it = this->pktTypePriorities.begin(); it != this->pktTypePriorities.end(); ++it)
	{
		cnt += queues[it->first].size();
	}

	return cnt;
}

uint32_t MyWeightedFairQueue::lengthInBytes()
{
	return sizeInBytes;
}

void MyWeightedFairQueue::enqueue(cPacket *msg)
{
	EtherFrame* pkt = dynamic_cast<EtherFrame*>(msg);

	int packetType = pkt->getPktType();

	WeightedFairQueuePacket admPkt;
	admPkt.pkt = pkt;
	admPkt.arrivalTime = simTime().dbl();

	// Queue full ?
	if (queueFullWith(pkt->getByteLength(), packetType))
	{
		// We insert it and remove the less prioritized packet
		queues[packetType].push_back(admPkt);
		sort(queues[packetType].begin(), queues[packetType].end(), WeightedFairQueueCompare);
		sizeInBytes += admPkt.pkt->getByteLength();

		WeightedFairQueuePacket p = queues[packetType].back();
		queues[packetType].pop_back();
		sizeInBytes -= p.pkt->getByteLength();

		delete p.pkt;
	}
	else
	{
		queues[packetType].push_back(admPkt);
		sort(queues[packetType].begin(), queues[packetType].end(), WeightedFairQueueCompare);
		sizeInBytes += admPkt.pkt->getByteLength();
	}

	EV << "received " << pkt->getHost() << " pkt type = " << pkt->getPktType() << endl;
}
