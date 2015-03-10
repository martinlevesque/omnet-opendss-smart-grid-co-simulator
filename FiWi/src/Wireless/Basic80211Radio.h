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

#ifndef __BASIC_80211_RADIO_H__
#define __BASIC_80211_RADIO_H__

#include <omnetpp.h>
#include "MPCPTools.h"
#include <map>
#include <vector>
#include "RangeArea.h"

/**
 *
 *
 */
class Basic80211Radio : public cSimpleModule
{
  protected:

    int ports;          // number of ports
    int portsRange;
    double propagationDelay;
    double BER;
    bool selectiveBERAck;
    double phyHeaderDuration;

    std::vector<RangeArea*> rangeAreas;

  protected:
    virtual void initialize(int stage);
    virtual int numInitStages() const {return 4;}
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    std::vector<RangeArea*> getAreaRangesOf(cModule* thisModule);
};



#endif
