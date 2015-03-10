/*
 * Copyright (c) ,2007 INRIA
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:   Ahmed Ayadi < Ahmed.Ayadi@ensi.rnu.tn>
 *            Masood Khosroshahy < m.khosroshahy@iee.org>
 */

#ifndef BPSK_MODE_H
#define BPSK_MODE_H

#include "transmissionMode.h"
#include "qamMode.h"

class NoFecBpskMode : public NoFecTransmissionMode
{
  public:
    NoFecBpskMode (double signal_spread, uint32_t rate, double cod_rate);
    virtual ~NoFecBpskMode ();
    double getChunkSuccessRate (double snr, unsigned int nbits, unsigned char *buffer, double bitrate) ;
    uint32_t getBitNumbersPerModulationSymbol (void) const;
    double calculateBER(double snir, double bandwidth, double bitrate) { return 0;};

  private:

};

class FecBpskMode : public FecTransmissionMode
{
  public:
    FecBpskMode (double signal_spread, uint32_t rate, double coding_rate, unsigned int d_free, unsigned int ad_free);
    FecBpskMode (double signal_spread, uint32_t rate, double coding_rate);
    virtual ~FecBpskMode ();

    double getChunkSuccessRate (double snr, unsigned int nbits, unsigned char *buffer, double bitrate);
    double calculateBER(double snir, double bandwidth, double bitrate) { return 0;};

    uint32_t getBitNumbersPerModulationSymbol (void) const;

  private:
    unsigned int dFree;
    unsigned int adFree;
};

#endif /* BPSK_MODE_H */
