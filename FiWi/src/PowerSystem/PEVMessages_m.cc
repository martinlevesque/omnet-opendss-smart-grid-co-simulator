//
// Generated file, do not edit! Created by opp_msgc 4.1 from PowerSystem/PEVMessages.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "PEVMessages_m.h"

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




Register_Class(PowerSysACKMessage);

PowerSysACKMessage::PowerSysACKMessage(const char *name, int kind) : cPacket(name,kind)
{
    this->type_var = 0;
}

PowerSysACKMessage::PowerSysACKMessage(const PowerSysACKMessage& other) : cPacket()
{
    setName(other.getName());
    operator=(other);
}

PowerSysACKMessage::~PowerSysACKMessage()
{
}

PowerSysACKMessage& PowerSysACKMessage::operator=(const PowerSysACKMessage& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->type_var = other.type_var;
    return *this;
}

void PowerSysACKMessage::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->type_var);
}

void PowerSysACKMessage::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->type_var);
}

const char * PowerSysACKMessage::getType() const
{
    return type_var.c_str();
}

void PowerSysACKMessage::setType(const char * type_var)
{
    this->type_var = type_var;
}

class PowerSysACKMessageDescriptor : public cClassDescriptor
{
  public:
    PowerSysACKMessageDescriptor();
    virtual ~PowerSysACKMessageDescriptor();

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

Register_ClassDescriptor(PowerSysACKMessageDescriptor);

PowerSysACKMessageDescriptor::PowerSysACKMessageDescriptor() : cClassDescriptor("PowerSysACKMessage", "cPacket")
{
}

PowerSysACKMessageDescriptor::~PowerSysACKMessageDescriptor()
{
}

bool PowerSysACKMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<PowerSysACKMessage *>(obj)!=NULL;
}

const char *PowerSysACKMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int PowerSysACKMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount(object) : 1;
}

unsigned int PowerSysACKMessageDescriptor::getFieldTypeFlags(void *object, int field) const
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

const char *PowerSysACKMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "type",
    };
    return (field>=0 && field<1) ? fieldNames[field] : NULL;
}

int PowerSysACKMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='t' && strcmp(fieldName, "type")==0) return base+0;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *PowerSysACKMessageDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "string",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : NULL;
}

const char *PowerSysACKMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int PowerSysACKMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    PowerSysACKMessage *pp = (PowerSysACKMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string PowerSysACKMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    PowerSysACKMessage *pp = (PowerSysACKMessage *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getType());
        default: return "";
    }
}

bool PowerSysACKMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    PowerSysACKMessage *pp = (PowerSysACKMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setType((value)); return true;
        default: return false;
    }
}

const char *PowerSysACKMessageDescriptor::getFieldStructName(void *object, int field) const
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

void *PowerSysACKMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    PowerSysACKMessage *pp = (PowerSysACKMessage *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(NodeNotificationMessage);

NodeNotificationMessage::NodeNotificationMessage(const char *name, int kind) : cPacket(name,kind)
{
    this->nodeId_var = 0;
    this->voltage_var = 0;
    this->load_var = 0;
}

NodeNotificationMessage::NodeNotificationMessage(const NodeNotificationMessage& other) : cPacket()
{
    setName(other.getName());
    operator=(other);
}

NodeNotificationMessage::~NodeNotificationMessage()
{
}

NodeNotificationMessage& NodeNotificationMessage::operator=(const NodeNotificationMessage& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->nodeId_var = other.nodeId_var;
    this->voltage_var = other.voltage_var;
    this->load_var = other.load_var;
    return *this;
}

void NodeNotificationMessage::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->nodeId_var);
    doPacking(b,this->voltage_var);
    doPacking(b,this->load_var);
}

void NodeNotificationMessage::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->nodeId_var);
    doUnpacking(b,this->voltage_var);
    doUnpacking(b,this->load_var);
}

const char * NodeNotificationMessage::getNodeId() const
{
    return nodeId_var.c_str();
}

void NodeNotificationMessage::setNodeId(const char * nodeId_var)
{
    this->nodeId_var = nodeId_var;
}

double NodeNotificationMessage::getVoltage() const
{
    return voltage_var;
}

void NodeNotificationMessage::setVoltage(double voltage_var)
{
    this->voltage_var = voltage_var;
}

double NodeNotificationMessage::getLoad() const
{
    return load_var;
}

void NodeNotificationMessage::setLoad(double load_var)
{
    this->load_var = load_var;
}

class NodeNotificationMessageDescriptor : public cClassDescriptor
{
  public:
    NodeNotificationMessageDescriptor();
    virtual ~NodeNotificationMessageDescriptor();

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

Register_ClassDescriptor(NodeNotificationMessageDescriptor);

NodeNotificationMessageDescriptor::NodeNotificationMessageDescriptor() : cClassDescriptor("NodeNotificationMessage", "cPacket")
{
}

NodeNotificationMessageDescriptor::~NodeNotificationMessageDescriptor()
{
}

bool NodeNotificationMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<NodeNotificationMessage *>(obj)!=NULL;
}

const char *NodeNotificationMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int NodeNotificationMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount(object) : 3;
}

unsigned int NodeNotificationMessageDescriptor::getFieldTypeFlags(void *object, int field) const
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

const char *NodeNotificationMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "nodeId",
        "voltage",
        "load",
    };
    return (field>=0 && field<3) ? fieldNames[field] : NULL;
}

int NodeNotificationMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='n' && strcmp(fieldName, "nodeId")==0) return base+0;
    if (fieldName[0]=='v' && strcmp(fieldName, "voltage")==0) return base+1;
    if (fieldName[0]=='l' && strcmp(fieldName, "load")==0) return base+2;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *NodeNotificationMessageDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "string",
        "double",
        "double",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : NULL;
}

const char *NodeNotificationMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int NodeNotificationMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    NodeNotificationMessage *pp = (NodeNotificationMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string NodeNotificationMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    NodeNotificationMessage *pp = (NodeNotificationMessage *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getNodeId());
        case 1: return double2string(pp->getVoltage());
        case 2: return double2string(pp->getLoad());
        default: return "";
    }
}

bool NodeNotificationMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    NodeNotificationMessage *pp = (NodeNotificationMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setNodeId((value)); return true;
        case 1: pp->setVoltage(string2double(value)); return true;
        case 2: pp->setLoad(string2double(value)); return true;
        default: return false;
    }
}

const char *NodeNotificationMessageDescriptor::getFieldStructName(void *object, int field) const
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

void *NodeNotificationMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    NodeNotificationMessage *pp = (NodeNotificationMessage *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(PEVAuthenticationMessage);

PEVAuthenticationMessage::PEVAuthenticationMessage(const char *name, int kind) : cPacket(name,kind)
{
    this->nodeId_var = 0;
    this->vehicleID_var = 0;
    this->customerID_var = 0;
    this->pinNumber_var = 0;
}

PEVAuthenticationMessage::PEVAuthenticationMessage(const PEVAuthenticationMessage& other) : cPacket()
{
    setName(other.getName());
    operator=(other);
}

PEVAuthenticationMessage::~PEVAuthenticationMessage()
{
}

PEVAuthenticationMessage& PEVAuthenticationMessage::operator=(const PEVAuthenticationMessage& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->nodeId_var = other.nodeId_var;
    this->vehicleID_var = other.vehicleID_var;
    this->customerID_var = other.customerID_var;
    this->pinNumber_var = other.pinNumber_var;
    return *this;
}

void PEVAuthenticationMessage::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->nodeId_var);
    doPacking(b,this->vehicleID_var);
    doPacking(b,this->customerID_var);
    doPacking(b,this->pinNumber_var);
}

void PEVAuthenticationMessage::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->nodeId_var);
    doUnpacking(b,this->vehicleID_var);
    doUnpacking(b,this->customerID_var);
    doUnpacking(b,this->pinNumber_var);
}

const char * PEVAuthenticationMessage::getNodeId() const
{
    return nodeId_var.c_str();
}

void PEVAuthenticationMessage::setNodeId(const char * nodeId_var)
{
    this->nodeId_var = nodeId_var;
}

const char * PEVAuthenticationMessage::getVehicleID() const
{
    return vehicleID_var.c_str();
}

void PEVAuthenticationMessage::setVehicleID(const char * vehicleID_var)
{
    this->vehicleID_var = vehicleID_var;
}

const char * PEVAuthenticationMessage::getCustomerID() const
{
    return customerID_var.c_str();
}

void PEVAuthenticationMessage::setCustomerID(const char * customerID_var)
{
    this->customerID_var = customerID_var;
}

const char * PEVAuthenticationMessage::getPinNumber() const
{
    return pinNumber_var.c_str();
}

void PEVAuthenticationMessage::setPinNumber(const char * pinNumber_var)
{
    this->pinNumber_var = pinNumber_var;
}

class PEVAuthenticationMessageDescriptor : public cClassDescriptor
{
  public:
    PEVAuthenticationMessageDescriptor();
    virtual ~PEVAuthenticationMessageDescriptor();

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

Register_ClassDescriptor(PEVAuthenticationMessageDescriptor);

PEVAuthenticationMessageDescriptor::PEVAuthenticationMessageDescriptor() : cClassDescriptor("PEVAuthenticationMessage", "cPacket")
{
}

PEVAuthenticationMessageDescriptor::~PEVAuthenticationMessageDescriptor()
{
}

bool PEVAuthenticationMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<PEVAuthenticationMessage *>(obj)!=NULL;
}

const char *PEVAuthenticationMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int PEVAuthenticationMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount(object) : 4;
}

unsigned int PEVAuthenticationMessageDescriptor::getFieldTypeFlags(void *object, int field) const
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
        FD_ISEDITABLE,
    };
    return (field>=0 && field<4) ? fieldTypeFlags[field] : 0;
}

const char *PEVAuthenticationMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "nodeId",
        "vehicleID",
        "customerID",
        "pinNumber",
    };
    return (field>=0 && field<4) ? fieldNames[field] : NULL;
}

int PEVAuthenticationMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='n' && strcmp(fieldName, "nodeId")==0) return base+0;
    if (fieldName[0]=='v' && strcmp(fieldName, "vehicleID")==0) return base+1;
    if (fieldName[0]=='c' && strcmp(fieldName, "customerID")==0) return base+2;
    if (fieldName[0]=='p' && strcmp(fieldName, "pinNumber")==0) return base+3;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *PEVAuthenticationMessageDescriptor::getFieldTypeString(void *object, int field) const
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
        "string",
        "string",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : NULL;
}

const char *PEVAuthenticationMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int PEVAuthenticationMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    PEVAuthenticationMessage *pp = (PEVAuthenticationMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string PEVAuthenticationMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    PEVAuthenticationMessage *pp = (PEVAuthenticationMessage *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getNodeId());
        case 1: return oppstring2string(pp->getVehicleID());
        case 2: return oppstring2string(pp->getCustomerID());
        case 3: return oppstring2string(pp->getPinNumber());
        default: return "";
    }
}

bool PEVAuthenticationMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    PEVAuthenticationMessage *pp = (PEVAuthenticationMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setNodeId((value)); return true;
        case 1: pp->setVehicleID((value)); return true;
        case 2: pp->setCustomerID((value)); return true;
        case 3: pp->setPinNumber((value)); return true;
        default: return false;
    }
}

const char *PEVAuthenticationMessageDescriptor::getFieldStructName(void *object, int field) const
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
    };
    return (field>=0 && field<4) ? fieldStructNames[field] : NULL;
}

void *PEVAuthenticationMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    PEVAuthenticationMessage *pp = (PEVAuthenticationMessage *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(PEVAuthenticationResponseMessage);

PEVAuthenticationResponseMessage::PEVAuthenticationResponseMessage(const char *name, int kind) : cPacket(name,kind)
{
    this->nodeId_var = 0;
    this->vehicleID_var = 0;
    this->customerID_var = 0;
}

PEVAuthenticationResponseMessage::PEVAuthenticationResponseMessage(const PEVAuthenticationResponseMessage& other) : cPacket()
{
    setName(other.getName());
    operator=(other);
}

PEVAuthenticationResponseMessage::~PEVAuthenticationResponseMessage()
{
}

PEVAuthenticationResponseMessage& PEVAuthenticationResponseMessage::operator=(const PEVAuthenticationResponseMessage& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->nodeId_var = other.nodeId_var;
    this->vehicleID_var = other.vehicleID_var;
    this->customerID_var = other.customerID_var;
    return *this;
}

void PEVAuthenticationResponseMessage::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->nodeId_var);
    doPacking(b,this->vehicleID_var);
    doPacking(b,this->customerID_var);
}

void PEVAuthenticationResponseMessage::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->nodeId_var);
    doUnpacking(b,this->vehicleID_var);
    doUnpacking(b,this->customerID_var);
}

const char * PEVAuthenticationResponseMessage::getNodeId() const
{
    return nodeId_var.c_str();
}

void PEVAuthenticationResponseMessage::setNodeId(const char * nodeId_var)
{
    this->nodeId_var = nodeId_var;
}

const char * PEVAuthenticationResponseMessage::getVehicleID() const
{
    return vehicleID_var.c_str();
}

void PEVAuthenticationResponseMessage::setVehicleID(const char * vehicleID_var)
{
    this->vehicleID_var = vehicleID_var;
}

const char * PEVAuthenticationResponseMessage::getCustomerID() const
{
    return customerID_var.c_str();
}

void PEVAuthenticationResponseMessage::setCustomerID(const char * customerID_var)
{
    this->customerID_var = customerID_var;
}

class PEVAuthenticationResponseMessageDescriptor : public cClassDescriptor
{
  public:
    PEVAuthenticationResponseMessageDescriptor();
    virtual ~PEVAuthenticationResponseMessageDescriptor();

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

Register_ClassDescriptor(PEVAuthenticationResponseMessageDescriptor);

PEVAuthenticationResponseMessageDescriptor::PEVAuthenticationResponseMessageDescriptor() : cClassDescriptor("PEVAuthenticationResponseMessage", "cPacket")
{
}

PEVAuthenticationResponseMessageDescriptor::~PEVAuthenticationResponseMessageDescriptor()
{
}

bool PEVAuthenticationResponseMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<PEVAuthenticationResponseMessage *>(obj)!=NULL;
}

const char *PEVAuthenticationResponseMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int PEVAuthenticationResponseMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount(object) : 3;
}

unsigned int PEVAuthenticationResponseMessageDescriptor::getFieldTypeFlags(void *object, int field) const
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

const char *PEVAuthenticationResponseMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "nodeId",
        "vehicleID",
        "customerID",
    };
    return (field>=0 && field<3) ? fieldNames[field] : NULL;
}

int PEVAuthenticationResponseMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='n' && strcmp(fieldName, "nodeId")==0) return base+0;
    if (fieldName[0]=='v' && strcmp(fieldName, "vehicleID")==0) return base+1;
    if (fieldName[0]=='c' && strcmp(fieldName, "customerID")==0) return base+2;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *PEVAuthenticationResponseMessageDescriptor::getFieldTypeString(void *object, int field) const
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
        "string",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : NULL;
}

const char *PEVAuthenticationResponseMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int PEVAuthenticationResponseMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    PEVAuthenticationResponseMessage *pp = (PEVAuthenticationResponseMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string PEVAuthenticationResponseMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    PEVAuthenticationResponseMessage *pp = (PEVAuthenticationResponseMessage *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getNodeId());
        case 1: return oppstring2string(pp->getVehicleID());
        case 2: return oppstring2string(pp->getCustomerID());
        default: return "";
    }
}

bool PEVAuthenticationResponseMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    PEVAuthenticationResponseMessage *pp = (PEVAuthenticationResponseMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setNodeId((value)); return true;
        case 1: pp->setVehicleID((value)); return true;
        case 2: pp->setCustomerID((value)); return true;
        default: return false;
    }
}

const char *PEVAuthenticationResponseMessageDescriptor::getFieldStructName(void *object, int field) const
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

void *PEVAuthenticationResponseMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    PEVAuthenticationResponseMessage *pp = (PEVAuthenticationResponseMessage *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(PEVControlMessage);

PEVControlMessage::PEVControlMessage(const char *name, int kind) : cPacket(name,kind)
{
    this->nodeId_var = 0;
    this->vehicleID_var = 0;
    this->customerID_var = 0;
    this->operation_var = 0;
}

PEVControlMessage::PEVControlMessage(const PEVControlMessage& other) : cPacket()
{
    setName(other.getName());
    operator=(other);
}

PEVControlMessage::~PEVControlMessage()
{
}

PEVControlMessage& PEVControlMessage::operator=(const PEVControlMessage& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->nodeId_var = other.nodeId_var;
    this->vehicleID_var = other.vehicleID_var;
    this->customerID_var = other.customerID_var;
    this->operation_var = other.operation_var;
    return *this;
}

void PEVControlMessage::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->nodeId_var);
    doPacking(b,this->vehicleID_var);
    doPacking(b,this->customerID_var);
    doPacking(b,this->operation_var);
}

void PEVControlMessage::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->nodeId_var);
    doUnpacking(b,this->vehicleID_var);
    doUnpacking(b,this->customerID_var);
    doUnpacking(b,this->operation_var);
}

const char * PEVControlMessage::getNodeId() const
{
    return nodeId_var.c_str();
}

void PEVControlMessage::setNodeId(const char * nodeId_var)
{
    this->nodeId_var = nodeId_var;
}

const char * PEVControlMessage::getVehicleID() const
{
    return vehicleID_var.c_str();
}

void PEVControlMessage::setVehicleID(const char * vehicleID_var)
{
    this->vehicleID_var = vehicleID_var;
}

const char * PEVControlMessage::getCustomerID() const
{
    return customerID_var.c_str();
}

void PEVControlMessage::setCustomerID(const char * customerID_var)
{
    this->customerID_var = customerID_var;
}

const char * PEVControlMessage::getOperation() const
{
    return operation_var.c_str();
}

void PEVControlMessage::setOperation(const char * operation_var)
{
    this->operation_var = operation_var;
}

class PEVControlMessageDescriptor : public cClassDescriptor
{
  public:
    PEVControlMessageDescriptor();
    virtual ~PEVControlMessageDescriptor();

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

Register_ClassDescriptor(PEVControlMessageDescriptor);

PEVControlMessageDescriptor::PEVControlMessageDescriptor() : cClassDescriptor("PEVControlMessage", "cPacket")
{
}

PEVControlMessageDescriptor::~PEVControlMessageDescriptor()
{
}

bool PEVControlMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<PEVControlMessage *>(obj)!=NULL;
}

const char *PEVControlMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int PEVControlMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount(object) : 4;
}

unsigned int PEVControlMessageDescriptor::getFieldTypeFlags(void *object, int field) const
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
        FD_ISEDITABLE,
    };
    return (field>=0 && field<4) ? fieldTypeFlags[field] : 0;
}

const char *PEVControlMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "nodeId",
        "vehicleID",
        "customerID",
        "operation",
    };
    return (field>=0 && field<4) ? fieldNames[field] : NULL;
}

int PEVControlMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='n' && strcmp(fieldName, "nodeId")==0) return base+0;
    if (fieldName[0]=='v' && strcmp(fieldName, "vehicleID")==0) return base+1;
    if (fieldName[0]=='c' && strcmp(fieldName, "customerID")==0) return base+2;
    if (fieldName[0]=='o' && strcmp(fieldName, "operation")==0) return base+3;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *PEVControlMessageDescriptor::getFieldTypeString(void *object, int field) const
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
        "string",
        "string",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : NULL;
}

const char *PEVControlMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int PEVControlMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    PEVControlMessage *pp = (PEVControlMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string PEVControlMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    PEVControlMessage *pp = (PEVControlMessage *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getNodeId());
        case 1: return oppstring2string(pp->getVehicleID());
        case 2: return oppstring2string(pp->getCustomerID());
        case 3: return oppstring2string(pp->getOperation());
        default: return "";
    }
}

bool PEVControlMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    PEVControlMessage *pp = (PEVControlMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setNodeId((value)); return true;
        case 1: pp->setVehicleID((value)); return true;
        case 2: pp->setCustomerID((value)); return true;
        case 3: pp->setOperation((value)); return true;
        default: return false;
    }
}

const char *PEVControlMessageDescriptor::getFieldStructName(void *object, int field) const
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
    };
    return (field>=0 && field<4) ? fieldStructNames[field] : NULL;
}

void *PEVControlMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    PEVControlMessage *pp = (PEVControlMessage *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(SubstationNotificationMessage);

SubstationNotificationMessage::SubstationNotificationMessage(const char *name, int kind) : cPacket(name,kind)
{
    this->nodeId_var = 0;
    this->totalPowerLoad_var = 0;
    this->totalPowerLosses_var = 0;
}

SubstationNotificationMessage::SubstationNotificationMessage(const SubstationNotificationMessage& other) : cPacket()
{
    setName(other.getName());
    operator=(other);
}

SubstationNotificationMessage::~SubstationNotificationMessage()
{
}

SubstationNotificationMessage& SubstationNotificationMessage::operator=(const SubstationNotificationMessage& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->nodeId_var = other.nodeId_var;
    this->totalPowerLoad_var = other.totalPowerLoad_var;
    this->totalPowerLosses_var = other.totalPowerLosses_var;
    return *this;
}

void SubstationNotificationMessage::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->nodeId_var);
    doPacking(b,this->totalPowerLoad_var);
    doPacking(b,this->totalPowerLosses_var);
}

void SubstationNotificationMessage::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->nodeId_var);
    doUnpacking(b,this->totalPowerLoad_var);
    doUnpacking(b,this->totalPowerLosses_var);
}

const char * SubstationNotificationMessage::getNodeId() const
{
    return nodeId_var.c_str();
}

void SubstationNotificationMessage::setNodeId(const char * nodeId_var)
{
    this->nodeId_var = nodeId_var;
}

double SubstationNotificationMessage::getTotalPowerLoad() const
{
    return totalPowerLoad_var;
}

void SubstationNotificationMessage::setTotalPowerLoad(double totalPowerLoad_var)
{
    this->totalPowerLoad_var = totalPowerLoad_var;
}

double SubstationNotificationMessage::getTotalPowerLosses() const
{
    return totalPowerLosses_var;
}

void SubstationNotificationMessage::setTotalPowerLosses(double totalPowerLosses_var)
{
    this->totalPowerLosses_var = totalPowerLosses_var;
}

class SubstationNotificationMessageDescriptor : public cClassDescriptor
{
  public:
    SubstationNotificationMessageDescriptor();
    virtual ~SubstationNotificationMessageDescriptor();

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

Register_ClassDescriptor(SubstationNotificationMessageDescriptor);

SubstationNotificationMessageDescriptor::SubstationNotificationMessageDescriptor() : cClassDescriptor("SubstationNotificationMessage", "cPacket")
{
}

SubstationNotificationMessageDescriptor::~SubstationNotificationMessageDescriptor()
{
}

bool SubstationNotificationMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<SubstationNotificationMessage *>(obj)!=NULL;
}

const char *SubstationNotificationMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int SubstationNotificationMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount(object) : 3;
}

unsigned int SubstationNotificationMessageDescriptor::getFieldTypeFlags(void *object, int field) const
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

const char *SubstationNotificationMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "nodeId",
        "totalPowerLoad",
        "totalPowerLosses",
    };
    return (field>=0 && field<3) ? fieldNames[field] : NULL;
}

int SubstationNotificationMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='n' && strcmp(fieldName, "nodeId")==0) return base+0;
    if (fieldName[0]=='t' && strcmp(fieldName, "totalPowerLoad")==0) return base+1;
    if (fieldName[0]=='t' && strcmp(fieldName, "totalPowerLosses")==0) return base+2;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *SubstationNotificationMessageDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "string",
        "double",
        "double",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : NULL;
}

const char *SubstationNotificationMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int SubstationNotificationMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    SubstationNotificationMessage *pp = (SubstationNotificationMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string SubstationNotificationMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    SubstationNotificationMessage *pp = (SubstationNotificationMessage *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getNodeId());
        case 1: return double2string(pp->getTotalPowerLoad());
        case 2: return double2string(pp->getTotalPowerLosses());
        default: return "";
    }
}

bool SubstationNotificationMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    SubstationNotificationMessage *pp = (SubstationNotificationMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setNodeId((value)); return true;
        case 1: pp->setTotalPowerLoad(string2double(value)); return true;
        case 2: pp->setTotalPowerLosses(string2double(value)); return true;
        default: return false;
    }
}

const char *SubstationNotificationMessageDescriptor::getFieldStructName(void *object, int field) const
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

void *SubstationNotificationMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    SubstationNotificationMessage *pp = (SubstationNotificationMessage *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(ChargingDeadlineRequestMessage);

ChargingDeadlineRequestMessage::ChargingDeadlineRequestMessage(const char *name, int kind) : cPacket(name,kind)
{
    this->nodeId_var = 0;
    this->vehicleID_var = 0;
    this->customerID_var = 0;
    this->deadline_var = 0;
    this->batteryCapacity_var = 0;
    this->batteryKwPerHour_var = 0;
    this->batteryDepthOfDischarge_var = 0;
    this->batteryStateOfCharge_var = 0;
    this->baseLoad_var = 0;
    this->wantV2G_var = 0;
}

ChargingDeadlineRequestMessage::ChargingDeadlineRequestMessage(const ChargingDeadlineRequestMessage& other) : cPacket()
{
    setName(other.getName());
    operator=(other);
}

ChargingDeadlineRequestMessage::~ChargingDeadlineRequestMessage()
{
}

ChargingDeadlineRequestMessage& ChargingDeadlineRequestMessage::operator=(const ChargingDeadlineRequestMessage& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->nodeId_var = other.nodeId_var;
    this->vehicleID_var = other.vehicleID_var;
    this->customerID_var = other.customerID_var;
    this->deadline_var = other.deadline_var;
    this->batteryCapacity_var = other.batteryCapacity_var;
    this->batteryKwPerHour_var = other.batteryKwPerHour_var;
    this->batteryDepthOfDischarge_var = other.batteryDepthOfDischarge_var;
    this->batteryStateOfCharge_var = other.batteryStateOfCharge_var;
    this->baseLoad_var = other.baseLoad_var;
    this->wantV2G_var = other.wantV2G_var;
    return *this;
}

void ChargingDeadlineRequestMessage::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->nodeId_var);
    doPacking(b,this->vehicleID_var);
    doPacking(b,this->customerID_var);
    doPacking(b,this->deadline_var);
    doPacking(b,this->batteryCapacity_var);
    doPacking(b,this->batteryKwPerHour_var);
    doPacking(b,this->batteryDepthOfDischarge_var);
    doPacking(b,this->batteryStateOfCharge_var);
    doPacking(b,this->baseLoad_var);
    doPacking(b,this->wantV2G_var);
}

void ChargingDeadlineRequestMessage::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->nodeId_var);
    doUnpacking(b,this->vehicleID_var);
    doUnpacking(b,this->customerID_var);
    doUnpacking(b,this->deadline_var);
    doUnpacking(b,this->batteryCapacity_var);
    doUnpacking(b,this->batteryKwPerHour_var);
    doUnpacking(b,this->batteryDepthOfDischarge_var);
    doUnpacking(b,this->batteryStateOfCharge_var);
    doUnpacking(b,this->baseLoad_var);
    doUnpacking(b,this->wantV2G_var);
}

const char * ChargingDeadlineRequestMessage::getNodeId() const
{
    return nodeId_var.c_str();
}

void ChargingDeadlineRequestMessage::setNodeId(const char * nodeId_var)
{
    this->nodeId_var = nodeId_var;
}

const char * ChargingDeadlineRequestMessage::getVehicleID() const
{
    return vehicleID_var.c_str();
}

void ChargingDeadlineRequestMessage::setVehicleID(const char * vehicleID_var)
{
    this->vehicleID_var = vehicleID_var;
}

const char * ChargingDeadlineRequestMessage::getCustomerID() const
{
    return customerID_var.c_str();
}

void ChargingDeadlineRequestMessage::setCustomerID(const char * customerID_var)
{
    this->customerID_var = customerID_var;
}

double ChargingDeadlineRequestMessage::getDeadline() const
{
    return deadline_var;
}

void ChargingDeadlineRequestMessage::setDeadline(double deadline_var)
{
    this->deadline_var = deadline_var;
}

double ChargingDeadlineRequestMessage::getBatteryCapacity() const
{
    return batteryCapacity_var;
}

void ChargingDeadlineRequestMessage::setBatteryCapacity(double batteryCapacity_var)
{
    this->batteryCapacity_var = batteryCapacity_var;
}

double ChargingDeadlineRequestMessage::getBatteryKwPerHour() const
{
    return batteryKwPerHour_var;
}

void ChargingDeadlineRequestMessage::setBatteryKwPerHour(double batteryKwPerHour_var)
{
    this->batteryKwPerHour_var = batteryKwPerHour_var;
}

double ChargingDeadlineRequestMessage::getBatteryDepthOfDischarge() const
{
    return batteryDepthOfDischarge_var;
}

void ChargingDeadlineRequestMessage::setBatteryDepthOfDischarge(double batteryDepthOfDischarge_var)
{
    this->batteryDepthOfDischarge_var = batteryDepthOfDischarge_var;
}

double ChargingDeadlineRequestMessage::getBatteryStateOfCharge() const
{
    return batteryStateOfCharge_var;
}

void ChargingDeadlineRequestMessage::setBatteryStateOfCharge(double batteryStateOfCharge_var)
{
    this->batteryStateOfCharge_var = batteryStateOfCharge_var;
}

double ChargingDeadlineRequestMessage::getBaseLoad() const
{
    return baseLoad_var;
}

void ChargingDeadlineRequestMessage::setBaseLoad(double baseLoad_var)
{
    this->baseLoad_var = baseLoad_var;
}

bool ChargingDeadlineRequestMessage::getWantV2G() const
{
    return wantV2G_var;
}

void ChargingDeadlineRequestMessage::setWantV2G(bool wantV2G_var)
{
    this->wantV2G_var = wantV2G_var;
}

class ChargingDeadlineRequestMessageDescriptor : public cClassDescriptor
{
  public:
    ChargingDeadlineRequestMessageDescriptor();
    virtual ~ChargingDeadlineRequestMessageDescriptor();

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

Register_ClassDescriptor(ChargingDeadlineRequestMessageDescriptor);

ChargingDeadlineRequestMessageDescriptor::ChargingDeadlineRequestMessageDescriptor() : cClassDescriptor("ChargingDeadlineRequestMessage", "cPacket")
{
}

ChargingDeadlineRequestMessageDescriptor::~ChargingDeadlineRequestMessageDescriptor()
{
}

bool ChargingDeadlineRequestMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<ChargingDeadlineRequestMessage *>(obj)!=NULL;
}

const char *ChargingDeadlineRequestMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int ChargingDeadlineRequestMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 10+basedesc->getFieldCount(object) : 10;
}

unsigned int ChargingDeadlineRequestMessageDescriptor::getFieldTypeFlags(void *object, int field) const
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
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<10) ? fieldTypeFlags[field] : 0;
}

const char *ChargingDeadlineRequestMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "nodeId",
        "vehicleID",
        "customerID",
        "deadline",
        "batteryCapacity",
        "batteryKwPerHour",
        "batteryDepthOfDischarge",
        "batteryStateOfCharge",
        "baseLoad",
        "wantV2G",
    };
    return (field>=0 && field<10) ? fieldNames[field] : NULL;
}

int ChargingDeadlineRequestMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='n' && strcmp(fieldName, "nodeId")==0) return base+0;
    if (fieldName[0]=='v' && strcmp(fieldName, "vehicleID")==0) return base+1;
    if (fieldName[0]=='c' && strcmp(fieldName, "customerID")==0) return base+2;
    if (fieldName[0]=='d' && strcmp(fieldName, "deadline")==0) return base+3;
    if (fieldName[0]=='b' && strcmp(fieldName, "batteryCapacity")==0) return base+4;
    if (fieldName[0]=='b' && strcmp(fieldName, "batteryKwPerHour")==0) return base+5;
    if (fieldName[0]=='b' && strcmp(fieldName, "batteryDepthOfDischarge")==0) return base+6;
    if (fieldName[0]=='b' && strcmp(fieldName, "batteryStateOfCharge")==0) return base+7;
    if (fieldName[0]=='b' && strcmp(fieldName, "baseLoad")==0) return base+8;
    if (fieldName[0]=='w' && strcmp(fieldName, "wantV2G")==0) return base+9;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *ChargingDeadlineRequestMessageDescriptor::getFieldTypeString(void *object, int field) const
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
        "string",
        "double",
        "double",
        "double",
        "double",
        "double",
        "double",
        "bool",
    };
    return (field>=0 && field<10) ? fieldTypeStrings[field] : NULL;
}

const char *ChargingDeadlineRequestMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int ChargingDeadlineRequestMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    ChargingDeadlineRequestMessage *pp = (ChargingDeadlineRequestMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ChargingDeadlineRequestMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    ChargingDeadlineRequestMessage *pp = (ChargingDeadlineRequestMessage *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getNodeId());
        case 1: return oppstring2string(pp->getVehicleID());
        case 2: return oppstring2string(pp->getCustomerID());
        case 3: return double2string(pp->getDeadline());
        case 4: return double2string(pp->getBatteryCapacity());
        case 5: return double2string(pp->getBatteryKwPerHour());
        case 6: return double2string(pp->getBatteryDepthOfDischarge());
        case 7: return double2string(pp->getBatteryStateOfCharge());
        case 8: return double2string(pp->getBaseLoad());
        case 9: return bool2string(pp->getWantV2G());
        default: return "";
    }
}

bool ChargingDeadlineRequestMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    ChargingDeadlineRequestMessage *pp = (ChargingDeadlineRequestMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setNodeId((value)); return true;
        case 1: pp->setVehicleID((value)); return true;
        case 2: pp->setCustomerID((value)); return true;
        case 3: pp->setDeadline(string2double(value)); return true;
        case 4: pp->setBatteryCapacity(string2double(value)); return true;
        case 5: pp->setBatteryKwPerHour(string2double(value)); return true;
        case 6: pp->setBatteryDepthOfDischarge(string2double(value)); return true;
        case 7: pp->setBatteryStateOfCharge(string2double(value)); return true;
        case 8: pp->setBaseLoad(string2double(value)); return true;
        case 9: pp->setWantV2G(string2bool(value)); return true;
        default: return false;
    }
}

const char *ChargingDeadlineRequestMessageDescriptor::getFieldStructName(void *object, int field) const
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
        NULL,
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<10) ? fieldStructNames[field] : NULL;
}

void *ChargingDeadlineRequestMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    ChargingDeadlineRequestMessage *pp = (ChargingDeadlineRequestMessage *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(StateOfChargeMessage);

StateOfChargeMessage::StateOfChargeMessage(const char *name, int kind) : cPacket(name,kind)
{
    this->nodeId_var = 0;
    this->vehicleID_var = 0;
    this->customerID_var = 0;
    this->batteryCapacity_var = 0;
    this->batteryKwPerHour_var = 0;
    this->batteryDepthOfDischarge_var = 0;
    this->batteryStateOfCharge_var = 0;
}

StateOfChargeMessage::StateOfChargeMessage(const StateOfChargeMessage& other) : cPacket()
{
    setName(other.getName());
    operator=(other);
}

StateOfChargeMessage::~StateOfChargeMessage()
{
}

StateOfChargeMessage& StateOfChargeMessage::operator=(const StateOfChargeMessage& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->nodeId_var = other.nodeId_var;
    this->vehicleID_var = other.vehicleID_var;
    this->customerID_var = other.customerID_var;
    this->batteryCapacity_var = other.batteryCapacity_var;
    this->batteryKwPerHour_var = other.batteryKwPerHour_var;
    this->batteryDepthOfDischarge_var = other.batteryDepthOfDischarge_var;
    this->batteryStateOfCharge_var = other.batteryStateOfCharge_var;
    return *this;
}

void StateOfChargeMessage::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->nodeId_var);
    doPacking(b,this->vehicleID_var);
    doPacking(b,this->customerID_var);
    doPacking(b,this->batteryCapacity_var);
    doPacking(b,this->batteryKwPerHour_var);
    doPacking(b,this->batteryDepthOfDischarge_var);
    doPacking(b,this->batteryStateOfCharge_var);
}

void StateOfChargeMessage::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->nodeId_var);
    doUnpacking(b,this->vehicleID_var);
    doUnpacking(b,this->customerID_var);
    doUnpacking(b,this->batteryCapacity_var);
    doUnpacking(b,this->batteryKwPerHour_var);
    doUnpacking(b,this->batteryDepthOfDischarge_var);
    doUnpacking(b,this->batteryStateOfCharge_var);
}

const char * StateOfChargeMessage::getNodeId() const
{
    return nodeId_var.c_str();
}

void StateOfChargeMessage::setNodeId(const char * nodeId_var)
{
    this->nodeId_var = nodeId_var;
}

const char * StateOfChargeMessage::getVehicleID() const
{
    return vehicleID_var.c_str();
}

void StateOfChargeMessage::setVehicleID(const char * vehicleID_var)
{
    this->vehicleID_var = vehicleID_var;
}

const char * StateOfChargeMessage::getCustomerID() const
{
    return customerID_var.c_str();
}

void StateOfChargeMessage::setCustomerID(const char * customerID_var)
{
    this->customerID_var = customerID_var;
}

double StateOfChargeMessage::getBatteryCapacity() const
{
    return batteryCapacity_var;
}

void StateOfChargeMessage::setBatteryCapacity(double batteryCapacity_var)
{
    this->batteryCapacity_var = batteryCapacity_var;
}

double StateOfChargeMessage::getBatteryKwPerHour() const
{
    return batteryKwPerHour_var;
}

void StateOfChargeMessage::setBatteryKwPerHour(double batteryKwPerHour_var)
{
    this->batteryKwPerHour_var = batteryKwPerHour_var;
}

double StateOfChargeMessage::getBatteryDepthOfDischarge() const
{
    return batteryDepthOfDischarge_var;
}

void StateOfChargeMessage::setBatteryDepthOfDischarge(double batteryDepthOfDischarge_var)
{
    this->batteryDepthOfDischarge_var = batteryDepthOfDischarge_var;
}

double StateOfChargeMessage::getBatteryStateOfCharge() const
{
    return batteryStateOfCharge_var;
}

void StateOfChargeMessage::setBatteryStateOfCharge(double batteryStateOfCharge_var)
{
    this->batteryStateOfCharge_var = batteryStateOfCharge_var;
}

class StateOfChargeMessageDescriptor : public cClassDescriptor
{
  public:
    StateOfChargeMessageDescriptor();
    virtual ~StateOfChargeMessageDescriptor();

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

Register_ClassDescriptor(StateOfChargeMessageDescriptor);

StateOfChargeMessageDescriptor::StateOfChargeMessageDescriptor() : cClassDescriptor("StateOfChargeMessage", "cPacket")
{
}

StateOfChargeMessageDescriptor::~StateOfChargeMessageDescriptor()
{
}

bool StateOfChargeMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<StateOfChargeMessage *>(obj)!=NULL;
}

const char *StateOfChargeMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int StateOfChargeMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 7+basedesc->getFieldCount(object) : 7;
}

unsigned int StateOfChargeMessageDescriptor::getFieldTypeFlags(void *object, int field) const
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
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<7) ? fieldTypeFlags[field] : 0;
}

const char *StateOfChargeMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "nodeId",
        "vehicleID",
        "customerID",
        "batteryCapacity",
        "batteryKwPerHour",
        "batteryDepthOfDischarge",
        "batteryStateOfCharge",
    };
    return (field>=0 && field<7) ? fieldNames[field] : NULL;
}

int StateOfChargeMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='n' && strcmp(fieldName, "nodeId")==0) return base+0;
    if (fieldName[0]=='v' && strcmp(fieldName, "vehicleID")==0) return base+1;
    if (fieldName[0]=='c' && strcmp(fieldName, "customerID")==0) return base+2;
    if (fieldName[0]=='b' && strcmp(fieldName, "batteryCapacity")==0) return base+3;
    if (fieldName[0]=='b' && strcmp(fieldName, "batteryKwPerHour")==0) return base+4;
    if (fieldName[0]=='b' && strcmp(fieldName, "batteryDepthOfDischarge")==0) return base+5;
    if (fieldName[0]=='b' && strcmp(fieldName, "batteryStateOfCharge")==0) return base+6;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *StateOfChargeMessageDescriptor::getFieldTypeString(void *object, int field) const
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
        "string",
        "double",
        "double",
        "double",
        "double",
    };
    return (field>=0 && field<7) ? fieldTypeStrings[field] : NULL;
}

const char *StateOfChargeMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int StateOfChargeMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    StateOfChargeMessage *pp = (StateOfChargeMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string StateOfChargeMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    StateOfChargeMessage *pp = (StateOfChargeMessage *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getNodeId());
        case 1: return oppstring2string(pp->getVehicleID());
        case 2: return oppstring2string(pp->getCustomerID());
        case 3: return double2string(pp->getBatteryCapacity());
        case 4: return double2string(pp->getBatteryKwPerHour());
        case 5: return double2string(pp->getBatteryDepthOfDischarge());
        case 6: return double2string(pp->getBatteryStateOfCharge());
        default: return "";
    }
}

bool StateOfChargeMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    StateOfChargeMessage *pp = (StateOfChargeMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setNodeId((value)); return true;
        case 1: pp->setVehicleID((value)); return true;
        case 2: pp->setCustomerID((value)); return true;
        case 3: pp->setBatteryCapacity(string2double(value)); return true;
        case 4: pp->setBatteryKwPerHour(string2double(value)); return true;
        case 5: pp->setBatteryDepthOfDischarge(string2double(value)); return true;
        case 6: pp->setBatteryStateOfCharge(string2double(value)); return true;
        default: return false;
    }
}

const char *StateOfChargeMessageDescriptor::getFieldStructName(void *object, int field) const
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
        NULL,
    };
    return (field>=0 && field<7) ? fieldStructNames[field] : NULL;
}

void *StateOfChargeMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    StateOfChargeMessage *pp = (StateOfChargeMessage *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(ChargingDeadlineResponseRow);

ChargingDeadlineResponseRow::ChargingDeadlineResponseRow(const char *name, int kind) : cPacket(name,kind)
{
    this->status_var = 0;
    this->startTime_var = 0;
    this->endTime_var = 0;
    this->type_var = 0;
}

ChargingDeadlineResponseRow::ChargingDeadlineResponseRow(const ChargingDeadlineResponseRow& other) : cPacket()
{
    setName(other.getName());
    operator=(other);
}

ChargingDeadlineResponseRow::~ChargingDeadlineResponseRow()
{
}

ChargingDeadlineResponseRow& ChargingDeadlineResponseRow::operator=(const ChargingDeadlineResponseRow& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->status_var = other.status_var;
    this->startTime_var = other.startTime_var;
    this->endTime_var = other.endTime_var;
    this->type_var = other.type_var;
    return *this;
}

void ChargingDeadlineResponseRow::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->status_var);
    doPacking(b,this->startTime_var);
    doPacking(b,this->endTime_var);
    doPacking(b,this->type_var);
}

void ChargingDeadlineResponseRow::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->status_var);
    doUnpacking(b,this->startTime_var);
    doUnpacking(b,this->endTime_var);
    doUnpacking(b,this->type_var);
}

int ChargingDeadlineResponseRow::getStatus() const
{
    return status_var;
}

void ChargingDeadlineResponseRow::setStatus(int status_var)
{
    this->status_var = status_var;
}

double ChargingDeadlineResponseRow::getStartTime() const
{
    return startTime_var;
}

void ChargingDeadlineResponseRow::setStartTime(double startTime_var)
{
    this->startTime_var = startTime_var;
}

double ChargingDeadlineResponseRow::getEndTime() const
{
    return endTime_var;
}

void ChargingDeadlineResponseRow::setEndTime(double endTime_var)
{
    this->endTime_var = endTime_var;
}

const char * ChargingDeadlineResponseRow::getType() const
{
    return type_var.c_str();
}

void ChargingDeadlineResponseRow::setType(const char * type_var)
{
    this->type_var = type_var;
}

class ChargingDeadlineResponseRowDescriptor : public cClassDescriptor
{
  public:
    ChargingDeadlineResponseRowDescriptor();
    virtual ~ChargingDeadlineResponseRowDescriptor();

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

Register_ClassDescriptor(ChargingDeadlineResponseRowDescriptor);

ChargingDeadlineResponseRowDescriptor::ChargingDeadlineResponseRowDescriptor() : cClassDescriptor("ChargingDeadlineResponseRow", "cPacket")
{
}

ChargingDeadlineResponseRowDescriptor::~ChargingDeadlineResponseRowDescriptor()
{
}

bool ChargingDeadlineResponseRowDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<ChargingDeadlineResponseRow *>(obj)!=NULL;
}

const char *ChargingDeadlineResponseRowDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int ChargingDeadlineResponseRowDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount(object) : 4;
}

unsigned int ChargingDeadlineResponseRowDescriptor::getFieldTypeFlags(void *object, int field) const
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
        FD_ISEDITABLE,
    };
    return (field>=0 && field<4) ? fieldTypeFlags[field] : 0;
}

const char *ChargingDeadlineResponseRowDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "status",
        "startTime",
        "endTime",
        "type",
    };
    return (field>=0 && field<4) ? fieldNames[field] : NULL;
}

int ChargingDeadlineResponseRowDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "status")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "startTime")==0) return base+1;
    if (fieldName[0]=='e' && strcmp(fieldName, "endTime")==0) return base+2;
    if (fieldName[0]=='t' && strcmp(fieldName, "type")==0) return base+3;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *ChargingDeadlineResponseRowDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "double",
        "double",
        "string",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : NULL;
}

const char *ChargingDeadlineResponseRowDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int ChargingDeadlineResponseRowDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    ChargingDeadlineResponseRow *pp = (ChargingDeadlineResponseRow *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ChargingDeadlineResponseRowDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    ChargingDeadlineResponseRow *pp = (ChargingDeadlineResponseRow *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getStatus());
        case 1: return double2string(pp->getStartTime());
        case 2: return double2string(pp->getEndTime());
        case 3: return oppstring2string(pp->getType());
        default: return "";
    }
}

bool ChargingDeadlineResponseRowDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    ChargingDeadlineResponseRow *pp = (ChargingDeadlineResponseRow *)object; (void)pp;
    switch (field) {
        case 0: pp->setStatus(string2long(value)); return true;
        case 1: pp->setStartTime(string2double(value)); return true;
        case 2: pp->setEndTime(string2double(value)); return true;
        case 3: pp->setType((value)); return true;
        default: return false;
    }
}

const char *ChargingDeadlineResponseRowDescriptor::getFieldStructName(void *object, int field) const
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
    };
    return (field>=0 && field<4) ? fieldStructNames[field] : NULL;
}

void *ChargingDeadlineResponseRowDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    ChargingDeadlineResponseRow *pp = (ChargingDeadlineResponseRow *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(ChargingDeadlineResponseMessage);

ChargingDeadlineResponseMessage::ChargingDeadlineResponseMessage(const char *name, int kind) : cPacket(name,kind)
{
    this->nodeId_var = 0;
    this->vehicleID_var = 0;
    this->customerID_var = 0;
    results_arraysize = 0;
    this->results_var = 0;
}

ChargingDeadlineResponseMessage::ChargingDeadlineResponseMessage(const ChargingDeadlineResponseMessage& other) : cPacket()
{
    setName(other.getName());
    results_arraysize = 0;
    this->results_var = 0;
    operator=(other);
}

ChargingDeadlineResponseMessage::~ChargingDeadlineResponseMessage()
{
    for (unsigned int i=0; i<results_arraysize; i++)
        drop(&(this->results_var[i]));
    delete [] results_var;
}

ChargingDeadlineResponseMessage& ChargingDeadlineResponseMessage::operator=(const ChargingDeadlineResponseMessage& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->nodeId_var = other.nodeId_var;
    this->vehicleID_var = other.vehicleID_var;
    this->customerID_var = other.customerID_var;
    delete [] this->results_var;
    this->results_var = (other.results_arraysize==0) ? NULL : new ::ChargingDeadlineResponseRow[other.results_arraysize];
    results_arraysize = other.results_arraysize;
    for (unsigned int i=0; i<results_arraysize; i++)
    {
        take(&(this->results_var[i]));
        this->results_var[i] = other.results_var[i];
        this->results_var[i].setName(other.results_var[i].getName());
    }
    return *this;
}

void ChargingDeadlineResponseMessage::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->nodeId_var);
    doPacking(b,this->vehicleID_var);
    doPacking(b,this->customerID_var);
    b->pack(results_arraysize);
    doPacking(b,this->results_var,results_arraysize);
}

void ChargingDeadlineResponseMessage::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->nodeId_var);
    doUnpacking(b,this->vehicleID_var);
    doUnpacking(b,this->customerID_var);
    delete [] this->results_var;
    b->unpack(results_arraysize);
    if (results_arraysize==0) {
        this->results_var = 0;
    } else {
        this->results_var = new ::ChargingDeadlineResponseRow[results_arraysize];
        doUnpacking(b,this->results_var,results_arraysize);
    }
}

const char * ChargingDeadlineResponseMessage::getNodeId() const
{
    return nodeId_var.c_str();
}

void ChargingDeadlineResponseMessage::setNodeId(const char * nodeId_var)
{
    this->nodeId_var = nodeId_var;
}

const char * ChargingDeadlineResponseMessage::getVehicleID() const
{
    return vehicleID_var.c_str();
}

void ChargingDeadlineResponseMessage::setVehicleID(const char * vehicleID_var)
{
    this->vehicleID_var = vehicleID_var;
}

const char * ChargingDeadlineResponseMessage::getCustomerID() const
{
    return customerID_var.c_str();
}

void ChargingDeadlineResponseMessage::setCustomerID(const char * customerID_var)
{
    this->customerID_var = customerID_var;
}

void ChargingDeadlineResponseMessage::setResultsArraySize(unsigned int size)
{
    ::ChargingDeadlineResponseRow *results_var2 = (size==0) ? NULL : new ::ChargingDeadlineResponseRow[size];
    unsigned int sz = results_arraysize < size ? results_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        results_var2[i] = this->results_var[i];
    for (unsigned int i=sz; i<size; i++)
        take(&(results_var2[i]));
    results_arraysize = size;
    delete [] this->results_var;
    this->results_var = results_var2;
}

unsigned int ChargingDeadlineResponseMessage::getResultsArraySize() const
{
    return results_arraysize;
}

ChargingDeadlineResponseRow& ChargingDeadlineResponseMessage::getResults(unsigned int k)
{
    if (k>=results_arraysize) throw cRuntimeError("Array of size %d indexed by %d", results_arraysize, k);
    return results_var[k];
}

void ChargingDeadlineResponseMessage::setResults(unsigned int k, const ChargingDeadlineResponseRow& results_var)
{
    if (k>=results_arraysize) throw cRuntimeError("Array of size %d indexed by %d", results_arraysize, k);
    this->results_var[k]=results_var;
}

class ChargingDeadlineResponseMessageDescriptor : public cClassDescriptor
{
  public:
    ChargingDeadlineResponseMessageDescriptor();
    virtual ~ChargingDeadlineResponseMessageDescriptor();

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

Register_ClassDescriptor(ChargingDeadlineResponseMessageDescriptor);

ChargingDeadlineResponseMessageDescriptor::ChargingDeadlineResponseMessageDescriptor() : cClassDescriptor("ChargingDeadlineResponseMessage", "cPacket")
{
}

ChargingDeadlineResponseMessageDescriptor::~ChargingDeadlineResponseMessageDescriptor()
{
}

bool ChargingDeadlineResponseMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<ChargingDeadlineResponseMessage *>(obj)!=NULL;
}

const char *ChargingDeadlineResponseMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int ChargingDeadlineResponseMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount(object) : 4;
}

unsigned int ChargingDeadlineResponseMessageDescriptor::getFieldTypeFlags(void *object, int field) const
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
        FD_ISARRAY | FD_ISCOMPOUND | FD_ISCOBJECT | FD_ISCOWNEDOBJECT,
    };
    return (field>=0 && field<4) ? fieldTypeFlags[field] : 0;
}

const char *ChargingDeadlineResponseMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "nodeId",
        "vehicleID",
        "customerID",
        "results",
    };
    return (field>=0 && field<4) ? fieldNames[field] : NULL;
}

int ChargingDeadlineResponseMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='n' && strcmp(fieldName, "nodeId")==0) return base+0;
    if (fieldName[0]=='v' && strcmp(fieldName, "vehicleID")==0) return base+1;
    if (fieldName[0]=='c' && strcmp(fieldName, "customerID")==0) return base+2;
    if (fieldName[0]=='r' && strcmp(fieldName, "results")==0) return base+3;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *ChargingDeadlineResponseMessageDescriptor::getFieldTypeString(void *object, int field) const
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
        "string",
        "ChargingDeadlineResponseRow",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : NULL;
}

const char *ChargingDeadlineResponseMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int ChargingDeadlineResponseMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    ChargingDeadlineResponseMessage *pp = (ChargingDeadlineResponseMessage *)object; (void)pp;
    switch (field) {
        case 3: return pp->getResultsArraySize();
        default: return 0;
    }
}

std::string ChargingDeadlineResponseMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    ChargingDeadlineResponseMessage *pp = (ChargingDeadlineResponseMessage *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getNodeId());
        case 1: return oppstring2string(pp->getVehicleID());
        case 2: return oppstring2string(pp->getCustomerID());
        case 3: {std::stringstream out; out << pp->getResults(i); return out.str();}
        default: return "";
    }
}

bool ChargingDeadlineResponseMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    ChargingDeadlineResponseMessage *pp = (ChargingDeadlineResponseMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setNodeId((value)); return true;
        case 1: pp->setVehicleID((value)); return true;
        case 2: pp->setCustomerID((value)); return true;
        default: return false;
    }
}

const char *ChargingDeadlineResponseMessageDescriptor::getFieldStructName(void *object, int field) const
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
        "ChargingDeadlineResponseRow",
    };
    return (field>=0 && field<4) ? fieldStructNames[field] : NULL;
}

void *ChargingDeadlineResponseMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    ChargingDeadlineResponseMessage *pp = (ChargingDeadlineResponseMessage *)object; (void)pp;
    switch (field) {
        case 3: return (void *)static_cast<cObject *>(&pp->getResults(i)); break;
        default: return NULL;
    }
}


