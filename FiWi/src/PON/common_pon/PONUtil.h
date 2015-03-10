/*
 * PONUtil.h
 *
 *  Created on: Aug 24, 2011
 *      Author: martin
 */

#include "Lambda.h"
#include "FiWiGeneralConfigs.h"

#ifndef PONUTIL_H_
#define PONUTIL_H_

class PONUtil
{
public:
	static double getCapacityOf(cSimpleModule* thisModule, int channel);
	static Lambda* getLambda(cSimpleModule* thisModule, int channel);
	static void checkUpstreamBusy(cSimpleModule* thisModule, int channel);
	static void checkDownstreamBusy(cSimpleModule* thisModule, int channel);
	static void sendUpstream(cSimpleModule* thisModule, double bits, int channel);
	static void sendDownstream(cSimpleModule* thisModule, double bits, int channel);
	static int getPonVersion(cSimpleModule* thisModule);
	static FiWiGeneralConfigs* getFiWiGeneralConfigs(cSimpleModule* thisModule);

};

#endif /* PONUTIL_H_ */
