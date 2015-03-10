// Copyright (C) 2009 Juan-Carlos Maureira
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

#include "ProgramedFailureChannel.h"

Register_Class(ProgramedFailureChannel);

ProgramedFailureChannel::ProgramedFailureChannel(const char* name) : cDatarateChannel(name)
{


}

ProgramedFailureChannel::~ProgramedFailureChannel()
{


}

bool ProgramedFailureChannel::initializeChannel(int stage)
{
    cDatarateChannel::initializeChannel(stage);

    this->lfm = (LinkFailureManager*)(simulation.getContextModule()->getSubmodule("linkFailureManager"));

    if (this->lfm==NULL)
    {
        // simulation does not have a linkFailureManager, inserting one

        cModuleType *moduleType = cModuleType::get("inet.underTest.channels.LinkFailureManager");
        this->lfm = (LinkFailureManager*)moduleType->create("linkFailureManager", simulation.getContextModule());
        this->lfm->buildInside();
        this->lfm->scheduleStart( simTime() );

    }

    cGate* source_port = this->getSourceGate();

    // schedule the failures on this link
    cStringTokenizer tokenizer_failures(par("failureAt"),",");
    while (tokenizer_failures.hasMoreTokens())
    {
        simtime_t when = atof(tokenizer_failures.nextToken());
        this->lfm->scheduleLinkStateChange(source_port, when, DOWN);
    }

    // schedule the recoveries on this link
    cStringTokenizer tokenizer_recoveries(par("recoveryAt"),",");
    while (tokenizer_recoveries.hasMoreTokens())
    {
        simtime_t when = atof(tokenizer_recoveries.nextToken());
        this->lfm->scheduleLinkStateChange(source_port, when, UP);
    }

    return false;
}

#if OMNETPP_VERSION>0x0400
void ProgramedFailureChannel::processMessage(cMessage *msg, simtime_t at, result_t &result)
{
    cDatarateChannel::processMessage(msg,at,result);
}
#else
bool ProgramedFailureChannel::deliver(cMessage *msg, simtime_t at)
{
    bool ret = cDatarateChannel::deliver(msg, at);
    return ret;
}
#endif

void ProgramedFailureChannel::setState(LinkState state)
{
    if (state == UP)
    {
        EV << "Generating a link recovery in the link from " << this->getSourceGate()->getFullPath()  << endl;
        this->getDisplayString().setTagArg("ls",0,"black");
        this->setDisabled(false);
    }
    else if (state == DOWN)
    {
        EV << "Generating a link failure in the link from " << this->getSourceGate()->getFullPath()  << endl;
        this->getDisplayString().setTagArg("ls",0,"red");
        this->setDisabled(true);
    }
}

