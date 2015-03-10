/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 MIRKO BANCHI
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mirko Banchi <mk.banchi@gmail.com>
 */
#ifndef BLOCK_ACK_CACHE_H
#define BLOCK_ACK_CACHE_H

#include <omnetpp.h>
#include "BlockAckFrame.h"

class BlockAckCache : public cOwnedObject
{
public:
    BlockAckCache(const char *name=NULL);
    BlockAckCache(const BlockAckCache& queue);
    virtual ~BlockAckCache();
    BlockAckCache& operator=(const BlockAckCache& blockAckCache);
    virtual BlockAckCache *dup() const  {return new BlockAckCache(*this);}
	void init (uint16_t winStart, uint16_t winSize);
	void updateWithMpdu (const Ieee80211DataOrMgmtFrame *frame);
	void updateWithBlockAckReq (uint16_t startingSeq);
	void fillBlockAckBitmap (Ieee80211BlockAckFrame *blockAckframe);
	virtual void parsimPack(cCommBuffer *buffer);
	virtual void parsimUnpack(cCommBuffer *buffer);
private:
  void resetPortionOfBitmap (uint16_t start, uint16_t end);
  bool isInWindow (uint16_t seq);

  uint16_t m_winStart;
  uint8_t m_winSize;
  uint16_t m_winEnd;
  uint16_t m_bitmap[4096];
};

#endif /* BLOCK_ACK_CACHE_H */
