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

#ifndef __EPON_RELAY_H__
#define __EPON_RELAY_H__

#include <omnetpp.h>
#include <map>
#include "EtherFrame_m.h"
#include "EtherFrameVlans_m.h"
#include "MACVlanRelayUnitBase.h"
#include "EPON_CtrlInfo.h"
#include "EPON_messages_m.h"
#include "EtherMacVlan.h"

/**
 * The default relay way. As mentioned in the MACVlanRelayUnitBase this
 * module is responsible for packet differentiation. The specific module
 * is based on the destination MAC address and is going to forward the frame
 * to all the known LLIDs. If the LLID is -1 (UNDEFINED) then the frame is
 * going to be broadcasted. The same will happen if we do not know any LLID
 * for that MAC address or if the ONU doesn't have any LLID assigned.
 *
 * NOTE here that mapping to LLIDs could be done with various ways based on the
 * VLAN or on the DSCP in the IP header or even per-port or pre-defined per
 * destination. Depending on the scenario, someone can extend the MACVlanRelayUnitBase
 * and change the relay's behavior.
 */
class EPON_ONU_relayDefault : public MACVlanRelayUnitBase
{
  protected:
	long numProcessedFrames;
	long numDroppedFrames;
	EtherMacVlan* vlanMac;

	typedef std::vector<uint16_t> llidTable;
	llidTable llids;

	MACAddress myAddress;

	std::vector<MACAddress> adjNodes;

  protected:
    virtual void initialize(int stage);
    virtual int numInitStages() const {return 3;}

    virtual void handleMessage(cMessage *msg);

    virtual void handleFromPON(EtherFrame *frame);
    virtual void handleFromLAN(EtherFrame *frame);

    bool checkSendToEthOut(EtherFrame* frame);

    virtual cModule * findModuleUp(const char * name);

    bool isConnectedToONU(MACAddress addr);

};

#endif
