//
// Copyright (C) 2006 Andras Varga, Levente Meszaros and Ahmed Ayadi
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#ifndef IEEE80211A_CONSTS_H
#define IEEE80211A_CONSTS_H

#include "Ieee80211Consts.h"

const double RED_PHY_HEADER_DURATION = 0.000020;
/** @brief Preambule transmision delay */
const double PLCP_PREAMBLE_DELAY = 20E-6;
/** @brief Length of PLCP deader without SERVICE and Tail */
const int PLCP_HEADER_LENGTH = 4 + 1 + 12 + 1 + 6;
/** @brief SIGNAL delay */
const double PLCP_SIGNAL_DELAY = 4E-6;
/** @brief SYMBOL delay */
const double T_SYM = 4E-6;

// frame lengths in bits
// XXX this is duplicate, it's already in Ieee80211Frame.msg

/** @brief BasicBitrate with which the header is send */
const double HEADER_BITRATE=6E+6;
const int PHY_HEADER_LENGTH_A = 4 + 1 + 12 + 1 + 6 + 16 + 6;
const int PHY_HEADER_LENGTH_B = 192;
//const double PHY_HEADER_LENGTH_G = 26E-6;
const int HEADER_WITHOUT_PREAMBLE = 48;
const double BITRATE_HEADER = 1E+6;
const double BANDWIDTH = 2E+6;

// time slot ST, short interframe space SIFS, distributed interframe
// space DIFS, and extended interframe space EIFS
const_simtime_t ST = 9E-6;
const_simtime_t SIFS = 16E-6;
const_simtime_t DIFS = 2*ST + SIFS;
const_simtime_t EIFS = SIFS + DIFS + PLCP_PREAMBLE_DELAY + (PHY_HEADER_LENGTH_A + LENGTH_ACK)/HEADER_BITRATE;
const_simtime_t MAX_PROPAGATION_DELAY = 0.000003333001;  // 1 km at the speed of light

/** Minimum size (initial size) of contention window */
const int CW_MIN = 15;

/** Maximum size of contention window */
const int CW_MAX = 1023;




#endif

