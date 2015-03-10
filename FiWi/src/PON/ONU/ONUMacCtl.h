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

#ifndef __ONUMACCTL_H__
#define __ONUMACCTL_H__

#include <omnetpp.h>
#include <string.h>
#include <vector>
#include "EtherFrame_m.h"
#include "MPCP_codes.h"
#include "EPON_messages_m.h"
#include "MACAddress.h"
#include "MPCPTools.h"
#include "EPON_mac.h"
#include "IExtPassiveQueue.h"
#include "PONUtil.h"
#include "IOLTMgmt.h"

// Self-message kind values
#define SEND_REPORT_MSG     100
#define SEND_BURST_MSG      101
#define TIMEOUT_REPORT_MSG  102
#define SEND_PON2_PACKET_MSG  103
#define EPON_POLLING_TIME_MSG  104

// Signal Q module to get new frame
#define GETFRAMEMSG			200
#define WAKEUPMSG			201

enum ONUMACState
{
	ONU_STATE_IDLE,
	ONU_STATE_TX
};

struct lessThanPacket
{
  bool operator()(const cPacket* p1, const cPacket* p2) const
  {
    return p1->getId() < p2->getId();
  }
};

struct ChannelInfos
{
	int id;

	// Dynamic Parameters from OLT
	simtime_t startBurst;
	uint32_t lenBurst;

	simtime_t endSimTimeAllocation(cSimpleModule* thisModule)
	{
		// lenBurst already contains all overhead
		return startBurst + (double)(lenBurst * 8) / (double)PONUtil::getCapacityOf(thisModule, id);
	}
};

class ONUMacCtl : public cSimpleModule
{
  protected:
	std::vector<ChannelInfos> channels;
	bool wdmEnabled;
	int nbWDMChannels;
	IOLTMgmt* oltMgmtObj;

	ONUMACState state;
	//cMessage* sendPON2PacketMsg;
	cMessage* eponPollingMsg;

	/// Pointer to the mac layer for some control and use of MAC address
	EPON_mac * emac;

	// Hold the number of MPCP GATEs
	int numGates;

	uint32_t txrate;
	std::set<cPacket*, lessThanPacket> burst;
	int nbPacketsInBurst;

	// Queue
	IExtPassiveQueue * queue_mod;		// Upper Layer Q


	// Statistics
	int numFramesFromHL;
	int numFramesFromLL;
	int numFramesFromHLDropped;
	simtime_t fragmentedTime;

  public:
	ONUMacCtl();
	virtual ~ONUMacCtl();
	static uint32_t calcPacketSizeOnMedium(uint32_t packetSizeInBytes);

  protected:
    virtual void initialize(int stage);
    virtual int numInitStages	() 	 const { return 7; }
    virtual void finish();
    virtual void handleMessage(cMessage *msg);

    virtual void processFrameFromHigherLayer(cMessage *msg);
    virtual void processFrameFromMAC(cMessage *msg);
    virtual void processMPCP(EtherFrame *frame );

    virtual void sendToLowerLayer(cMessage* msg);

    // void sendReportMessage();
    void sendPacket(int ch);
    void scheduleNextPacket();
    void handleEPONDataPacket(cMessage* msg);

    void reserveAndPrepareEPONBurst();

    // DEBUG
    virtual void dumpReg(ChannelInfos& ch);

    // TOOLS TODO: add to a different class
    virtual cModule * getNeighbourOnGate(const char * gate);

  private:
};

#endif
