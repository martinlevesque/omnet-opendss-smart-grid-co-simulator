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

#include "SimpleQueue.h"

#ifndef ONU_QPL_RR_H_
#define ONU_QPL_RR_H_

#include "ONUQPerLLiDBase.h"

class ONU_QPL_RR : public ONUQPerLLiDBase
{
public:

	virtual void initialize(int stage);
	virtual int numInitStages() 	 const { return 4; }

	/**
	 * This method returns the next frame and removes it
	 * from the queue. Thus this method (and the getNextFrameSize()) control
	 * the queue's priorities and QoS. The default behavior is to do
	 * Round Robin on the queues. NOTE: RR is per frame...
	 *
	 * This could be overridden in a subclass and change the default
	 * behavior.
	 */
	virtual int requestPacket(int maxPacketSize);
	virtual uint32_t queueSizeInBytes();
};

#endif /* ONU_QPL_RR_H_ */
