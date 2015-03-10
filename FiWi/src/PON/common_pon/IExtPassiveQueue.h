/*
 * IExtPassiveQueue.h
 *
 *  Created on: Aug 11, 2011
 *      Author: martin
 */

#ifndef __INET_IEXTPASSIVEQUEUE_H
#define __INET_IEXTPASSIVEQUEUE_H

#include <omnetpp.h>
#include "INETDefs.h"

/**
 * Abstract interface for passive queues.
 */
class INET_API IExtPassiveQueue
{
  public:
    virtual ~IExtPassiveQueue() {}

    /**
     * The queue should send a packet whenever this method is invoked.
     * If the queue is currently empty, it should send a packet when
     * when one becomes available.
     */
    virtual int requestPacket(int maxPacketLength) = 0;
    virtual uint32_t queueSizeInBytes() = 0;
};

#endif
