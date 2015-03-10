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

#ifndef __ONU_Q_MGMT_PERLLID_H__
#define __ONU_Q_MGMT_PERLLID_H__

#include <omnetpp.h>
#include <string>
#include "EtherFrame_m.h"
#include "MPCP_codes.h"
#include "EPON_messages_m.h"
#include "MACAddress.h"
#include "MPCPTools.h"
#include "MPCP_codes.h"
#include "EPON_mac.h"
#include "IExtPassiveQueue.h"
#include "ONUMacCtl.h"
#include "MyAbstractQueue.h"


// Self-Messages
#define REGTOMSG			100
#define REGSENDMSG			101
#define REPORTINITIALCONFIGMSG			102

/**
 * ONU_Q_mgmt_PerLLiD creates on queue per LLID. Here is actually done
 * all the bandwidth allocation and timer calculation. Also this class handles the
 * ONU registration and the updates the ONU table. Thus some MPCP messages are
 * generated from here and timeouts are defined here. Now since all the time
 * management is done here... we have to define the number of time slots and the
 * time slot duration. Finally this class is aware of any services in this network
 * and thus has a pointer to the SrvList class (if any module is available in the
 * scenario else it is NULL).
 *
 * The whole idea is to extend this class and create your own bandwidth allocation
 * algorithms. The default behavior of this class is to apply round robin on the
 * queues (per frame). The only thing considered is the priority (not strictly) and
 * happens because the queues are sorted based on the service priority.
 *
 * Basic methods that would be useful to override when you extend are explained
 * below.
 */
class ONUQPerLLiDBase : public cSimpleModule, public IExtPassiveQueue
{
  public:
	  ONUQPerLLiDBase();
	  virtual ~ONUQPerLLiDBase();

  protected:

	// Parameters
	double regTimeOut;
	int queueLimit;

	bool wdmEnabled;
	int nbWDMChannels;
	long nbPacketsReceivedFromHigherLayer;
	std::string queuingStrategy;
	std::string admissionControlConfs;

	// SimpleQueue fifoQueue;
	MyAbstractQueue* queue;
	uint16_t llid;

	int dedicatedChannel; // when AWG

	// Self Messages
	cMessage *regTOMsg, * regSendMsg;

    virtual void initialize(int stage);
    virtual int numInitStages() 	 const { return 4; }

    virtual void finish();
    virtual void handleMessage(cMessage *msg);

    virtual void processFrameFromHigherLayer(cMessage *msg);
    virtual void processFrameFromLowerLayer(cMessage *msg);

    // MPCP Operation
    virtual void processMPCP(EthernetIIFrame *frame );
    virtual void sendMPCPReg();

    virtual void startMPCPReg();

    cModule * findModuleUp(const char * name);

    /**
	 * The queue should send a packet whenever this method is invoked.
	 * If the queue is currently empty, it should send a packet when
	 * when one becomes available.
	 */
	virtual int requestPacket(int maxPacketSize) = 0;
	virtual uint32_t queueSizeInBytes() = 0;

	virtual bool isEmpty();

};

#endif
