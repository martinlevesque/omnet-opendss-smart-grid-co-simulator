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

#include <string>
#include <stdlib.h>
#include <cobject.h>
#include <inttypes.h>
#include <stdio.h>
#include <omnetpp.h>

#ifndef EPON_LLID_CTRLINFO_H_
#define EPON_LLID_CTRLINFO_H_

const uint16_t LLID_EPON_BC=0x7FFF;


/**
 * EPON_LLidCtrlInfo extends cObject and is used for
 * ControlInfo carried in the messages. It currently
 * contains only LLID (Logical Link ID) information.
 * This information are added by the module generated
 * the message OR by the RelayUnit which is responsible
 * for traffic differentiation. Finally, this is used
 * in the EPON_mac module in order to generate the correct
 * Ethernet preamble.
 *
 * The methods are the classical ones: Constructor/Destructor,
 * dup() and info() (needed to extend cObject).
 */
class EPON_LLidCtrlInfo : public cObject{

public:
	int llid;

	int channel;

	simtime_t createdAt;

	EPON_LLidCtrlInfo();
	EPON_LLidCtrlInfo(int lid, int p_channel);
	virtual ~EPON_LLidCtrlInfo();

	int getChannel() { return channel; }
	void setCreatedAt(simtime_t t) { createdAt = t; }

	virtual cObject * dup() const;
	virtual std::string info() const;
};

#endif /* EPON_LLID_CTRLINFO_H_ */
