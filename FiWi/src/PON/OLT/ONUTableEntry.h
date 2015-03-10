/*
 * ONUTableEntry.h
 *
 *  Created on: Apr 11, 2010
 *      Author: urban
 */

#ifndef ONUTABLEENTRY_H_
#define ONUTABLEENTRY_H_

#include <vector>
#include "MACAddress.h"

using std::vector;

/**
 * This class holds a single ONUTable entry...
 */
class ONUTableEntry : public cObject{
public:
	ONUTableEntry();
	ONUTableEntry(const ONUTableEntry & en);
	virtual ~ONUTableEntry();

protected:
	MACAddress id;
	uint16_t RTT;
	vector<uint16_t> LLIDs;
	bool wdmEnabled;
	uint8_t channel;
	uint8_t dedicatedChannel;

public:
	void setId(MACAddress &mac);
	void setRTT(uint32_t rtt);
	int addLLID(uint16_t llid);
	void removeLLID(int idx);

	void setWdmEnabled(bool p_wdmEnabled) { wdmEnabled = p_wdmEnabled; }
	const bool getWdmEnabled() const { return wdmEnabled; }

	MACAddress getId();
	uint16_t getRTT();
	uint16_t getLLID(int idx);
	vector<uint16_t> & getLLIDs() const;
	int getLLIDsNum();
	uint8_t getChannel() const { return channel; }
	void setChannel(uint8_t p_channel) { channel = p_channel; }

	uint8_t getDedicatedChannel() { return dedicatedChannel; }
	void setDedicatedChannel(uint8_t ch) { dedicatedChannel = ch; }

	bool isValid();

	// Methods to override from cObject
	virtual cObject * dup();
	virtual std::string info();

	ONUTableEntry& operator=(const ONUTableEntry &en);
	friend std::ostream & operator<<(std::ostream &out, const ONUTableEntry &en);

};

#endif /* ONUTABLEENTRY_H_ */
