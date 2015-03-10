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

#include "ONUTable.h"
#include <algorithm>
#include "PONUtil.h"
#include <vector>
#include <map>

using namespace std;

Define_Module(ONUTable);

//ONUTable::~ONUTable(){
//	tbl.clear();
//}
//ONUTable::ONUTable(){}

vector<ONUInfoWR> ONUTable::WRNodeInfos;

std::map<int, map<int, double > > ONUTable::channelUtil;
map<int, double> ONUTable::channelSchedDelay;

ONUInfoWR ONUTable::getONUInfoWROf(MACAddress addr)
{
	for (int i = 0; i < (int)WRNodeInfos.size(); ++i)
	{
		if (WRNodeInfos[i].addr == addr)
			return WRNodeInfos[i];
	}

	ONUInfoWR o;
	o.id = -1;

	return o;
}

void ONUTable::initialize()
{

}

void ONUTable::handleMessage(cMessage *msg)
{
	// TODO - Generated method body
	delete msg;
}

simtime_t ONUTable::timeAfterBytes(int64 curTime, uint32_t sizeBytes, int channel)
{
	simtime_t t;
	t.setRaw(curTime);

	return t + (double)(sizeBytes * 8) / PONUtil::getCapacityOf(this, channel);
}

int ONUTable::calculatesAllocationTo(MACAddress addr, int normalizedMaxAllocation)
{
	int nbONUs = (int)table[0].size();
	int totalMaxAllocation = normalizedMaxAllocation * nbONUs;

	EV << "TOTAL MAX = " << totalMaxAllocation << endl;

	int myAllocation = previousONUQueueLengths[addr];

	if (myAllocation <= normalizedMaxAllocation)
	{
		//return ((int)ceil((float)myAllocation / 1500.0)) * 1500;
		return myAllocation;
	}

	int sumAllocations = 0;

	for (int i = 0; i < (int)table[0].size(); ++i)
	{
		int curAlloc = previousONUQueueLengths[table[0][i].getId()];

		EV << "CUR ALLOC = " << curAlloc << endl;

		if (table[0][i].getId() == addr)
			continue;

		EV << "  + " << endl;

		sumAllocations += curAlloc;
	}

	int availableAllocation = max(0, totalMaxAllocation - sumAllocations);

	if (availableAllocation < normalizedMaxAllocation)
	{
		availableAllocation = normalizedMaxAllocation;
	}

	int allocation = min(myAllocation, availableAllocation);

	//allocation = ((int)ceil((float)allocation / 1500.0)) * 1500;

	EV << "allocation = " << allocation << endl;

	return allocation;
}

// Finds the nearest channel slot. If it's report, must be on TDM (0)
ONUReservation ONUTable::reserve(int64 minTime, uint32_t sizeInBytes, bool isReport, MACAddress id, const std::vector<uint8_t>& channelsToAvoid, bool persistent)
{
	int64 nearestTime = -1;
	uint8_t channelNearest = -1;

	bool wdmEnabled = getEntry(id, 0)->getWdmEnabled();

	// On which channel should we reserve ?
	for (map<int, vector<ONUTableEntry> >::iterator it = table.begin(); it != table.end(); ++it)
	{
		uint8_t ch = (uint8_t)it->first;

		if (std::find (channelsToAvoid.begin(), channelsToAvoid.end(), ch) != channelsToAvoid.end())
		{
			continue;
		}

		// REPORT, control -> channel 0 only
		if (((isReport || ! wdmEnabled) && ch > 0))
		{
			break;
		}

		int64 curTime = -1;

		simtime_t endMin = timeAfterBytes(minTime, sizeInBytes, ch);

		// 1- There is NO reservation, so we can schedule the minTime
		if (reservations[ch].size() == 0)
		{
			curTime = minTime;
		}
		else
		// 2- does it fits before the first one ?
		if (reservations[ch].size() > 0 && reservations[ch][0].getStartSimTime() >= endMin)
		{
			curTime = minTime;
		}
		else
		{
			// 3- then find the nearest reservation which it fits AFTER
			ONUReservation* fitsAfterReservation = NULL;

			for (int i= 0; i < (int)reservations[ch].size(); ++i)
			{
				if (i == (int)reservations[ch].size() - 1)
				{
					curTime = reservations[ch][i].endReservation(PONUtil::getCapacityOf(this, ch)).raw();
					fitsAfterReservation = &reservations[ch][i];

					if (curTime < minTime)
					{
						curTime = minTime;
					}
				}
				else
				{
					simtime_t tMin;
					tMin.setRaw(minTime);
					EV << "ONUTable::reserve - cas 3 tMin = " << tMin << endl;

					simtime_t tEnd =  timeAfterBytes(minTime, (uint32_t)sizeInBytes, ch);

					// FITS between two reservations
					if (tMin >= reservations[ch][i].endReservation(PONUtil::getCapacityOf(this, ch)) && tEnd <= reservations[ch][i + 1].getStartSimTime())
					{
						// FOUND
						curTime = minTime;
						fitsAfterReservation = &reservations[ch][i];

						break;
					}
				}
			}

			// There is a reservation:
			if (fitsAfterReservation == NULL)
			{
				error("ONUTable::reserve: else.");
			}


		}

		if (curTime < minTime)
		{
			error("ONUTable::reserve: problem... BUG 2. ch = %d", ch);
		}

		if (curTime == -1)
		{

			error("ONUTable::reserve: no slot found... BUG. ch = %d", ch);
		}

		EV << "ONUTable::reserve, ch = " << (int)ch << " cut time = " << curTime << endl;

		if (nearestTime == -1 || curTime < nearestTime)
		{
			nearestTime = curTime;
			channelNearest = ch;
		}
	}

	if (channelNearest < 0)
	{
		error("ONUTable::reserve: channelNearest < 0.");
	}



	// Finally we create the new reservation.
	ONUReservation reservation = ONUReservation(nearestTime, sizeInBytes, isReport, getEntry(id, channelNearest));

	EV << "ONUTable::reserve - finished... " << endl;

	simtime_t simTimeNearest;
	simTimeNearest.setRaw(nearestTime);

	EV << "ONUTable::reserve: mac = " << id << " ch = " << (int)channelNearest << " at = " << simTimeNearest << " size in bytes = " << (int)sizeInBytes
			<< " report = " << isReport << " entry ptr = " << reservation.getEntry() << endl;

	reservations[(int)channelNearest].push_back(reservation);

	this->cleanReservations();

	return reservation;
}

// Remove passed reservations, sort current ones
void ONUTable::cleanReservations()
{
	// Remove old reservations

	// loop channels:
	for (map<int, vector<ONUReservation> >::iterator it = reservations.begin(); it != reservations.end(); ++it)
	{
		EV << "channel = " << it->first << endl;

		// loop reservations in channel:
		for (vector<ONUReservation>::iterator itReservations = it->second.begin(); itReservations != it->second.end(); )
		{
			EV << "cleaning reservations cur time = " << simTime().dbl() << " end reservation = " << itReservations->endReservation(PONUtil::getCapacityOf(this, it->first)) << endl;

			if (simTime() > itReservations->endReservation(PONUtil::getCapacityOf(this, it->first)))
			{
				EV << "	DELETEEEEEEEEE" << endl;
				itReservations = it->second.erase(itReservations);
			}
			else
			{
				++itReservations;
			}
		}
	}

	// Sort current reservations
	for (map<int, vector<ONUReservation> >::iterator it = reservations.begin(); it != reservations.end(); ++it)
	{
		std::sort(it->second.begin(), it->second.end());
	}
}

vector<int> ONUTable::wavelengths()
{
	vector<int> ws;

	if (PONUtil::getPonVersion(this) == 2)
	{
		for (int i = 0; i < (int)WRNodeInfos.size(); ++i)
		{
			if (find(ws.begin(), ws.end(), WRNodeInfos[i].channel) == ws.end())
			{
				ws.push_back(WRNodeInfos[i].channel);
				EV << " ONUTable::wavelengths push " << WRNodeInfos[i].channel << endl;
			}
		}
	}
	else
	{
		for (map<int, vector<ONUTableEntry> >::iterator it = table.begin(); it != table.end(); ++it)
		{
			ws.push_back(it->first);
		}
	}

	return ws;
}

void ONUTable::printReservations()
{
	EV << "ONUTable::printReservations prints" << endl;

	for (map<int, vector<ONUReservation> >::iterator it = reservations.begin(); it != reservations.end(); ++it)
	{
		// loop reservations in channel:
		for (vector<ONUReservation>::iterator itReservations = it->second.begin(); itReservations != it->second.end(); ++itReservations)
		{
			EV << itReservations->toString(PONUtil::getCapacityOf(this, it->first)) << endl;
		}
	}
}

void ONUTable::addONU(ONUTableEntry &entry)
{

	EV << "ONUTable::addONU of " << entry.getId() << " 1 " << endl;

	if (table[entry.getChannel()].size() == 0 )
	{
		EV << "ONUTable::addONU of " << entry.getId() << " 2 " << endl;

		table[entry.getChannel()].push_back(entry);
		return;
	}

	for (uint32_t i= 0; i < table[entry.getChannel()].size(); i++)
	{
		// Do not add the same onu 2 times in the same channel
		if (table[entry.getChannel()][i].getId() == entry.getId())
		{
			EV << "ONUTable::addONU of " << entry.getId() << " 3 " << endl;
			table[entry.getChannel()][i] = entry;
			return;
		}
	}

	// If HERE, entry is new
	EV << "ONUTable::addONU of " << entry.getId() << " 4 " << endl;
	table[entry.getChannel()].push_back(entry);
}

ONUTableEntry* ONUTable::getEntry(MACAddress id)
{
	for (map<int, vector<ONUTableEntry> >::iterator it = table.begin(); it != table.end(); ++it)
	{
		for (uint32_t i=0; i < it->second.size(); i++)
		{
			if (it->second.at(i).getId() == id)
			{
				return &(it->second.at(i));
			}
		}
	}

	return NULL;
}

ONUTableEntry* ONUTable::getEntry(MACAddress id, int channel)
{
	for (uint32_t i=0; i<table[channel].size(); i++)
	{
		if (table[channel][i].getId() == id && table[channel][i].getChannel() == channel)
		{
			return &(table[channel][i]);

		}
	}

	return NULL;
}

ONUTableEntry* ONUTable::getEntry(std::string id)
{
	for (map<int, vector<ONUTableEntry> >::iterator it = table.begin(); it != table.end(); ++it)
	{
		for (uint32_t i=0; i < it->second.size(); i++)
		{
			if (it->second[i].getId().str() == id)
				return &(it->second[i]);
		}
	}

	return NULL;
}

// Tools
MACAddress ONUTable::getMACFromString(std::string id){
	return getEntry(id)->getId();
}

uint16_t ONUTable::nbONUs()
{
	if (table.size() == 0)
		return 0;

	return table[0].size();
}

void ONUTable::print()
{
	EV << "ONUTable: " << endl;

	for (map<int, vector<ONUTableEntry> >::iterator it = table.begin(); it != table.end(); ++it)
	{
		for (uint32_t i=0; i < it->second.size(); i++)
		{
			EV << it->first << " " << it->second[i] << endl;
		}
	}
}
