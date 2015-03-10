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

#include "ONUReservation.h"
#include <string>
#include <sstream>

ONUReservation::ONUReservation()
{
	start = 0;
	slotSizeInBytes = 0;
	isReport = false;
	entry = NULL;
}

ONUReservation::ONUReservation(int64 p_start, uint32_t p_slotSizeInBytes, bool p_isReport, ONUTableEntry* p_entry)
{
	start = p_start;
	slotSizeInBytes = p_slotSizeInBytes;
	isReport = p_isReport;
	entry = p_entry;
}

ONUReservation::ONUReservation(const ONUReservation& r)
{
	start = r.getStart();
	slotSizeInBytes = r.getSlotSizeInBytes();
	isReport = r.getIsReport();
	entry = r.getEntry();
}

ONUReservation::~ONUReservation()
{

}

simtime_t ONUReservation::getStartSimTime()
{
	simtime_t t;
	t.setRaw(this->start);

	return t;
}

simtime_t ONUReservation::endReservation(uint32_t txrate)
{
	simtime_t t;
	t = t.setRaw(this->start) + (double)(this->slotSizeInBytes * 8) / (double)txrate;

	return t;
}

bool operator<(const ONUReservation& r1, const ONUReservation& r2)
{
	return r1.getStart() < r2.getStart();
}

ONUReservation& ONUReservation::operator=(const ONUReservation &r)
{
	start = r.getStart();
	slotSizeInBytes = r.getSlotSizeInBytes();
	isReport = r.getIsReport();
	entry = r.getEntry();

	return *this;
}

std::string ONUReservation::toString(uint32_t txrate)
{
	std::stringstream ss;
	ss << getEntry()->getId() << " " << getStartSimTime() << " -> " << endReservation(txrate) << " channel -> " << (int)getEntry()->getChannel() << " report ? " << getIsReport() << " size = " << getSlotSizeInBytes();

	return ss.str();
}


