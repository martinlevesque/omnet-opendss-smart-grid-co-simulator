//
// Generated file, do not edit! Created by opp_msgc 4.1 from linklayer/EtherFrameVlans.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "EtherFrameVlans_m.h"

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




EthernetDot1QFrame::EthernetDot1QFrame() : EtherFrame()
{
    this->vlanType_var = 0;
    this->vlanID_var = 0;
    this->etherType_var = 0;
}

EthernetDot1QFrame::EthernetDot1QFrame(const EthernetDot1QFrame& other) : EtherFrame()
{
    operator=(other);
}

EthernetDot1QFrame::~EthernetDot1QFrame()
{
}

EthernetDot1QFrame& EthernetDot1QFrame::operator=(const EthernetDot1QFrame& other)
{
    if (this==&other) return *this;
    EtherFrame::operator=(other);
    this->vlanType_var = other.vlanType_var;
    this->vlanID_var = other.vlanID_var;
    this->etherType_var = other.etherType_var;
    return *this;
}

void EthernetDot1QFrame::parsimPack(cCommBuffer *b)
{
    doPacking(b,(EtherFrame&)*this);
    doPacking(b,this->vlanType_var);
    doPacking(b,this->vlanID_var);
    doPacking(b,this->etherType_var);
}

void EthernetDot1QFrame::parsimUnpack(cCommBuffer *b)
{
    doUnpacking(b,(EtherFrame&)*this);
    doUnpacking(b,this->vlanType_var);
    doUnpacking(b,this->vlanID_var);
    doUnpacking(b,this->etherType_var);
}

uint16_t EthernetDot1QFrame::getVlanType() const
{
    return vlanType_var;
}

void EthernetDot1QFrame::setVlanType(uint16_t vlanType_var)
{
    this->vlanType_var = vlanType_var;
}

uint16_t EthernetDot1QFrame::getVlanID() const
{
    return vlanID_var;
}

void EthernetDot1QFrame::setVlanID(uint16_t vlanID_var)
{
    this->vlanID_var = vlanID_var;
}

uint16_t EthernetDot1QFrame::getEtherType() const
{
    return etherType_var;
}

void EthernetDot1QFrame::setEtherType(uint16_t etherType_var)
{
    this->etherType_var = etherType_var;
}

class EthernetDot1QFrameDescriptor : public cClassDescriptor
{
  public:
    EthernetDot1QFrameDescriptor();
    virtual ~EthernetDot1QFrameDescriptor();

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

Register_ClassDescriptor(EthernetDot1QFrameDescriptor);

EthernetDot1QFrameDescriptor::EthernetDot1QFrameDescriptor() : cClassDescriptor("EthernetDot1QFrame", "EtherFrame")
{
}

EthernetDot1QFrameDescriptor::~EthernetDot1QFrameDescriptor()
{
}

bool EthernetDot1QFrameDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<EthernetDot1QFrame *>(obj)!=NULL;
}

const char *EthernetDot1QFrameDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int EthernetDot1QFrameDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount(object) : 3;
}

unsigned int EthernetDot1QFrameDescriptor::getFieldTypeFlags(void *object, int field) const
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
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *EthernetDot1QFrameDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "vlanType",
        "vlanID",
        "etherType",
    };
    return (field>=0 && field<3) ? fieldNames[field] : NULL;
}

int EthernetDot1QFrameDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='v' && strcmp(fieldName, "vlanType")==0) return base+0;
    if (fieldName[0]=='v' && strcmp(fieldName, "vlanID")==0) return base+1;
    if (fieldName[0]=='e' && strcmp(fieldName, "etherType")==0) return base+2;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *EthernetDot1QFrameDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "uint16_t",
        "uint16_t",
        "uint16_t",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : NULL;
}

const char *EthernetDot1QFrameDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int EthernetDot1QFrameDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    EthernetDot1QFrame *pp = (EthernetDot1QFrame *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string EthernetDot1QFrameDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    EthernetDot1QFrame *pp = (EthernetDot1QFrame *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getVlanType());
        case 1: return ulong2string(pp->getVlanID());
        case 2: return ulong2string(pp->getEtherType());
        default: return "";
    }
}

bool EthernetDot1QFrameDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    EthernetDot1QFrame *pp = (EthernetDot1QFrame *)object; (void)pp;
    switch (field) {
        case 0: pp->setVlanType(string2ulong(value)); return true;
        case 1: pp->setVlanID(string2ulong(value)); return true;
        case 2: pp->setEtherType(string2ulong(value)); return true;
        default: return false;
    }
}

const char *EthernetDot1QFrameDescriptor::getFieldStructName(void *object, int field) const
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
    };
    return (field>=0 && field<3) ? fieldStructNames[field] : NULL;
}

void *EthernetDot1QFrameDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    EthernetDot1QFrame *pp = (EthernetDot1QFrame *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


