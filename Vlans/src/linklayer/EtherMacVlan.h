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

#ifndef __ETHERMACVLAN_H__
#define __ETHERMACVLAN_H__

#include <string>
#include <omnetpp.h>
#include "EtherMAC.h"
#include "EtherVlanDefs.h"
#include "IInterfaceTable.h"
#include "InterfaceTableAccess.h"

/**
 * Currently not used, may be needed thought in the future to allow
 * bigger frames to go through...
 */
class EtherMacVlan : public EtherMAC
{
  protected:
	/**
	 * Override from EtherMAC and EtherMACBase in order to allow
	 * 1522 byte Frames (12(macs)+4(vlan)+2(len)+4(FCS) = 22 (mtu:1500))
	 */
	virtual void processFrameFromUpperLayer(EtherFrame *frame);

	virtual void processMsgFromNetwork(cPacket *msg);

	/**
	 * Override to clone packet name to the duplicated frame
	 */
	virtual void startFrameTransmission();
};

#endif
