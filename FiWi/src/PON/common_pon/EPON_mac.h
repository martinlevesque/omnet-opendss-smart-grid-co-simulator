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


#ifndef __EPON_MAC_H
#define __EPON_MAC_H

#include <omnetpp.h>
#include "INETDefs.h"
#include "EtherFrame_m.h"
#include "EtherMACBase.h"
#include "EtherVlanDefs.h"
#include "EPON_messages_m.h"
#include "EPON_CtrlInfo.h"


#define SLOT_SYNC_CODE (char)0x01001100L



// PREAMBLE_BYTES - 2 bytes for the LLID
#define EPON_PREAMBLE_BYTES (PREAMBLE_BYTES - 2)

/**
 * EPON_mac class is actually the EtherMAC2 class modified. From the
 * INET framework documentation:
 *
 * "A simplified version of EtherMAC. Since modern Ethernets typically
 * operate over duplex links where's no contention, the original CSMA/CD
 * algorithm is no longer needed. This simplified implementation doesn't
 * contain CSMA/CD, frames are just simply queued and sent out one by one."
 *
 * We use this class for some basic operations. The first one (and most
 * important) is to change the way the frame is transmitted. On EPON network
 * the Ethernet pre-amble includes the LLID (Logical Link ID). This LLID is
 * carried, from the higher layers, inside the cMessage ControInfo. Thus what
 * we do here is too remove the ControlInfo and use them in EtherFrameWithLLID.
 * The original message is encapsulated inside this frame. Finally the preamble
 * is added (-2 Bytes cause they are already in the EtherFrameWithLLID) and the
 * SFD. For incoming frame from the network we do the opposite process (decapsulate
 * the frame and add ControlInfo)
 *
 * The second usage of this module is to handle the IFG times that, based on the
 * drafts, should be there.
 *
 * Finally note here that the basic control of the transmission times is located
 * in the MacCtl modules (ONUMacCtl & OLTMacCtl), thus most of the times the queue
 * this module has is not used at all.
 */
class INET_API EPON_mac : public EtherMACBase
{
  public:
    EPON_mac();
    virtual ~EPON_mac();

  protected:
    u_int32_t paramTxrate;
	// cOutVector TxRateVector;

    bool withOverhead;

    virtual void initialize();
    virtual void finalize();
    virtual void finish();
    virtual void initializeTxrate();
    virtual void handleMessage(cMessage *msg);

    // event handlers
    virtual void startFrameTransmission();
    virtual void processFrameFromUpperLayer(EtherFrame *frame);
    virtual void processMsgFromNetwork(cPacket *msg);
    virtual void handleEndIFGPeriod();
    virtual void handleEndTxPeriod();

    // notifications
    virtual void updateHasSubcribers();

    void scheduleEndTxPeriod(cPacket *frame);

};

#endif

