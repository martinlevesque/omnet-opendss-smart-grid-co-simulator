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

#ifndef IEEE80211FRAMEBLOCK_H_
#define IEEE80211FRAMEBLOCK_H_
#include <omnetpp.h>

class FrameBlock : public cPacket
{
private:
    struct ShareStruct{
        cPacket * pkt;
        unsigned int shareCount;
        ShareStruct(){
            pkt=NULL;
            shareCount =0;
        }
    };
    std::vector<ShareStruct*> encapsulateVector;
    void _deleteEncapVector();
    bool _checkIfShare();
    void _detachShareVector(unsigned int i);
public:
    FrameBlock(const char *name=NULL, int kind=0);
    FrameBlock(FrameBlock &);
    virtual FrameBlock * dup(){return new FrameBlock(*this);}
    virtual ~FrameBlock();
    FrameBlock& operator=(const FrameBlock& msg);
    virtual cPacket *getPacket(unsigned int i) const;
    virtual void setPacketKind(unsigned int i,int kind);
    virtual unsigned int getNumEncap() const {return encapsulateVector.size();}
    uint64_t getPktLength(unsigned int i) const
    {
        if (i<encapsulateVector.size())
            return encapsulateVector[i]->pkt->getBitLength();
        return 0;
    }

    virtual void pushFrom(cPacket *);
    virtual void pushBack(cPacket *);
    virtual cPacket *popFrom();
    virtual cPacket *popBack();
    virtual bool haveBlock(){return !encapsulateVector.empty();}
    virtual void forEachChild(cVisitor *v);

    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);
};

inline void doPacking(cCommBuffer *b, FrameBlock& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, FrameBlock& obj) {obj.parsimUnpack(b);}

#endif /* IEEE80211FRAMEBLOCK_H_ */
