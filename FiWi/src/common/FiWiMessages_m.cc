//
// Generated file, do not edit! Created by opp_msgc 4.1 from common/FiWiMessages.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "FiWiMessages_m.h"

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




Register_Class(FiWiMessage);

FiWiMessage::FiWiMessage(const char *name, int kind) : cPacket(name,kind)
{
    this->pktType_var = 0;
    this->host_var = 0;
    this->color_var = 0;
}

FiWiMessage::FiWiMessage(const FiWiMessage& other) : cPacket()
{
    setName(other.getName());
    operator=(other);
}

FiWiMessage::~FiWiMessage()
{
}

FiWiMessage& FiWiMessage::operator=(const FiWiMessage& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->pktType_var = other.pktType_var;
    this->host_var = other.host_var;
    this->color_var = other.color_var;
    return *this;
}

void FiWiMessage::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->pktType_var);
    doPacking(b,this->host_var);
    doPacking(b,this->color_var);
}

void FiWiMessage::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->pktType_var);
    doUnpacking(b,this->host_var);
    doUnpacking(b,this->color_var);
}

const char * FiWiMessage::getPktType() const
{
    return pktType_var.c_str();
}

void FiWiMessage::setPktType(const char * pktType_var)
{
    this->pktType_var = pktType_var;
}

const char * FiWiMessage::getHost() const
{
    return host_var.c_str();
}

void FiWiMessage::setHost(const char * host_var)
{
    this->host_var = host_var;
}

int FiWiMessage::getColor() const
{
    return color_var;
}

void FiWiMessage::setColor(int color_var)
{
    this->color_var = color_var;
}

class FiWiMessageDescriptor : public cClassDescriptor
{
  public:
    FiWiMessageDescriptor();
    virtual ~FiWiMessageDescriptor();

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

Register_ClassDescriptor(FiWiMessageDescriptor);

FiWiMessageDescriptor::FiWiMessageDescriptor() : cClassDescriptor("FiWiMessage", "cPacket")
{
}

FiWiMessageDescriptor::~FiWiMessageDescriptor()
{
}

bool FiWiMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<FiWiMessage *>(obj)!=NULL;
}

const char *FiWiMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int FiWiMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount(object) : 3;
}

unsigned int FiWiMessageDescriptor::getFieldTypeFlags(void *object, int field) const
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

const char *FiWiMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "pktType",
        "host",
        "color",
    };
    return (field>=0 && field<3) ? fieldNames[field] : NULL;
}

int FiWiMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='p' && strcmp(fieldName, "pktType")==0) return base+0;
    if (fieldName[0]=='h' && strcmp(fieldName, "host")==0) return base+1;
    if (fieldName[0]=='c' && strcmp(fieldName, "color")==0) return base+2;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *FiWiMessageDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "string",
        "string",
        "int",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : NULL;
}

const char *FiWiMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int FiWiMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    FiWiMessage *pp = (FiWiMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string FiWiMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    FiWiMessage *pp = (FiWiMessage *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getPktType());
        case 1: return oppstring2string(pp->getHost());
        case 2: return long2string(pp->getColor());
        default: return "";
    }
}

bool FiWiMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    FiWiMessage *pp = (FiWiMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setPktType((value)); return true;
        case 1: pp->setHost((value)); return true;
        case 2: pp->setColor(string2long(value)); return true;
        default: return false;
    }
}

const char *FiWiMessageDescriptor::getFieldStructName(void *object, int field) const
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

void *FiWiMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    FiWiMessage *pp = (FiWiMessage *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


