//
// Copyright (C) 2008 Alfonso Ariza
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#ifndef __ManetManager_H__
#define __ManetManager_H__

#include "INETDefs.h"
#include "ICMPAccess.h"
// ICMP type 2, code 4: fragmentation needed, but don't-fragment bit set


/**
 *
 */

class INET_API ManetManager : public cSimpleModule
{
  private:
    ICMPAccess icmpAccess;
    enum RouteTypeProtocol
    {
        AODV,
        DSR,
        DYMO,
        OLSR,
        DSDV,
        BATMAN
    };
    cModule *routingModule;
    RouteTypeProtocol routing_protocol;
    bool dynamicLoad;
  protected:
    // config
    bool manetActive;
    const char *routingProtocol;
  public:
    ManetManager() {dynamicLoad=false;};
  protected:
    /**
     * Initialization
     */
    int numInitStages() const  {return 5;}
    void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    virtual void finish() {};
};

class INET_API ManetManagerStatic : public ManetManager {};
#endif

