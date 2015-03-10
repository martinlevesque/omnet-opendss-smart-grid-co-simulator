
#ifndef ETHER_VLAN_DEFS_H
#define ETHER_VLAN_DEFS_H

#include <inttypes.h>
#include "Ethernet.h"
#include "ethernet.h"

#define VLAN_TYPE_D1Q	0x8100

// 12(macs)+4(vlan)+2+4(FCS) + 1500 = 1522
#define MAX_ETHERNET_FRAME_D1Q 	(1518 + 4)
#define ETHERNET_HEADER_LEN_D1Q 22


#endif
