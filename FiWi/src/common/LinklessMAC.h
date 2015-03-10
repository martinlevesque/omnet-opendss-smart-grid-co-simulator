/*
 * Copyright (C) 2013 Martin Lévesque <levesque.martin@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __FIWI_LINKLESS_MAC_H
#define __FIWI_LINKLESS_MAC_H

#include "INETDefs.h"
#include "Ethernet.h"
#include "EtherFrame_m.h"
#include "EtherMACBase.h"
#include "MACAddress.h"


/**
 *
 */
class INET_API LinklessMAC : public EtherMACBase
{
public:
	LinklessMAC();
    virtual ~LinklessMAC();

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


