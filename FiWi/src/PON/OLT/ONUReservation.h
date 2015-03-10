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

#include <omnetpp.h>
#include "ONUTableEntry.h"
#include <string>

#ifndef ONURESERVATION_H_
#define ONURESERVATION_H_

class ONUReservation
{

private:
	int64 start;
	uint32_t slotSizeInBytes;
	bool isReport;
	ONUTableEntry* entry;

public:
	ONUReservation();
	ONUReservation(int64 p_start, uint32_t p_slotSizeInBytes, bool p_isReport, ONUTableEntry* p_entry);
	ONUReservation(const ONUReservation& r);
	virtual ~ONUReservation();

	int64 getStart() const { return start; }
	simtime_t getStartSimTime();
	uint32_t getSlotSizeInBytes() const { return slotSizeInBytes; }
	bool getIsReport() const { return isReport; }
	ONUTableEntry* getEntry() const { return entry; }

	uint8_t getChannel() const { return entry->getChannel(); }

	simtime_t endReservation(uint32_t txrate);
	std::string toString(uint32_t txrate);

	ONUReservation& operator=(const ONUReservation& r);

};

bool operator<(const ONUReservation& r1, const ONUReservation& r2);

#endif /* ONURESERVATION_H_ */
