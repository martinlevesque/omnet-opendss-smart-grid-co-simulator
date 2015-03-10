#include "BlockAckFrame.h"
void Ieee80211BlockAckFrame::setTypeAckBlock (enum BlockAckType type)
{
  switch (type) {
    case BASIC_BLOCK_ACK:
      setMultiTid(false);
      setCompressed(false);
      break;
    case COMPRESSED_BLOCK_ACK:
      setMultiTid(false);
      setCompressed(true);
      break;
    case MULTI_TID_BLOCK_ACK:
      setMultiTid(true);
      setCompressed(true);
      break;
    default:
      opp_error ("Invalid variant type");
      break;
  }
}


uint16_t
Ieee80211BlockAckFrame::getStartingSequenceControl (void) const
{
  return (getStartingSequence() << 4) & 0xfff0;
}

void
Ieee80211BlockAckFrame::setStartingSequenceControl (uint16_t seqControl)
{
  setStartingSequence((seqControl >> 4) & 0x0fff);
}

void
Ieee80211BlockAckFrame::setReceivedPacket (uint16_t seq)
{
  if (!isInBitmap (seq))
    return;
  if (!getMultiTid())
    {
      if (!getCompressed())
        {
          /* To set correctly basic block ack bitmap we need fragment number too.
             So if it's not specified, we consider packet not fragmented. */
          bitmap.m_bitmap[indexInBitmap (seq)] |= 0x0001;
        }
      else
        {
          bitmap.m_compressedBitmap |= (uint64_t(0x0000000000000001) << indexInBitmap (seq));
        }
    }
  else
    {
      if (getCompressed())
        {
          opp_error ("Multi-tid block ack is not supported.");
        }
      else
        {
          opp_error ("Reserved configuration.");
        }
    }
}

void
Ieee80211BlockAckFrame::setReceivedFragment (uint16_t seq, uint8_t frag)
{
  ASSERT (frag < 16);
  if (!isInBitmap (seq))
    return;
  if (!getMultiTid())
    {
      if (!getCompressed())
        {
          bitmap.m_bitmap[indexInBitmap (seq)] |= (0x0001<<frag);
        }
      else
        {
          /* We can ignore this...compressed block ack doesn't support
             acknowledgement of single fragments */
        }
    }
  else
    {
      if (getCompressed())
        {
          opp_error ("Multi-tid block ack is not supported.");
        }
      else
        {
          opp_error ("Reserved configuration.");
        }
    }
}

bool
Ieee80211BlockAckFrame::isPacketReceived (uint16_t seq) const
{
  if (!isInBitmap (seq))
    return false;
  if (!getMultiTid())
    {
      if (!getCompressed())
        {
          /*It's impossible to say if an entire packet was correctly received. */
          return false; 
        }
      else
        {
          uint64_t mask = uint64_t(0x0000000000000001);
          return (((bitmap.m_compressedBitmap >> indexInBitmap (seq)) & mask) == 1)?true:false;
        }
    }
  else
    {
      if (getCompressed())
        {
          opp_error ("Multi-tid block ack is not supported.");
        }
      else
        {
          opp_error ("Reserved configuration.");
        }
    }
  return false;
}

bool
Ieee80211BlockAckFrame::isFragmentReceived (uint16_t seq, uint8_t frag) const
{
  ASSERT (frag < 16);
  if (!isInBitmap (seq))
    return false;
  if (!getMultiTid())
    {
      if (!getCompressed())
        {
          return ((bitmap.m_bitmap[indexInBitmap (seq)] & (0x0001<<frag)) != 0x0000)?true:false;
        }
      else
        {
          /* Although this could make no sense, if packet with sequence number
             equal to <i>seq</i> was correctly received, also all of its fragments 
             were correctly received. */
          uint64_t mask = uint64_t(0x0000000000000001);
          return (((bitmap.m_compressedBitmap >> indexInBitmap (seq)) & mask) == 1)?true:false;
        }
    }
  else
    {
      if (getCompressed())
        {
          opp_error ("Multi-tid block ack is not supported.");
        }
      else
        {
          opp_error ("Reserved configuration.");
        }
    }
  return false;
}

uint8_t
Ieee80211BlockAckFrame::indexInBitmap (uint16_t seq) const
{
  uint8_t index;
  if (seq >= getStartingSequence())
    {
      index = seq - getStartingSequence();
    }
  else
    {
      index = 4096 - getStartingSequence() + seq;
    }
  ASSERT (index <= 63);
  return index;
}

bool
Ieee80211BlockAckFrame::isInBitmap (uint16_t seq) const
{
  return (seq - getStartingSequence() + 4096) % 4096 < 64;
}

const uint16_t*
Ieee80211BlockAckFrame::getBitmap (void) const
{
  return bitmap.m_bitmap;
}

uint64_t
Ieee80211BlockAckFrame::getCompressedBitmap (void) const
{
  return bitmap.m_compressedBitmap;
}

void
Ieee80211BlockAckFrame::resetBitmap (void)
{
  memset (&bitmap, 0, sizeof (bitmap));
}

