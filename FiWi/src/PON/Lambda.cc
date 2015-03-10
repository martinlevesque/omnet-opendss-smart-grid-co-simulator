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

#include "EPON_CtrlInfo.h"
#include "Lambda.h"

Define_Module(Lambda);

void Lambda::initialize()
{
    capacity = par("capacity");

    upstreamChannelReservation.hasInfos = false;
    downstreamChannelReservation.hasInfos = false;
}

void Lambda::handleMessage(cMessage *msg)
{
	error("Lambda::handleMessage - wtf ?");
}

bool Lambda::downstreamBusy()
{
	if (downstreamChannelReservation.hasInfos && simTime() < downstreamChannelReservation.end)
		return true;

	return false;
}

bool Lambda::upstreamBusy()
{
	if (upstreamChannelReservation.hasInfos && simTime() < upstreamChannelReservation.end)
		return true;

	return false;
}

void Lambda::sendUpstream(double bits, int channel)
{
	upstreamChannelReservation.hasInfos = true;
	upstreamChannelReservation.begin = simTime();
	upstreamChannelReservation.end = simTime() + (bits / capacity);
}

void Lambda::sendDownstream(double bits, int channel)
{
	downstreamChannelReservation.hasInfos = true;
	downstreamChannelReservation.begin = simTime();
	downstreamChannelReservation.end = simTime() + (bits / capacity);
}

void Lambda::finish ()
{
}
