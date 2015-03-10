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
#include "BlockAckCache.h"
#include "Ieee80211Frame_m.h"
#include <string.h>

static bool
QosUtilsIsOldPacket (uint16_t startingSeq, uint16_t seqNumber)
{
  ASSERT (startingSeq < 4096);
  ASSERT (seqNumber < 4096);
  uint16_t distance = ((seqNumber - startingSeq) + 4096) % 4096;
  return (distance >= 2048);
}

#define WINSIZE_ASSERT ASSERT ((m_winEnd - m_winStart + 4096) % 4096 == m_winSize - 1)

BlockAckCache::BlockAckCache(const BlockAckCache& block) : cOwnedObject()
{
    setName( block.getName() );
    operator=(block);
}

BlockAckCache::BlockAckCache(const char *name) : cOwnedObject(name)
{
  m_winStart=0;
  m_winSize=0;
  m_winEnd=0;
  memset(m_bitmap,0,sizeof(uint16_t)*4096);
}

BlockAckCache::~BlockAckCache()
{
}

BlockAckCache& BlockAckCache::operator=(const BlockAckCache& block)
{
    if (this==&block) return *this;
    cOwnedObject::operator=(block);
    m_winStart=block.m_winStart;
    m_winSize=block.m_winSize;
    m_winEnd=block.m_winEnd;
    memcpy(m_bitmap,block.m_bitmap,sizeof(m_bitmap));
    return *this;
}


void BlockAckCache::parsimPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cOwnedObject::parsimPack(buffer);
	buffer->pack(m_winStart);
	buffer->pack(m_winSize);
	buffer->pack(m_winEnd);
	buffer->pack(m_bitmap,4096);

#endif
}

void BlockAckCache::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cOwnedObject::parsimUnpack(buffer);
	buffer->unpack(m_winStart);
	buffer->unpack(m_winSize);
	buffer->unpack(m_winEnd);
	buffer->unpack(m_bitmap,4096);
#endif
}


void
BlockAckCache::init (uint16_t winStart, uint16_t winSize)
{
  m_winStart = winStart;
  m_winSize = winSize <= 64 ? winSize : 64;
  m_winEnd = ((m_winStart + m_winSize) % 4096) - 1;
  memset (m_bitmap, 0, sizeof (m_bitmap));
}

void
BlockAckCache::updateWithMpdu (const Ieee80211DataOrMgmtFrame *frame)
{
  uint16_t seqNumber = frame->getSequenceNumber ();
  if (!QosUtilsIsOldPacket (m_winStart, seqNumber))
    {
      if (!isInWindow (seqNumber))
        {
          uint16_t delta = (seqNumber - m_winEnd + 4096) % 4096;
          if (delta > 1)
            {
              resetPortionOfBitmap ((m_winEnd + 1) % 4096, ((seqNumber - 1) + 4096) % 4096);
            }
          m_winStart = (m_winStart + delta) % 4096;
          m_winEnd = seqNumber;

          WINSIZE_ASSERT;
        }
      m_bitmap[seqNumber] |= (0x0001<<frame->getFragmentNumber ());
    }
}

void
BlockAckCache::updateWithBlockAckReq (uint16_t startingSeq)
{
  if (!QosUtilsIsOldPacket (m_winStart, startingSeq))
    {
      if (isInWindow (startingSeq))
        {
          if (startingSeq != m_winStart)
            {
              m_winStart = startingSeq;
              uint16_t newWinEnd = ((m_winStart + m_winSize) % 4096) - 1;
              resetPortionOfBitmap ((m_winEnd + 1) % 4096, newWinEnd);
              m_winEnd = newWinEnd;

              WINSIZE_ASSERT;
            }
        }
      else
        {
          m_winStart = startingSeq;
          m_winEnd = ((m_winStart + m_winSize) % 4096) - 1;
          resetPortionOfBitmap (m_winStart, m_winEnd);

          WINSIZE_ASSERT;
        }
    }
}

void
BlockAckCache::resetPortionOfBitmap (uint16_t start, uint16_t end)
{
  uint32_t i = start;
  for (; i != end; i = (i + 1) % 4096)
    {
      m_bitmap[i] = 0; 
    }
  m_bitmap[i] = 0;
}

bool
BlockAckCache::isInWindow (uint16_t seq)
{
  return ((seq - m_winStart + 4096) % 4096) < m_winSize;
}

void
BlockAckCache::fillBlockAckBitmap (Ieee80211BlockAckFrame *blockAckframe)
{
  if (blockAckframe->isBasic ())
    {
      opp_error ("Basic block ack is only partially implemented.");
    }
  else if (blockAckframe->isCompressed ())
    {
      uint32_t i = blockAckframe->getStartingSequence ();
      uint32_t end = ((i + m_winSize) % 4096) - 1; 
      for (; i != end; i = (i + 1) % 4096)
        {
          if (m_bitmap[i] == 1)
            {
        	  blockAckframe->setReceivedPacket (i);
            }
        }
      if (m_bitmap[i] == 1)
        {
    	  blockAckframe->setReceivedPacket (i);
        }
    }
  else if (blockAckframe->isMultiTid ())
    {
      opp_error ("Multi-tid block ack is not supported.");
    }
}

