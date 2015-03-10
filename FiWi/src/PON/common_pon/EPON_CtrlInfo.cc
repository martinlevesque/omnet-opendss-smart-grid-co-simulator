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

#include "EPON_CtrlInfo.h"
#include <omnetpp.h>

#include <sstream>

EPON_LLidCtrlInfo::EPON_LLidCtrlInfo() {
	llid = 0;
	channel = 0;
	createdAt = simTime();
}

EPON_LLidCtrlInfo::EPON_LLidCtrlInfo(int lid, int p_channel)
{
	llid = lid;
	channel = p_channel;
	createdAt = simTime();
}

EPON_LLidCtrlInfo::~EPON_LLidCtrlInfo()
{
}


std::string EPON_LLidCtrlInfo::info() const{
	std::stringstream ss;
	ss << "LLID = " << llid << std::string(", channel = ") << channel;
	return ss.str();
}

cObject * EPON_LLidCtrlInfo::dup() const{
	EPON_LLidCtrlInfo* t = new EPON_LLidCtrlInfo(llid, channel);

	t->setCreatedAt(createdAt);

	return t;
}
