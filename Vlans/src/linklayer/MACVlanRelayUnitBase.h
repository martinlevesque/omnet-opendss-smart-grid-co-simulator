/*
 * Copyright (C) 2003 Andras Varga; CTIE, Monash University, Australia
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
*/


#ifndef _MACVLANRELAYUNITBASE_H
#define _MACVLANRELAYUNITBASE_H

#include <omnetpp.h>
#include <map>
#include <string>
#include <vector>
#include "MACAddress.h"
#include "EtherFrameVlans_m.h"


class EtherFrame;

/**
 * The AddressEntry holds the rest of the information
 * needed in the MAC Address table. The basic difference from
 * the INET frame work is that the port now is not a number but
 * a string (in order to support both ethX and ponX ports).
 */
struct AddressEntry
{
	std::string portName;    // Input port
    simtime_t insertionTime; // Arrival time of Lookup Address Table entry
};

/**
 * Bonds a MAC address with a logical link ID (or VLAN).
 */
typedef
struct _mac_llid{
	MACAddress mac;
	int llid;
	bool operator==(struct _mac_llid ml ) const {
		return (mac == ml.mac && llid == ml.llid);
	}

	friend std::ostream & operator<<(std::ostream & out, const struct _mac_llid & ml){
		out<<"MAC: "<<ml.mac<<"  LLID: "<<ml.llid;
		return out;
	}
} mac_llid;

/**
 * Bonds a port with an LLID. (I don't remember where I used it and if it
 * makes any sense).
 */
typedef
struct _port_llid{
	std::string port;
	int llid;
	bool operator==(struct _port_llid pl ) const {
		return (port == pl.port && llid == pl.llid);
	}
} port_llid;

/**
 * Comparison operator for using the mac_llid struct in
 * an std::map container. Again I don't know if map is the
 * proper solution (it is probably faster on lookups...)
 */
struct macllidCmp {
  // FUCK STL compare: result true if s1<s2... grrr
  // s1<s2 - true
  // s2<s1 - true ???
  bool operator()( const mac_llid s1, const mac_llid s2 ) const {
	  int res=s1.mac.compareTo(s2.mac);
	  if (res==0) return s1.llid < s2.llid;
	  return res<0;
  }
};

/**
 *
 * This class is taken from INET FrameWork switches and changed a bit to
 * support logical links. The basic struct used here is the mac_llid which
 * bonds a MAC address with a logical entity. The same struct could be used
 * to support VLANs in the switches.
 *
 * From the INET documentation:
 *
 * "Implements base switching functionality of Ethernet switches. Note that
 * neither activity() nor handleMessage() is redefined here -- active
 * behavior (incl. queueing and performance aspects) must be addressed
 * in subclasses."
 */
class MACVlanRelayUnitBase : public cSimpleModule
{
  public:


  protected:

	/// The mac-address table as a map of mac_llid and extra information.
    typedef std::map<mac_llid, AddressEntry, macllidCmp> AddressTable;

    // Parameters controlling how the switch operates
    int numPorts;               // Number of ports of the switch
    int addressTableSize;       // Maximum size of the Address Table
    simtime_t agingTime;        // Determines when Ethernet entries are to be removed

    AddressTable addresstable;  // Address Lookup Table

    int seqNum;                 // counter for PAUSE frames

  protected:
    /**
     * Read parameters parameters.
     */
    virtual void initialize();



    /**
     * Pre-reads in entries for Address Table during initialization.
     */
    virtual void readAddressTable(const char* fileName);

    /**
     * Enters address into table.
     */
    virtual void updateTableWithAddress(mac_llid MacLLid, std::string portName);

    /**
	 * Enters address and vlan (if any) into table.
	 */
	virtual void updateTableFromFrame(EtherFrame * frame);

    /**
     * Returns output port for address, or "" if unknown.
     */
    virtual std::string getPortForAddress(mac_llid MacLLid);

    /**
     * Returns LLIDs for address.
     */
    virtual std::vector<port_llid> getLLIDsForAddress(MACAddress mac);

    /**
     * Prints contents of address table on ev.
     */
    virtual void printAddressTable();

    /**
     * Utility function: throws out all aged entries from table.
     */
    virtual void removeAgedEntriesFromTable();

    /**
     * Utility function: throws out oldest (not necessarily aged) entry from table.
     */
    virtual void removeOldestTableEntry();

    /*
     * Find a Logical Port in the Table ["",-1] is returned on failure.
     */


};

#endif


