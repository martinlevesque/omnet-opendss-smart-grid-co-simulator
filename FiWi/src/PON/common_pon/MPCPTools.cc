/*
 * MPCPTools.cpp
 *
 *  Created on: Apr 12, 2010
 *      Author: urban
 */

#include "MPCPTools.h"

MPCPTools::MPCPTools() {
	// TODO Auto-generated constructor stub

}

MPCPTools::~MPCPTools()
{
	// TODO Auto-generated destructor stub
}

uint64_t MPCPTools::simTimeToNS16(){
	// NOTE: You can add skew here...
	int scale= (-9 -simTime().getScaleExp())/3;
	// Clock Granularity is 16ns
	return (simTime().raw()/(scale*1000*16))%MPCP_CLOCK_MAX;
}

uint64_t MPCPTools::simTimeToNS16(uint64_t time){
	int scale= (-9 -simTime().getScaleExp())/3;
	// Clock Granularity is 16ns
	return (time/(scale*1000*16))%MPCP_CLOCK_MAX;
}

uint64_t MPCPTools::ns16ToSimTime(uint64_t time){
	int scale= (-9 -simTime().getScaleExp())/3;
	// Clock Granularity is 16ns
	return time * (scale*1000*16);
}

uint64_t MPCPTools::nsToSimTime(uint64_t time){
	int scale= (-9 -simTime().getScaleExp())/3;
	// Clock Granularity is 16ns
	return time * (scale*1000);
}
