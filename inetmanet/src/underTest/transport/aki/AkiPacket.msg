enum DataType_t;

enum AdminType_t;

enum SessionType_t;


cplusplus {{
#include "AkiControlInfo_m.h"
}}

cplusplus {{
#include "INETDefs.h"
}}

packet AkiPacket
{
	int dataType enum(DataType_t);
    int adminType enum(AdminType_t);
    int sessionType enum(SessionType_t);
    bool ackType;
    //IPvXAddress srcAddr;
    //IPvXAddress destAddr;
    int srcPort;
    int destPort;
	int byteLength;
};
