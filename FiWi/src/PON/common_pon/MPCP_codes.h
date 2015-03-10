#ifndef MPCP_CODES
#define MPCP_CODES

#include <inttypes.h>

// MPCP TYPE AND CODES
const uint16_t MPCP_TYPE 	 = 		0x8808;

#define MPCP_GATE		2
#define MPCP_REPORT		3
#define MPCP_REGREQ		4
#define MPCP_REGISTER	5
#define MPCP_REGACK		6


#define MPCP_HEADER_LEN		6
#define MPCP_LLID_LEN		2
#define MPCP_TIMERS_LEN		8
#define MPCP_LIST_LEN		1
#define MPCP_ACK_LEN		1
#define MPCP_REPORT_LEN		2

#endif
