/*
 * AbstractQueue.h
 *
 *  Created on: Feb 7, 2012
 *      Author: martin
 */


#include <omnetpp.h>
//#include "EtherFrame_m.h"

#include <list>
#include <vector>

#ifndef MY_ABSTRACT_QUEUE_H_
#define MY_ABSTRACT_QUEUE_H_

class MyAbstractQueue
{
	protected:
		double limit;
	public:
		virtual uint32_t length() = 0;
		virtual uint32_t lengthInBytes() = 0;
		virtual bool isFull() = 0;
		virtual bool isEmpty() = 0;
		virtual void enqueue(cPacket* p) = 0;
		virtual cPacket* front(int maximumPacketSize) = 0;
		virtual void remove(cPacket* pkt) = 0;
		virtual void initialize(cModule* module) = 0;

		virtual void setLimit(uint32_t l) { limit = l; }
};

#endif /* SIMPLEQUEUE_H_ */
