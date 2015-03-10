cplusplus {{
#include "IPvXAddress.h"
}}


class noncobject IPvXAddress;

enum DataType_t
{

    administration = 0; // Group and User Maintenance Packet
    sessionControl = 1; // Session Control Packet
    mm_Signaling = 2; // Multimedia Signaling Packet
    //----------------
    mm_Audio = 3; // Multimedia Audio Stream
    mm_Video = 4; // Multimedia Video Stream
    mm_Data = 5; // Multimedia Data Stream
    reserved = 6; // Packet is reserved for experimental 
    mm_mcAudio = 7;  // Multicast Multimedia Audio Stream
    no_Data = -1;
}

enum AdminType_t
{

    adminRegister = 0; // registers to a server
    adminUpdate = 1; // updates information like address an renews online status on server
    adminStatus = 2; // inform server about status changes
    adminJoinGroup = 3; // joins a group additional data says, if group should be created, if not available
    adminUserInfo = 4; // requests information about users
    adminSearchUsers = 5; // requests user names of a group (group * is all friends)
    adminInviteUsers = 6; // invites a user to a group (type of group must be specified)
    adminSyncGroups = 7; // user wants to resync to server information
    adminServerUserInfo = 8;
    no_admin = -1;
}

enum SessionType_t
{

    sessionOccupyRequest = 0;
    sessionOccupy = 1;
    sessionOccupyRelease = 2;
    sessionCentralOccupy = 3;
    sessionMultiplexGroup = 4;
    sessionAvalanceGroup = 5;
    sessionMulticastDiscovery = 6;
    sessionPing = 7;
    no_session = -1;
}


class AkiControlInfo
{
    int dataType enum(DataType_t) = -1;
    int adminType enum(AdminType_t) = -1;
    int sessionType enum(SessionType_t) = -1;
    bool ackType = false;
    IPvXAddress srcAddr;  // source \IP or \IPv6 address, or local address with BIND
    IPvXAddress destAddr; // destination \IP or \IPv6 address
    int srcPort;   // \UDP source port in packet, or local port with BIND
    int destPort;  // \UDP destination port in packet
	
	int sockId = -1;   // uniquely identifies the \UDP socket
    int userId = -1;   // id than can be freely used by the app
	
}

