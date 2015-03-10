#ifndef __INET_IOLTMGMT_H
#define __INET_IOLTMGMT_H

#include <omnetpp.h>
#include "INETDefs.h"
#include <vector>
#include "MACAddress.h"
#include "ONUReservation.h"

/**
 * Abstract interface for passive queues.
 */
class INET_API IOLTMgmt
{
  public:
    virtual ~IOLTMgmt() {}


    virtual void requestPacket() = 0;

    virtual std::vector<ONUReservation> DoUpstreamDBA(MACAddress onuAddr, int newSlotLength) = 0;

};

#endif
