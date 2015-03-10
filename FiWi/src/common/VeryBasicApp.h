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

#ifndef __FIWI_VERY_BASIC_APP_H__
#define __FIWI_VERY_BASIC_APP_H__

#include <omnetpp.h>
#include <map>
#include <string>
#include <vector>
#include <list>
#include "MACAddress.h"


/**
 *
 */
class VeryBasicApp : public cSimpleModule
{
public:

protected:
	int packetSizeInBytes;
	int lambda;
	std::string hostId;
	std::string packetType;
	int cnt;

	cMessage* msgNextPacket;

	virtual void initialize(int stage);
	virtual int numInitStages() const {return 6;}

    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    double randomInter();
};



#endif
