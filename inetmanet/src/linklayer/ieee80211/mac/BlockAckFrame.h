#ifndef _IEEE80211BLOCKACKFRAME_H_
#define _IEEE80211BLOCKACKFRAME_H_
#include "Ieee80211Frame_m.h"

enum BlockAckType
{
  BASIC_BLOCK_ACK,
  COMPRESSED_BLOCK_ACK,
  MULTI_TID_BLOCK_ACK
};

class Ieee80211BlockAckFrameReq: public Ieee80211BlockAckFrameReq_Base
{

  public:
    virtual ~Ieee80211BlockAckFrameReq();
	Ieee80211BlockAckFrameReq(const char *name=NULL) : Ieee80211BlockAckFrameReq_Base(name) {}
    Ieee80211BlockAckFrameReq(const Ieee80211BlockAckFrameReq& other) : Ieee80211BlockAckFrameReq_Base(other) {}
    Ieee80211BlockAckFrameReq& operator=(const Ieee80211BlockAckFrameReq& other)
        {Ieee80211BlockAckFrameReq_Base::operator=(other); return *this;}
    virtual Ieee80211BlockAckFrameReq *dup() {return new Ieee80211BlockAckFrameReq(*this);}
    bool isBasic (void) const {
        return (!getMultiTid() && !getCompressed())?true:false;
    }
    bool isCompressed (void) const{
        return (!getMultiTid() && !getCompressed())?true:false;
    }
    bool isMultiTid (void) const{
       return (getMultiTid() && getCompressed())?true:false;
    }
};

class Ieee80211BlockAckFrame: public ::Ieee80211BlockAckFrame_Base
{
  protected:
     union {
       uint16_t m_bitmap[64];
       uint64_t m_compressedBitmap;
     } bitmap;
    uint8_t indexInBitmap (uint16_t seq) const;
  /**
   * Checks if sequence number <i>seq</i> can be acknowledged in the bitmap.
   */
    bool isInBitmap (uint16_t seq) const;
  public:
      virtual ~Ieee80211BlockAckFrame(){}
      Ieee80211BlockAckFrame(const char *name=NULL) : Ieee80211BlockAckFrame_Base(name) {}
      Ieee80211BlockAckFrame(const Ieee80211BlockAckFrame& other) : Ieee80211BlockAckFrame_Base(other) {}
      Ieee80211BlockAckFrame& operator=(const Ieee80211BlockAckFrame& other)
        {Ieee80211BlockAckFrame_Base::operator=(other); return *this;}
      virtual Ieee80211BlockAckFrame *dup() {return new Ieee80211BlockAckFrame(*this);}
      bool isBasic (void) const {
        return (!getMultiTid() && !getCompressed())?true:false;
      }
      bool isCompressed (void) const{
       return (!getMultiTid() && !getCompressed())?true:false;
      }
      bool isMultiTid (void) const{
          return (getMultiTid() && getCompressed())?true:false;
      }
      void setTypeAckBlock (enum BlockAckType type);
      virtual void setReceivedPacket (uint16_t seq);
      virtual void setReceivedFragment (uint16_t seq, uint8_t frag);
      virtual bool isPacketReceived (uint16_t seq) const;
      virtual bool isFragmentReceived (uint16_t seq, uint8_t frag) const;

      virtual uint16_t getStartingSequenceControl (void) const;
      virtual void setStartingSequenceControl (uint16_t seqControl);
      const uint16_t* getBitmap (void) const;
      uint64_t getCompressedBitmap (void) const;
      void resetBitmap (void);

      virtual void parsimPack(cCommBuffer *b){
    	  Ieee80211BlockAckFrame_Base::parsimPack(b);
    	  b->pack(bitmap.m_bitmap,64);
      }
      virtual void parsimUnpack(cCommBuffer *b){
    	  Ieee80211BlockAckFrame_Base::parsimUnpack(b);
    	  b->unpack(bitmap.m_bitmap,64);
      }
};
#endif
