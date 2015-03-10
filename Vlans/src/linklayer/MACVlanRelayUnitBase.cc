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

#include "MACVlanRelayUnitBase.h"
#include "MACAddress.h"
#include "EtherFrame_m.h"
#include "Ethernet.h"


#define MAX_LINE 100

using std::string;


/* unused for now
static std::ostream& operator<< (std::ostream& os, cMessage *msg)
{
    os << "(" << msg->getClassName() << ")" << msg->getFullName();
    return os;
}
*/

static std::ostream& operator<< (std::ostream& os, const AddressEntry& e)
{
    os << "port=" << e.portName  << "   insTime=" << e.insertionTime;
    return os;
}

/**
 * Function reads from a file stream pointed to by 'fp' and stores characters
 * until the '\n' or EOF character is found, the resultant string is returned.
 * Note that neither '\n' nor EOF character is stored to the resultant string,
 * also note that if on a line containing useful data that EOF occurs, then
 * that line will not be read in, hence must terminate file with unused line.
 */
static char *fgetline (FILE *fp)
{
    // alloc buffer and read a line
    char *line = new char[MAX_LINE];
    if (fgets(line,MAX_LINE,fp)==NULL)
        return NULL;

    // chop CR/LF
    line[MAX_LINE-1] = '\0';
    int len = strlen(line);
    while (len>0 && (line[len-1]=='\n' || line[len-1]=='\r'))
        line[--len]='\0';

    return line;
}

void MACVlanRelayUnitBase::initialize()
{
    // number of ports
    numPorts = 2;

    // other parameters
    addressTableSize = par("addressTableSize");
    addressTableSize = addressTableSize >= 0 ? addressTableSize : 0;

    agingTime = par("agingTime");
    agingTime = agingTime > 0 ? agingTime : 10;

    // Option to pre-read in Address Table. To turn ot off, set addressTableFile to empty string
    const char *addressTableFile = par("addressTableFile");
    if (addressTableFile && *addressTableFile)
        readAddressTable(addressTableFile);

    seqNum = 0;

    WATCH_MAP(addresstable);
}



void MACVlanRelayUnitBase::printAddressTable()
{
    AddressTable::iterator iter;
    EV << "Address Table (" << addresstable.size() << " entries):\n";
    for (iter = addresstable.begin(); iter!=addresstable.end(); ++iter)
    {
        EV << "  " << iter->first << " --> port " << iter->second.portName
			<< " --> llid " << iter->first.llid <<
         (iter->second.insertionTime+agingTime <= simTime() ? " (aged)" : "") << endl;
    }
}

void MACVlanRelayUnitBase::removeAgedEntriesFromTable()
{
    for (AddressTable::iterator iter = addresstable.begin(); iter != addresstable.end();)
    {
        AddressTable::iterator cur = iter++; // iter will get invalidated after erase()
        AddressEntry& entry = cur->second;
        if (entry.insertionTime + agingTime <= simTime())
        {
            EV << "Removing aged entry from Address Table: " <<
                  cur->first << " --> port" << cur->second.portName << "\n";
            addresstable.erase(cur);
        }
    }
}

void MACVlanRelayUnitBase::removeOldestTableEntry()
{
    AddressTable::iterator oldest = addresstable.end();
    simtime_t oldestInsertTime = simTime()+1;
    for (AddressTable::iterator iter = addresstable.begin(); iter != addresstable.end(); iter++)
    {
        if (iter->second.insertionTime < oldestInsertTime)
        {
            oldest = iter;
            oldestInsertTime = iter->second.insertionTime;
        }
    }
    if (oldest != addresstable.end())
    {
        EV << "Table full, removing oldest entry: " <<
              oldest->first << " --> port" << oldest->second.portName <<"\n";
        addresstable.erase(oldest);
    }
}

void MACVlanRelayUnitBase::updateTableFromFrame(EtherFrame * frame)
{
	mac_llid ml;
	ml.llid=-1;

	if (dynamic_cast<EthernetDot1QFrame *>(frame)!=NULL)
		ml.llid=dynamic_cast<EthernetDot1QFrame *>(frame)->getVlanID();

	ml.mac = frame->getSrc();
	updateTableWithAddress(ml, "ethOut");
}

void MACVlanRelayUnitBase::updateTableWithAddress(mac_llid MacLLid, std::string portName)
{
    AddressTable::iterator iter;

    iter = addresstable.find(MacLLid);
    if (iter == addresstable.end())
    {
        // Observe finite table size
        if (addressTableSize!=0 && addresstable.size() == (unsigned int)addressTableSize)
        {
            // lazy removal of aged entries: only if table gets full (this step is not strictly needed)
            EV << "Making room in Address Table by throwing out aged entries.\n";
            removeAgedEntriesFromTable();

            if (addresstable.size() == (unsigned int)addressTableSize)
                removeOldestTableEntry();
        }

        // Add entry to table
        EV << "Adding entry to Address Table: "<< MacLLid.mac << " --> port " << portName
				<< " --> llid " << MacLLid.llid << "\n";
        AddressEntry entry;
        entry.portName = portName;
        entry.insertionTime = simTime();
        addresstable[MacLLid] = entry;

    }
    else
    {
        // Update existing entry
        EV << "Updating entry in Address Table: "<< MacLLid.mac << " --> port " << portName
				<< " --> llid " << MacLLid.llid << "\n";
        AddressEntry& entry = iter->second;
        entry.insertionTime = simTime();
        entry.portName = portName;
    }
}

string
MACVlanRelayUnitBase::getPortForAddress(mac_llid ml)
{
    AddressTable::iterator iter = addresstable.find(ml);
    if (iter == addresstable.end())
    {
        // not found
        return "";
    }
    if (iter->second.insertionTime + agingTime <= simTime())
    {
        // don't use (and throw out) aged entries
        EV << "Ignoring and deleting aged entry: "<< iter->first.mac << " --> port " << iter->second.portName
			<< " llid " << iter->first.llid << "\n";
        addresstable.erase(iter);
        return "";
    }
    return iter->second.portName;
}

std::vector<port_llid>
MACVlanRelayUnitBase::getLLIDsForAddress(MACAddress mac){
	std::vector<port_llid> res;

    for (AddressTable::iterator iter = addresstable.begin(); iter != addresstable.end(); iter++)
    {
    	// Check for aged
    	if (iter->second.insertionTime + agingTime <= simTime())
		{
			// don't use (and throw out) aged entries
			EV << "Ignoring and deleting aged entry: "<< iter->first.mac << " --> port " << iter->second.portName
				<< " llid " << iter->first.llid << "\n";
			addresstable.erase(iter);
			continue;
		}

    	if (iter->first.mac == mac){
    		port_llid pl;
    		pl.llid = iter->first.llid;
    		pl.port = iter->second.portName;
    		res.push_back(pl);
    	}

    }

    return res;
}


void MACVlanRelayUnitBase::readAddressTable(const char* fileName)
{
    FILE *fp = fopen(fileName, "r");
    if (fp == NULL)
        error("cannot open address table file `%s'", fileName);

    //  Syntax of the file goes as:
    //  Address in hexadecimal representation, portName
    //  ffffffff    Xout	1000
    //  ffffeed1    Yout	901
    //  aabcdeff    Zout	3
    //
    //  etc...
    //
    //  Each iteration of the loop reads in an entire line i.e. up to '\n' or EOF characters
    //  and uses strtok to extract tokens from the resulting string
    char *line;
    int lineno = 0;
    while ((line = fgetline(fp)) != NULL)
    {
        lineno++;

        // lines beginning with '#' are treated as comments
        if (line[0]=='#')
            continue;

        // scan in hexaddress
        char *hexaddress = strtok(line, " \t");
        // scan in port number
        string portName;
        portName = strtok(NULL, " \t");
        // scan in logical number
        char *llid = strtok(NULL, " \t");

        // empty line?
        if (!hexaddress)
            continue;

        // broken line?
        if (portName!="" || !llid)
            error("line %d invalid in address table file `%s'", lineno, fileName);


        // Create an entry with address and portName and insert into table
        AddressEntry entry;
        entry.insertionTime = 0;
        entry.portName = portName;
        mac_llid ml;
        ml.mac=MACAddress(hexaddress);
        ml.llid = atoi(llid);
        addresstable[ml] = entry;

        // Garbage collection before next iteration
        delete [] line;
    }
    fclose(fp);
}




