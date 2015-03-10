/*
 * SimpleQueue.cc
 *
 *  Created on: Aug 15, 2011
 *      Author: martin
 */

#include "SimpleQueue.h"
#include <vector>
#include "ONUMacCtl.h"

SimpleQueue::SimpleQueue(uint32_t p_limit)
{
	lengthsInBytes = 0;
	limit = p_limit;
}

SimpleQueue::SimpleQueue()
{
	lengthsInBytes = 0;
	this->limit = 0;
}

SimpleQueue::~SimpleQueue()
{
}

void SimpleQueue::initialize(cModule* module)
{

}

void SimpleQueue::enqueue(cPacket* p)
{
	if ( ! isFull())
	{
		queue.push_back(p);
		lengthsInBytes += p->getByteLength();
	}
}

cPacket* SimpleQueue::front(int maximumPacketSize)
{
	if (length() > 0)
	{
		cPacket* p = queue.front();

		if (p && p->getByteLength() <= maximumPacketSize)

		return p;
	}

	return NULL;
}

void SimpleQueue::remove(cPacket* pkt)
{
	if (length() > 0)
	{
		cPacket* p = queue.front();
		queue.pop_front();

		lengthsInBytes -= p->getByteLength();
	}
}

uint32_t SimpleQueue::lengthInBytes()
{
	return lengthsInBytes;
}

cPacket* SimpleQueue::popPacketLessThan(int p_maximumSize)
{
	for (std::list<cPacket*>::iterator it = queue.begin(); it != queue.end(); ++it)
	{
		cPacket* p = *it;
		int curSize = ONUMacCtl::calcPacketSizeOnMedium(p->getByteLength());

		if (curSize <= p_maximumSize)
		{
			queue.erase(it);
			return p;
		}
	}

	return NULL;
}

bool SimpleQueue::isFull()
{
	return lengthsInBytes >= (int)limit;
}
