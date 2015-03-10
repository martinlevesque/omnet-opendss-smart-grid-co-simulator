/*
 * ONUTableEntry.cpp
 *
 *  Created on: Apr 11, 2010
 *      Author: urban
 */

#include "ONUTableEntry.h"

ONUTableEntry::ONUTableEntry() {
	id.setAddress("00:00:00:00:00:00");
	RTT=0;
	wdmEnabled = false;
	channel = 0;
	dedicatedChannel = 0;
}

ONUTableEntry::ONUTableEntry(const ONUTableEntry & en){
	id=en.id;
	RTT=en.RTT;
	wdmEnabled = en.wdmEnabled;
	channel = en.channel;
	dedicatedChannel = en.dedicatedChannel;

	for (uint32_t i=0; i<en.LLIDs.size(); i++)
		LLIDs.push_back(en.LLIDs[i]);
}

ONUTableEntry::~ONUTableEntry() {
	LLIDs.clear();
}

void ONUTableEntry::setId(MACAddress &mac){id=mac;}
void ONUTableEntry::setRTT(uint32_t rtt){RTT=rtt;}
/**
 * Check the LLID is unique and add. IF it is not
 * return -1;
 */
int ONUTableEntry::addLLID(uint16_t llid){
	for (uint32_t i=0; i<LLIDs.size(); i++){
		if (LLIDs[i] == llid) return -1;
	}

	LLIDs.push_back(llid);
	return 0;
}
void ONUTableEntry::removeLLID(int idx)
{
	LLIDs.erase(LLIDs.begin()+idx);
}

MACAddress ONUTableEntry::getId(){return id;}

uint16_t ONUTableEntry::getRTT(){return RTT;}

uint16_t ONUTableEntry::getLLID(int idx){return LLIDs[idx];}

int ONUTableEntry::getLLIDsNum(){return LLIDs.size();}

vector<uint16_t> & ONUTableEntry::getLLIDs() const{
	return (vector<uint16_t> &)LLIDs;
}

bool ONUTableEntry::isValid(){
	return !id.equals("00:00:00:00:00:00");
}

ONUTableEntry& ONUTableEntry::operator=(const ONUTableEntry &en){
	id=en.id;
	RTT=en.RTT;
	wdmEnabled = en.wdmEnabled;
	channel = en.channel;
	dedicatedChannel = en.dedicatedChannel;

	LLIDs.clear();

	for (uint32_t i=0; i<en.LLIDs.size(); i++)
		LLIDs.push_back(en.LLIDs[i]);

	return *this;
}

cObject * ONUTableEntry::dup(){
	return new ONUTableEntry(*this);
}
std::string ONUTableEntry::info(){
	return "A single ONU entry";
}

std::ostream & operator<<(std::ostream &out, const ONUTableEntry &en){
	out<<"ID: "<<en.id<<"  RTT: "<<en.RTT<<"  # of LLIDs: "<<en.LLIDs.size()
		<< ", wdm enabled: " << en.getWdmEnabled()
		<< " ch = " << (int)en.getChannel();
	return out;
}



