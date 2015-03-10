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

#include "ONU_QPL_RR.h"
#include <omnetpp.h>

Define_Module(ONU_QPL_RR);

void ONU_QPL_RR::initialize(int stage)
{
	ONUQPerLLiDBase::initialize(stage);
}

int ONU_QPL_RR::requestPacket(int maximumPacketSize)
{
	Enter_Method("requestPacket()");

	cPacket* p = NULL;

	p = queue->front(maximumPacketSize);

	if (p && p->getByteLength() <= maximumPacketSize)
	{
		queue->remove(p);
	}
	else
	{
		p = NULL;
	}

	if (p == NULL)
	{
		return 0;
	}

	send(p, "lowerLayerOut");

	return ONUMacCtl::calcPacketSizeOnMedium(p->getByteLength());
}

uint32_t ONU_QPL_RR::queueSizeInBytes()
{
	return queue->lengthInBytes();
}
