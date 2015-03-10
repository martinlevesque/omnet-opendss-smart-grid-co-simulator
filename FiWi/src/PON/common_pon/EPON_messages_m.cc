//
// Generated file, do not edit! Created by opp_msgc 4.1 from PON/common_pon/EPON_messages.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "EPON_messages_m.h"

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




Register_Class(SyncCode);

SyncCode::SyncCode(const char *name, int kind) : cPacket(name,kind)
{
    this->code_var = 0;
}

SyncCode::SyncCode(const SyncCode& other) : cPacket()
{
    setName(other.getName());
    operator=(other);
}

SyncCode::~SyncCode()
{
}

SyncCode& SyncCode::operator=(const SyncCode& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->code_var = other.code_var;
    return *this;
}

void SyncCode::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->code_var);
}

void SyncCode::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->code_var);
}

char SyncCode::getCode() const
{
    return code_var;
}

void SyncCode::setCode(char code_var)
{
    this->code_var = code_var;
}

class SyncCodeDescriptor : public cClassDescriptor
{
  public:
    SyncCodeDescriptor();
    virtual ~SyncCodeDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(SyncCodeDescriptor);

SyncCodeDescriptor::SyncCodeDescriptor() : cClassDescriptor("SyncCode", "cPacket")
{
}

SyncCodeDescriptor::~SyncCodeDescriptor()
{
}

bool SyncCodeDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<SyncCode *>(obj)!=NULL;
}

const char *SyncCodeDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int SyncCodeDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount(object) : 1;
}

unsigned int SyncCodeDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *SyncCodeDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "code",
    };
    return (field>=0 && field<1) ? fieldNames[field] : NULL;
}

int SyncCodeDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='c' && strcmp(fieldName, "code")==0) return base+0;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *SyncCodeDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "char",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : NULL;
}

const char *SyncCodeDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int SyncCodeDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    SyncCode *pp = (SyncCode *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string SyncCodeDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    SyncCode *pp = (SyncCode *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getCode());
        default: return "";
    }
}

bool SyncCodeDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    SyncCode *pp = (SyncCode *)object; (void)pp;
    switch (field) {
        case 0: pp->setCode(string2long(value)); return true;
        default: return false;
    }
}

const char *SyncCodeDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
    };
    return (field>=0 && field<1) ? fieldStructNames[field] : NULL;
}

void *SyncCodeDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    SyncCode *pp = (SyncCode *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(EtherFrameWithLLID);

EtherFrameWithLLID::EtherFrameWithLLID(const char *name, int kind) : cPacket(name,kind)
{
    this->llid_var = 0;
    this->channel_var = 0;
}

EtherFrameWithLLID::EtherFrameWithLLID(const EtherFrameWithLLID& other) : cPacket()
{
    setName(other.getName());
    operator=(other);
}

EtherFrameWithLLID::~EtherFrameWithLLID()
{
}

EtherFrameWithLLID& EtherFrameWithLLID::operator=(const EtherFrameWithLLID& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->llid_var = other.llid_var;
    this->channel_var = other.channel_var;
    return *this;
}

void EtherFrameWithLLID::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->llid_var);
    doPacking(b,this->channel_var);
}

void EtherFrameWithLLID::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->llid_var);
    doUnpacking(b,this->channel_var);
}

uint16_t EtherFrameWithLLID::getLlid() const
{
    return llid_var;
}

void EtherFrameWithLLID::setLlid(uint16_t llid_var)
{
    this->llid_var = llid_var;
}

uint8_t EtherFrameWithLLID::getChannel() const
{
    return channel_var;
}

void EtherFrameWithLLID::setChannel(uint8_t channel_var)
{
    this->channel_var = channel_var;
}

class EtherFrameWithLLIDDescriptor : public cClassDescriptor
{
  public:
    EtherFrameWithLLIDDescriptor();
    virtual ~EtherFrameWithLLIDDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(EtherFrameWithLLIDDescriptor);

EtherFrameWithLLIDDescriptor::EtherFrameWithLLIDDescriptor() : cClassDescriptor("EtherFrameWithLLID", "cPacket")
{
}

EtherFrameWithLLIDDescriptor::~EtherFrameWithLLIDDescriptor()
{
}

bool EtherFrameWithLLIDDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<EtherFrameWithLLID *>(obj)!=NULL;
}

const char *EtherFrameWithLLIDDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int EtherFrameWithLLIDDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount(object) : 2;
}

unsigned int EtherFrameWithLLIDDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *EtherFrameWithLLIDDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "llid",
        "channel",
    };
    return (field>=0 && field<2) ? fieldNames[field] : NULL;
}

int EtherFrameWithLLIDDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='l' && strcmp(fieldName, "llid")==0) return base+0;
    if (fieldName[0]=='c' && strcmp(fieldName, "channel")==0) return base+1;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *EtherFrameWithLLIDDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "uint16_t",
        "uint8_t",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : NULL;
}

const char *EtherFrameWithLLIDDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int EtherFrameWithLLIDDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    EtherFrameWithLLID *pp = (EtherFrameWithLLID *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string EtherFrameWithLLIDDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    EtherFrameWithLLID *pp = (EtherFrameWithLLID *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getLlid());
        case 1: return ulong2string(pp->getChannel());
        default: return "";
    }
}

bool EtherFrameWithLLIDDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    EtherFrameWithLLID *pp = (EtherFrameWithLLID *)object; (void)pp;
    switch (field) {
        case 0: pp->setLlid(string2ulong(value)); return true;
        case 1: pp->setChannel(string2ulong(value)); return true;
        default: return false;
    }
}

const char *EtherFrameWithLLIDDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
    };
    return (field>=0 && field<2) ? fieldStructNames[field] : NULL;
}

void *EtherFrameWithLLIDDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    EtherFrameWithLLID *pp = (EtherFrameWithLLID *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

MPCP::MPCP() : EthernetIIFrame()
{
    this->opcode_var = 0;
    this->ts_var = 0;
}

MPCP::MPCP(const MPCP& other) : EthernetIIFrame()
{
    operator=(other);
}

MPCP::~MPCP()
{
}

MPCP& MPCP::operator=(const MPCP& other)
{
    if (this==&other) return *this;
    EthernetIIFrame::operator=(other);
    this->opcode_var = other.opcode_var;
    this->ts_var = other.ts_var;
    return *this;
}

void MPCP::parsimPack(cCommBuffer *b)
{
    doPacking(b,(EthernetIIFrame&)*this);
    doPacking(b,this->opcode_var);
    doPacking(b,this->ts_var);
}

void MPCP::parsimUnpack(cCommBuffer *b)
{
    doUnpacking(b,(EthernetIIFrame&)*this);
    doUnpacking(b,this->opcode_var);
    doUnpacking(b,this->ts_var);
}

uint16_t MPCP::getOpcode() const
{
    return opcode_var;
}

void MPCP::setOpcode(uint16_t opcode_var)
{
    this->opcode_var = opcode_var;
}

uint32_t MPCP::getTs() const
{
    return ts_var;
}

void MPCP::setTs(uint32_t ts_var)
{
    this->ts_var = ts_var;
}

class MPCPDescriptor : public cClassDescriptor
{
  public:
    MPCPDescriptor();
    virtual ~MPCPDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(MPCPDescriptor);

MPCPDescriptor::MPCPDescriptor() : cClassDescriptor("MPCP", "EthernetIIFrame")
{
}

MPCPDescriptor::~MPCPDescriptor()
{
}

bool MPCPDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<MPCP *>(obj)!=NULL;
}

const char *MPCPDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int MPCPDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount(object) : 2;
}

unsigned int MPCPDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *MPCPDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "opcode",
        "ts",
    };
    return (field>=0 && field<2) ? fieldNames[field] : NULL;
}

int MPCPDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='o' && strcmp(fieldName, "opcode")==0) return base+0;
    if (fieldName[0]=='t' && strcmp(fieldName, "ts")==0) return base+1;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *MPCPDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "uint16_t",
        "uint32_t",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : NULL;
}

const char *MPCPDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int MPCPDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    MPCP *pp = (MPCP *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string MPCPDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    MPCP *pp = (MPCP *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getOpcode());
        case 1: return ulong2string(pp->getTs());
        default: return "";
    }
}

bool MPCPDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    MPCP *pp = (MPCP *)object; (void)pp;
    switch (field) {
        case 0: pp->setOpcode(string2ulong(value)); return true;
        case 1: pp->setTs(string2ulong(value)); return true;
        default: return false;
    }
}

const char *MPCPDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
    };
    return (field>=0 && field<2) ? fieldStructNames[field] : NULL;
}

void *MPCPDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    MPCP *pp = (MPCP *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

MPCPGate::MPCPGate() : MPCP()
{
    this->startTimeReport_var = 0;
    this->durationReport_var = 0;
    this->nbSlots_var = 0;
    startTimeBurst_arraysize = 0;
    this->startTimeBurst_var = 0;
    durationBurst_arraysize = 0;
    this->durationBurst_var = 0;
    channelBurst_arraysize = 0;
    this->channelBurst_var = 0;
}

MPCPGate::MPCPGate(const MPCPGate& other) : MPCP()
{
    startTimeBurst_arraysize = 0;
    this->startTimeBurst_var = 0;
    durationBurst_arraysize = 0;
    this->durationBurst_var = 0;
    channelBurst_arraysize = 0;
    this->channelBurst_var = 0;
    operator=(other);
}

MPCPGate::~MPCPGate()
{
    delete [] startTimeBurst_var;
    delete [] durationBurst_var;
    delete [] channelBurst_var;
}

MPCPGate& MPCPGate::operator=(const MPCPGate& other)
{
    if (this==&other) return *this;
    MPCP::operator=(other);
    this->startTimeReport_var = other.startTimeReport_var;
    this->durationReport_var = other.durationReport_var;
    this->nbSlots_var = other.nbSlots_var;
    delete [] this->startTimeBurst_var;
    this->startTimeBurst_var = (other.startTimeBurst_arraysize==0) ? NULL : new int64[other.startTimeBurst_arraysize];
    startTimeBurst_arraysize = other.startTimeBurst_arraysize;
    for (unsigned int i=0; i<startTimeBurst_arraysize; i++)
        this->startTimeBurst_var[i] = other.startTimeBurst_var[i];
    delete [] this->durationBurst_var;
    this->durationBurst_var = (other.durationBurst_arraysize==0) ? NULL : new uint16_t[other.durationBurst_arraysize];
    durationBurst_arraysize = other.durationBurst_arraysize;
    for (unsigned int i=0; i<durationBurst_arraysize; i++)
        this->durationBurst_var[i] = other.durationBurst_var[i];
    delete [] this->channelBurst_var;
    this->channelBurst_var = (other.channelBurst_arraysize==0) ? NULL : new uint8_t[other.channelBurst_arraysize];
    channelBurst_arraysize = other.channelBurst_arraysize;
    for (unsigned int i=0; i<channelBurst_arraysize; i++)
        this->channelBurst_var[i] = other.channelBurst_var[i];
    return *this;
}

void MPCPGate::parsimPack(cCommBuffer *b)
{
    doPacking(b,(MPCP&)*this);
    doPacking(b,this->startTimeReport_var);
    doPacking(b,this->durationReport_var);
    doPacking(b,this->nbSlots_var);
    b->pack(startTimeBurst_arraysize);
    doPacking(b,this->startTimeBurst_var,startTimeBurst_arraysize);
    b->pack(durationBurst_arraysize);
    doPacking(b,this->durationBurst_var,durationBurst_arraysize);
    b->pack(channelBurst_arraysize);
    doPacking(b,this->channelBurst_var,channelBurst_arraysize);
}

void MPCPGate::parsimUnpack(cCommBuffer *b)
{
    doUnpacking(b,(MPCP&)*this);
    doUnpacking(b,this->startTimeReport_var);
    doUnpacking(b,this->durationReport_var);
    doUnpacking(b,this->nbSlots_var);
    delete [] this->startTimeBurst_var;
    b->unpack(startTimeBurst_arraysize);
    if (startTimeBurst_arraysize==0) {
        this->startTimeBurst_var = 0;
    } else {
        this->startTimeBurst_var = new int64[startTimeBurst_arraysize];
        doUnpacking(b,this->startTimeBurst_var,startTimeBurst_arraysize);
    }
    delete [] this->durationBurst_var;
    b->unpack(durationBurst_arraysize);
    if (durationBurst_arraysize==0) {
        this->durationBurst_var = 0;
    } else {
        this->durationBurst_var = new uint16_t[durationBurst_arraysize];
        doUnpacking(b,this->durationBurst_var,durationBurst_arraysize);
    }
    delete [] this->channelBurst_var;
    b->unpack(channelBurst_arraysize);
    if (channelBurst_arraysize==0) {
        this->channelBurst_var = 0;
    } else {
        this->channelBurst_var = new uint8_t[channelBurst_arraysize];
        doUnpacking(b,this->channelBurst_var,channelBurst_arraysize);
    }
}

int64 MPCPGate::getStartTimeReport() const
{
    return startTimeReport_var;
}

void MPCPGate::setStartTimeReport(int64 startTimeReport_var)
{
    this->startTimeReport_var = startTimeReport_var;
}

uint8_t MPCPGate::getDurationReport() const
{
    return durationReport_var;
}

void MPCPGate::setDurationReport(uint8_t durationReport_var)
{
    this->durationReport_var = durationReport_var;
}

uint8_t MPCPGate::getNbSlots() const
{
    return nbSlots_var;
}

void MPCPGate::setNbSlots(uint8_t nbSlots_var)
{
    this->nbSlots_var = nbSlots_var;
}

void MPCPGate::setStartTimeBurstArraySize(unsigned int size)
{
    int64 *startTimeBurst_var2 = (size==0) ? NULL : new int64[size];
    unsigned int sz = startTimeBurst_arraysize < size ? startTimeBurst_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        startTimeBurst_var2[i] = this->startTimeBurst_var[i];
    for (unsigned int i=sz; i<size; i++)
        startTimeBurst_var2[i] = 0;
    startTimeBurst_arraysize = size;
    delete [] this->startTimeBurst_var;
    this->startTimeBurst_var = startTimeBurst_var2;
}

unsigned int MPCPGate::getStartTimeBurstArraySize() const
{
    return startTimeBurst_arraysize;
}

int64 MPCPGate::getStartTimeBurst(unsigned int k) const
{
    if (k>=startTimeBurst_arraysize) throw cRuntimeError("Array of size %d indexed by %d", startTimeBurst_arraysize, k);
    return startTimeBurst_var[k];
}

void MPCPGate::setStartTimeBurst(unsigned int k, int64 startTimeBurst_var)
{
    if (k>=startTimeBurst_arraysize) throw cRuntimeError("Array of size %d indexed by %d", startTimeBurst_arraysize, k);
    this->startTimeBurst_var[k]=startTimeBurst_var;
}

void MPCPGate::setDurationBurstArraySize(unsigned int size)
{
    uint16_t *durationBurst_var2 = (size==0) ? NULL : new uint16_t[size];
    unsigned int sz = durationBurst_arraysize < size ? durationBurst_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        durationBurst_var2[i] = this->durationBurst_var[i];
    for (unsigned int i=sz; i<size; i++)
        durationBurst_var2[i] = 0;
    durationBurst_arraysize = size;
    delete [] this->durationBurst_var;
    this->durationBurst_var = durationBurst_var2;
}

unsigned int MPCPGate::getDurationBurstArraySize() const
{
    return durationBurst_arraysize;
}

uint16_t MPCPGate::getDurationBurst(unsigned int k) const
{
    if (k>=durationBurst_arraysize) throw cRuntimeError("Array of size %d indexed by %d", durationBurst_arraysize, k);
    return durationBurst_var[k];
}

void MPCPGate::setDurationBurst(unsigned int k, uint16_t durationBurst_var)
{
    if (k>=durationBurst_arraysize) throw cRuntimeError("Array of size %d indexed by %d", durationBurst_arraysize, k);
    this->durationBurst_var[k]=durationBurst_var;
}

void MPCPGate::setChannelBurstArraySize(unsigned int size)
{
    uint8_t *channelBurst_var2 = (size==0) ? NULL : new uint8_t[size];
    unsigned int sz = channelBurst_arraysize < size ? channelBurst_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        channelBurst_var2[i] = this->channelBurst_var[i];
    for (unsigned int i=sz; i<size; i++)
        channelBurst_var2[i] = 0;
    channelBurst_arraysize = size;
    delete [] this->channelBurst_var;
    this->channelBurst_var = channelBurst_var2;
}

unsigned int MPCPGate::getChannelBurstArraySize() const
{
    return channelBurst_arraysize;
}

uint8_t MPCPGate::getChannelBurst(unsigned int k) const
{
    if (k>=channelBurst_arraysize) throw cRuntimeError("Array of size %d indexed by %d", channelBurst_arraysize, k);
    return channelBurst_var[k];
}

void MPCPGate::setChannelBurst(unsigned int k, uint8_t channelBurst_var)
{
    if (k>=channelBurst_arraysize) throw cRuntimeError("Array of size %d indexed by %d", channelBurst_arraysize, k);
    this->channelBurst_var[k]=channelBurst_var;
}

class MPCPGateDescriptor : public cClassDescriptor
{
  public:
    MPCPGateDescriptor();
    virtual ~MPCPGateDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(MPCPGateDescriptor);

MPCPGateDescriptor::MPCPGateDescriptor() : cClassDescriptor("MPCPGate", "MPCP")
{
}

MPCPGateDescriptor::~MPCPGateDescriptor()
{
}

bool MPCPGateDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<MPCPGate *>(obj)!=NULL;
}

const char *MPCPGateDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int MPCPGateDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 6+basedesc->getFieldCount(object) : 6;
}

unsigned int MPCPGateDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<6) ? fieldTypeFlags[field] : 0;
}

const char *MPCPGateDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "startTimeReport",
        "durationReport",
        "nbSlots",
        "startTimeBurst",
        "durationBurst",
        "channelBurst",
    };
    return (field>=0 && field<6) ? fieldNames[field] : NULL;
}

int MPCPGateDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "startTimeReport")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "durationReport")==0) return base+1;
    if (fieldName[0]=='n' && strcmp(fieldName, "nbSlots")==0) return base+2;
    if (fieldName[0]=='s' && strcmp(fieldName, "startTimeBurst")==0) return base+3;
    if (fieldName[0]=='d' && strcmp(fieldName, "durationBurst")==0) return base+4;
    if (fieldName[0]=='c' && strcmp(fieldName, "channelBurst")==0) return base+5;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *MPCPGateDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int64",
        "uint8_t",
        "uint8_t",
        "int64",
        "uint16_t",
        "uint8_t",
    };
    return (field>=0 && field<6) ? fieldTypeStrings[field] : NULL;
}

const char *MPCPGateDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int MPCPGateDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    MPCPGate *pp = (MPCPGate *)object; (void)pp;
    switch (field) {
        case 3: return pp->getStartTimeBurstArraySize();
        case 4: return pp->getDurationBurstArraySize();
        case 5: return pp->getChannelBurstArraySize();
        default: return 0;
    }
}

std::string MPCPGateDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    MPCPGate *pp = (MPCPGate *)object; (void)pp;
    switch (field) {
        case 0: return int642string(pp->getStartTimeReport());
        case 1: return ulong2string(pp->getDurationReport());
        case 2: return ulong2string(pp->getNbSlots());
        case 3: return int642string(pp->getStartTimeBurst(i));
        case 4: return ulong2string(pp->getDurationBurst(i));
        case 5: return ulong2string(pp->getChannelBurst(i));
        default: return "";
    }
}

bool MPCPGateDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    MPCPGate *pp = (MPCPGate *)object; (void)pp;
    switch (field) {
        case 0: pp->setStartTimeReport(string2int64(value)); return true;
        case 1: pp->setDurationReport(string2ulong(value)); return true;
        case 2: pp->setNbSlots(string2ulong(value)); return true;
        case 3: pp->setStartTimeBurst(i,string2int64(value)); return true;
        case 4: pp->setDurationBurst(i,string2ulong(value)); return true;
        case 5: pp->setChannelBurst(i,string2ulong(value)); return true;
        default: return false;
    }
}

const char *MPCPGateDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<6) ? fieldStructNames[field] : NULL;
}

void *MPCPGateDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    MPCPGate *pp = (MPCPGate *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

MPCPReport::MPCPReport() : MPCP()
{
    this->lengthSlot_var = 0;
}

MPCPReport::MPCPReport(const MPCPReport& other) : MPCP()
{
    operator=(other);
}

MPCPReport::~MPCPReport()
{
}

MPCPReport& MPCPReport::operator=(const MPCPReport& other)
{
    if (this==&other) return *this;
    MPCP::operator=(other);
    this->lengthSlot_var = other.lengthSlot_var;
    return *this;
}

void MPCPReport::parsimPack(cCommBuffer *b)
{
    doPacking(b,(MPCP&)*this);
    doPacking(b,this->lengthSlot_var);
}

void MPCPReport::parsimUnpack(cCommBuffer *b)
{
    doUnpacking(b,(MPCP&)*this);
    doUnpacking(b,this->lengthSlot_var);
}

uint32_t MPCPReport::getLengthSlot() const
{
    return lengthSlot_var;
}

void MPCPReport::setLengthSlot(uint32_t lengthSlot_var)
{
    this->lengthSlot_var = lengthSlot_var;
}

class MPCPReportDescriptor : public cClassDescriptor
{
  public:
    MPCPReportDescriptor();
    virtual ~MPCPReportDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(MPCPReportDescriptor);

MPCPReportDescriptor::MPCPReportDescriptor() : cClassDescriptor("MPCPReport", "MPCP")
{
}

MPCPReportDescriptor::~MPCPReportDescriptor()
{
}

bool MPCPReportDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<MPCPReport *>(obj)!=NULL;
}

const char *MPCPReportDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int MPCPReportDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount(object) : 1;
}

unsigned int MPCPReportDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *MPCPReportDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "lengthSlot",
    };
    return (field>=0 && field<1) ? fieldNames[field] : NULL;
}

int MPCPReportDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='l' && strcmp(fieldName, "lengthSlot")==0) return base+0;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *MPCPReportDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "uint32_t",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : NULL;
}

const char *MPCPReportDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int MPCPReportDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    MPCPReport *pp = (MPCPReport *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string MPCPReportDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    MPCPReport *pp = (MPCPReport *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getLengthSlot());
        default: return "";
    }
}

bool MPCPReportDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    MPCPReport *pp = (MPCPReport *)object; (void)pp;
    switch (field) {
        case 0: pp->setLengthSlot(string2ulong(value)); return true;
        default: return false;
    }
}

const char *MPCPReportDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
    };
    return (field>=0 && field<1) ? fieldStructNames[field] : NULL;
}

void *MPCPReportDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    MPCPReport *pp = (MPCPReport *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

MPCPRegReq::MPCPRegReq() : MPCP()
{
    this->ptpNumReq_var = 0;
    this->wdmEnabled_var = 0;
}

MPCPRegReq::MPCPRegReq(const MPCPRegReq& other) : MPCP()
{
    operator=(other);
}

MPCPRegReq::~MPCPRegReq()
{
}

MPCPRegReq& MPCPRegReq::operator=(const MPCPRegReq& other)
{
    if (this==&other) return *this;
    MPCP::operator=(other);
    this->ptpNumReq_var = other.ptpNumReq_var;
    this->wdmEnabled_var = other.wdmEnabled_var;
    return *this;
}

void MPCPRegReq::parsimPack(cCommBuffer *b)
{
    doPacking(b,(MPCP&)*this);
    doPacking(b,this->ptpNumReq_var);
    doPacking(b,this->wdmEnabled_var);
}

void MPCPRegReq::parsimUnpack(cCommBuffer *b)
{
    doUnpacking(b,(MPCP&)*this);
    doUnpacking(b,this->ptpNumReq_var);
    doUnpacking(b,this->wdmEnabled_var);
}

uint8_t MPCPRegReq::getPtpNumReq() const
{
    return ptpNumReq_var;
}

void MPCPRegReq::setPtpNumReq(uint8_t ptpNumReq_var)
{
    this->ptpNumReq_var = ptpNumReq_var;
}

bool MPCPRegReq::getWdmEnabled() const
{
    return wdmEnabled_var;
}

void MPCPRegReq::setWdmEnabled(bool wdmEnabled_var)
{
    this->wdmEnabled_var = wdmEnabled_var;
}

class MPCPRegReqDescriptor : public cClassDescriptor
{
  public:
    MPCPRegReqDescriptor();
    virtual ~MPCPRegReqDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(MPCPRegReqDescriptor);

MPCPRegReqDescriptor::MPCPRegReqDescriptor() : cClassDescriptor("MPCPRegReq", "MPCP")
{
}

MPCPRegReqDescriptor::~MPCPRegReqDescriptor()
{
}

bool MPCPRegReqDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<MPCPRegReq *>(obj)!=NULL;
}

const char *MPCPRegReqDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int MPCPRegReqDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount(object) : 2;
}

unsigned int MPCPRegReqDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *MPCPRegReqDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "ptpNumReq",
        "wdmEnabled",
    };
    return (field>=0 && field<2) ? fieldNames[field] : NULL;
}

int MPCPRegReqDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='p' && strcmp(fieldName, "ptpNumReq")==0) return base+0;
    if (fieldName[0]=='w' && strcmp(fieldName, "wdmEnabled")==0) return base+1;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *MPCPRegReqDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "uint8_t",
        "bool",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : NULL;
}

const char *MPCPRegReqDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int MPCPRegReqDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    MPCPRegReq *pp = (MPCPRegReq *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string MPCPRegReqDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    MPCPRegReq *pp = (MPCPRegReq *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getPtpNumReq());
        case 1: return bool2string(pp->getWdmEnabled());
        default: return "";
    }
}

bool MPCPRegReqDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    MPCPRegReq *pp = (MPCPRegReq *)object; (void)pp;
    switch (field) {
        case 0: pp->setPtpNumReq(string2ulong(value)); return true;
        case 1: pp->setWdmEnabled(string2bool(value)); return true;
        default: return false;
    }
}

const char *MPCPRegReqDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
    };
    return (field>=0 && field<2) ? fieldStructNames[field] : NULL;
}

void *MPCPRegReqDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    MPCPRegReq *pp = (MPCPRegReq *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

MPCPRegister::MPCPRegister() : MPCP()
{
    this->ptpNumReg_var = 0;
    LLIDs_arraysize = 0;
    this->LLIDs_var = 0;
}

MPCPRegister::MPCPRegister(const MPCPRegister& other) : MPCP()
{
    LLIDs_arraysize = 0;
    this->LLIDs_var = 0;
    operator=(other);
}

MPCPRegister::~MPCPRegister()
{
    delete [] LLIDs_var;
}

MPCPRegister& MPCPRegister::operator=(const MPCPRegister& other)
{
    if (this==&other) return *this;
    MPCP::operator=(other);
    this->ptpNumReg_var = other.ptpNumReg_var;
    delete [] this->LLIDs_var;
    this->LLIDs_var = (other.LLIDs_arraysize==0) ? NULL : new uint16_t[other.LLIDs_arraysize];
    LLIDs_arraysize = other.LLIDs_arraysize;
    for (unsigned int i=0; i<LLIDs_arraysize; i++)
        this->LLIDs_var[i] = other.LLIDs_var[i];
    return *this;
}

void MPCPRegister::parsimPack(cCommBuffer *b)
{
    doPacking(b,(MPCP&)*this);
    doPacking(b,this->ptpNumReg_var);
    b->pack(LLIDs_arraysize);
    doPacking(b,this->LLIDs_var,LLIDs_arraysize);
}

void MPCPRegister::parsimUnpack(cCommBuffer *b)
{
    doUnpacking(b,(MPCP&)*this);
    doUnpacking(b,this->ptpNumReg_var);
    delete [] this->LLIDs_var;
    b->unpack(LLIDs_arraysize);
    if (LLIDs_arraysize==0) {
        this->LLIDs_var = 0;
    } else {
        this->LLIDs_var = new uint16_t[LLIDs_arraysize];
        doUnpacking(b,this->LLIDs_var,LLIDs_arraysize);
    }
}

uint8_t MPCPRegister::getPtpNumReg() const
{
    return ptpNumReg_var;
}

void MPCPRegister::setPtpNumReg(uint8_t ptpNumReg_var)
{
    this->ptpNumReg_var = ptpNumReg_var;
}

void MPCPRegister::setLLIDsArraySize(unsigned int size)
{
    uint16_t *LLIDs_var2 = (size==0) ? NULL : new uint16_t[size];
    unsigned int sz = LLIDs_arraysize < size ? LLIDs_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        LLIDs_var2[i] = this->LLIDs_var[i];
    for (unsigned int i=sz; i<size; i++)
        LLIDs_var2[i] = 0;
    LLIDs_arraysize = size;
    delete [] this->LLIDs_var;
    this->LLIDs_var = LLIDs_var2;
}

unsigned int MPCPRegister::getLLIDsArraySize() const
{
    return LLIDs_arraysize;
}

uint16_t MPCPRegister::getLLIDs(unsigned int k) const
{
    if (k>=LLIDs_arraysize) throw cRuntimeError("Array of size %d indexed by %d", LLIDs_arraysize, k);
    return LLIDs_var[k];
}

void MPCPRegister::setLLIDs(unsigned int k, uint16_t LLIDs_var)
{
    if (k>=LLIDs_arraysize) throw cRuntimeError("Array of size %d indexed by %d", LLIDs_arraysize, k);
    this->LLIDs_var[k]=LLIDs_var;
}

class MPCPRegisterDescriptor : public cClassDescriptor
{
  public:
    MPCPRegisterDescriptor();
    virtual ~MPCPRegisterDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(MPCPRegisterDescriptor);

MPCPRegisterDescriptor::MPCPRegisterDescriptor() : cClassDescriptor("MPCPRegister", "MPCP")
{
}

MPCPRegisterDescriptor::~MPCPRegisterDescriptor()
{
}

bool MPCPRegisterDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<MPCPRegister *>(obj)!=NULL;
}

const char *MPCPRegisterDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int MPCPRegisterDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount(object) : 2;
}

unsigned int MPCPRegisterDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *MPCPRegisterDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "ptpNumReg",
        "LLIDs",
    };
    return (field>=0 && field<2) ? fieldNames[field] : NULL;
}

int MPCPRegisterDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='p' && strcmp(fieldName, "ptpNumReg")==0) return base+0;
    if (fieldName[0]=='L' && strcmp(fieldName, "LLIDs")==0) return base+1;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *MPCPRegisterDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "uint8_t",
        "uint16_t",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : NULL;
}

const char *MPCPRegisterDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int MPCPRegisterDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    MPCPRegister *pp = (MPCPRegister *)object; (void)pp;
    switch (field) {
        case 1: return pp->getLLIDsArraySize();
        default: return 0;
    }
}

std::string MPCPRegisterDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    MPCPRegister *pp = (MPCPRegister *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getPtpNumReg());
        case 1: return ulong2string(pp->getLLIDs(i));
        default: return "";
    }
}

bool MPCPRegisterDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    MPCPRegister *pp = (MPCPRegister *)object; (void)pp;
    switch (field) {
        case 0: pp->setPtpNumReg(string2ulong(value)); return true;
        case 1: pp->setLLIDs(i,string2ulong(value)); return true;
        default: return false;
    }
}

const char *MPCPRegisterDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
    };
    return (field>=0 && field<2) ? fieldStructNames[field] : NULL;
}

void *MPCPRegisterDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    MPCPRegister *pp = (MPCPRegister *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

MPCPRegAck::MPCPRegAck() : MPCP()
{
}

MPCPRegAck::MPCPRegAck(const MPCPRegAck& other) : MPCP()
{
    operator=(other);
}

MPCPRegAck::~MPCPRegAck()
{
}

MPCPRegAck& MPCPRegAck::operator=(const MPCPRegAck& other)
{
    if (this==&other) return *this;
    MPCP::operator=(other);
    return *this;
}

void MPCPRegAck::parsimPack(cCommBuffer *b)
{
    doPacking(b,(MPCP&)*this);
}

void MPCPRegAck::parsimUnpack(cCommBuffer *b)
{
    doUnpacking(b,(MPCP&)*this);
}

class MPCPRegAckDescriptor : public cClassDescriptor
{
  public:
    MPCPRegAckDescriptor();
    virtual ~MPCPRegAckDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(MPCPRegAckDescriptor);

MPCPRegAckDescriptor::MPCPRegAckDescriptor() : cClassDescriptor("MPCPRegAck", "MPCP")
{
}

MPCPRegAckDescriptor::~MPCPRegAckDescriptor()
{
}

bool MPCPRegAckDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<MPCPRegAck *>(obj)!=NULL;
}

const char *MPCPRegAckDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int MPCPRegAckDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 0+basedesc->getFieldCount(object) : 0;
}

unsigned int MPCPRegAckDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    return 0;
}

const char *MPCPRegAckDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    return NULL;
}

int MPCPRegAckDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *MPCPRegAckDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    return NULL;
}

const char *MPCPRegAckDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int MPCPRegAckDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    MPCPRegAck *pp = (MPCPRegAck *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string MPCPRegAckDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    MPCPRegAck *pp = (MPCPRegAck *)object; (void)pp;
    switch (field) {
        default: return "";
    }
}

bool MPCPRegAckDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    MPCPRegAck *pp = (MPCPRegAck *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *MPCPRegAckDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    return NULL;
}

void *MPCPRegAckDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    MPCPRegAck *pp = (MPCPRegAck *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


