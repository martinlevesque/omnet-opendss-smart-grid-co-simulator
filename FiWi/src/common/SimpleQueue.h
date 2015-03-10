/*
 * SimpleQueue.h
 *
 *  Created on: Aug 15, 2011
 *      Author: martin
 */


#include <omnetpp.h>
#include "EtherFrame_m.h"
#include "MyAbstractQueue.h"

#include <list>
#include <vector>

#ifndef SIMPLEQUEUE_H_
#define SIMPLEQUEUE_H_

class SimpleQueue : public MyAbstractQueue
{
	private:
	std::list<cPacket*> queue;
	uint32_t lengthsInBytes;

	public:
		SimpleQueue();
		SimpleQueue(uint32_t p_limit);
		virtual ~SimpleQueue();

		uint32_t length() { return queue.size(); }
		uint32_t lengthInBytes();
		bool isFull();
		bool isEmpty() { return length() == 0; }
		void enqueue(cPacket* p);
		void remove(cPacket* pkt);
		cPacket* front(int maximumPacketSize);
		void initialize(cModule* module);

		cPacket* popPacketLessThan(int size);
};

#endif /* SIMPLEQUEUE_H_ */
