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

#ifndef __PON_SPLITTER_H__
#define __PON_SPLITTER_H__

#include <omnetpp.h>
#include "MPCPTools.h"
#include <map>



/**
 * PON_Splitter is a fairly simple module to forward
 * frames from ONU side to OLT side and the opposite.
 * This module thought allow you to see the collision
 * counters, which are really useful if you change the
 * timing and synchronization functions.
 */
class PON_Splitter : public cSimpleModule
{
  protected:

    int ports;          // number of ports
    long numMessages;   // number of messages handled
    cMessage * previousMsg;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};



#endif
