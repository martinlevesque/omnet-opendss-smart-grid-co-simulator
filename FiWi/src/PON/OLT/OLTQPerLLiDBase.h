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

#ifndef __OLT_Q_MGMT_PERLLID_H__
#define __OLT_Q_MGMT_PERLLID_H__

#include <omnetpp.h>
#include <vector>
#include "ONUTable.h"
#include "EPON_messages_m.h"
#include "MPCP_codes.h"
#include "MPCPTools.h"
#include "ONUTable.h"
#include "MACVlanRelayUnitBase.h"
#include "OLTMacCtl.h"
#include "IPassiveQueue.h"
#include "EPON_CtrlInfo.h"
#include "CopyableQueueCVectors.h"
#include "IOLTMgmt.h"

// Self Messages
#define ONUPENACK	100


/**
 * Allocation per MAC-LLID. Holds all the information around a
 * queue per LLID. These include service name, priority, max. committed
 * data rate and currently sent bytes (to be used later on DBA algorithm).
 * Also this class holds all the scalar and vector parameters needed for
 * logging.
 *
 * NOTE: The cOutVector class is not copyable... which means that we cannot
 * have objects of this type in this class and use it in a vector (copy constructor
 * will be called). So what we did is to have pointers to the cOutVectors which
 * can be copied.
 *
 * NOTE: Call the clean() method on this class to free all the vectors...
 */
class QueuePerMacLLid : public QForContainer{
public:
	// Only the index... all the others are inherited
	mac_llid ml;
	bool isDefault;

	QueuePerMacLLid() : QForContainer(){}
	~QueuePerMacLLid(){}

	// Friends
	friend std::ostream & operator<<(std::ostream & out, QueuePerMacLLid & qml){
		out<<qml.ml<<" SrvName: "<<qml.getServiceName()<<" QueueSize: "<<qml.length();
		return out;
	}
};

/**
 * OLT_Q_mgmt_PerLLiD creates on queue per MAC-LLID tuple. Here is actually done
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
class OLTQPerLLiDBase : public cSimpleModule, public IPassiveQueue, public IOLTMgmt
{
  public:
	OLTQPerLLiDBase();
	virtual ~OLTQPerLLiDBase();

  protected:
	// Pointer to the ONU table
	ONUTable * onutbl;

	OLTMacCtl* oltMac;

	vector<ONUTableEntry> temptbl;
	vector<cMessage *> pendingAck;

	int nbWDMChannels;
	int nbONUs;

	//Parameters
	double regAckTimeOut;
	int32_t regTimeInt;
	int queueLimit;

	int maximumGrantSize;
	double propagationDelay;

	double maximumQueueDurationControl;

    virtual void initialize(int stage);
    virtual int numInitStages	() 	 const { return 7; }
    virtual void finish();
    virtual void handleMessage(cMessage *msg);

    virtual void processFrameFromHigherLayer(cMessage *msg);
    virtual void processFrameFromLowerLayer(cMessage *msg);

    // MPCP Operations
    virtual void processMPCP(EthernetIIFrame *frame );
    virtual void handleRegTimeOut(cMessage *msg);
    virtual void doOnuRegistration(MACAddress mac);

    //double durationGateMsg(int nbSlots);
    //double durationReport();
    int sizeGateMsgInBytes(int nbSlots);
    MPCPGate * makeRegPacket();


    // TOOLS TODO: add to a different class
    virtual cModule * findModuleUp(const char * name);

    /**
     * This method calculates the upstream timers
     * for each ONU in the ONUTable. This could be overridden
     * to change the default behavior. The default is fair
     * allocation per ONU.
     *
     * NOTE: 2 Basic RULES At the end this method should: <br>
     *  - Have set the _total_ length and start registers on the ONU table
     *  - Call SendGateUpdates() to announce to ONUs
     */
	virtual std::vector<ONUReservation> DoUpstreamDBA(MACAddress onuAddr, int newSlotLength);

	/**
	 * Generate and send the initial gate message. This message
	 * is used for the registration of the ONUs.
	 */
	virtual void sendMPCPGateReg();

	// void SendGateUpdate(ONUReservation reportReservation, std::vector<ONUReservation> burstReservation);
};

#endif
