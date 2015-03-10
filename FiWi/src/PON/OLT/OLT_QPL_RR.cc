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

#include "OLT_QPL_RR.h"

Define_Module(OLT_QPL_RR);

OLT_QPL_RR::~OLT_QPL_RR(){}

void OLT_QPL_RR::initialize(int stage)
{
	OLTQPerLLiDBase::initialize(stage);
}

void OLT_QPL_RR::requestPacket()
{
	Enter_Method("requestPacket()");

	// send(msg, "lowerLayerOut");
}


