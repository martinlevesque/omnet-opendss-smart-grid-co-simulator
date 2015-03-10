/*
 * PONUtil.cpp
 *
 *  Created on: Aug 24, 2011
 *      Author: martin
 */

#include <omnetpp.h>
#include "PONUtil.h"
#include "MyUtil.h"
#include "Lambda.h"

double PONUtil::getCapacityOf(cSimpleModule* thisModule, int channel)
{
	return PONUtil::getLambda(thisModule, channel)->getCapacity();
}

Lambda* PONUtil::getLambda(cSimpleModule* thisModule, int channel)
{
	return dynamic_cast<Lambda*>(MyUtil::findModuleUp(thisModule, "lambdas", channel));
}

void PONUtil::checkUpstreamBusy(cSimpleModule* thisModule, int channel)
{
	if (PONUtil::getLambda(thisModule, channel)->upstreamBusy())
	{
		thisModule->error("PONUtil::checkUpstreamBusy - busy, channel = %d, last sched end at = %f", channel,
				PONUtil::getLambda(thisModule, channel)->upstreamChannelReservation.end.dbl());
	}
}

void PONUtil::checkDownstreamBusy(cSimpleModule* thisModule, int channel)
{
	if (PONUtil::getLambda(thisModule, channel)->downstreamBusy())
	{
		thisModule->error("PONUtil::checkDownstreamBusy - busy, channel = %d, last sched end at = %f", channel,
				PONUtil::getLambda(thisModule, channel)->downstreamChannelReservation.end.dbl());
	}
}

void PONUtil::sendUpstream(cSimpleModule* thisModule, double bits, int channel)
{
	PONUtil::getLambda(thisModule, channel)->sendUpstream(bits, channel);
}

void PONUtil::sendDownstream(cSimpleModule* thisModule, double bits, int channel)
{
	PONUtil::getLambda(thisModule, channel)->sendDownstream(bits, channel);
}

int PONUtil::getPonVersion(cSimpleModule* thisModule)
{
	return PONUtil::getFiWiGeneralConfigs(thisModule)->getPonVersion();
}

FiWiGeneralConfigs* PONUtil::getFiWiGeneralConfigs(cSimpleModule* thisModule)
{
	return dynamic_cast<FiWiGeneralConfigs*>(MyUtil::findModuleUp(thisModule, "fiWiGeneralConfigs"));
}

