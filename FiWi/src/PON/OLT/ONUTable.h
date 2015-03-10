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

#ifndef __ONUTABLE_H__
#define __ONUTABLE_H__

#include <omnetpp.h>
#include <vector>
#include "ONUTableEntry.h"
#include "MACAddress.h"
#include "ONUReservation.h"

using std::vector;
using std::map;

using namespace std;

struct ONUInfoWR
{
	int channel;
	int id;
	MACAddress addr;
};

/**
 * A simple class containing a vector of ONUTableEntry
 * objects. ONUTable also provides some basic functions
 * for adding, removing and finding specific entries based
 * on MAC address or index. NOTE: DO NOT FORGET: It is NOT
 * a vector so you must not call size() BUT instead call
 * getTableSize().
 */
class ONUTable : public cSimpleModule
{
  protected:
	// vector<ONUTableEntry> tbl;
	map<int, vector<ONUTableEntry> > table;
	map<int, vector<ONUReservation> > reservations;


	MACAddress oltAddr;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

  public:


    static std::map<int, std::map<int, double > > channelUtil; // Channel -> second -> bits
    static std::map<int, double> channelSchedDelay; // Channel -> second -> bits

    static std::vector<ONUInfoWR> WRNodeInfos;
    static ONUInfoWR getONUInfoWROf(MACAddress addr);

    map<MACAddress, int> previousONUQueueLengths;

    int calculatesAllocationTo(MACAddress addr, int normalizedMaxAllocation);

    void setOLTAddr(MACAddress addr) { oltAddr = addr; }

    MACAddress getOLTAddr() { return oltAddr; }

    vector<int> wavelengths();

//	ONUTable();
//	virtual ~ONUTable();
	virtual void addONU(ONUTableEntry &entry);

	// Pointer to be able to change stuff...
	virtual ONUTableEntry* getEntry(MACAddress id);
	virtual ONUTableEntry* getEntry(MACAddress id, int channel);
	virtual ONUTableEntry* getEntry(std::string id);
	vector<ONUTableEntry>& getONUsChannel(int ch) { return table[ch]; }
	uint16_t nbONUs();

	void print();
	void cleanReservations();
	ONUReservation reserve(int64 minTime, uint32_t sizeInBytes, bool isReport, MACAddress id, const std::vector<uint8_t>& channelsToAvoid, bool persistent);

	void printReservations();

	// Tools
	virtual MACAddress getMACFromString(std::string id);

	simtime_t timeAfterBytes(int64 curTime, uint32_t sizeBytes, int channel);

  private:

};

#endif
