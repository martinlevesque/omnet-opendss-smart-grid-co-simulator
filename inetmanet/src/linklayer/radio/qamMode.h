/*
 * Copyright (c) 2007 INRIA
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
 * Authors:   Ahmed Ayadi < Ahmed.Ayadi@sophia.inria.fr>
 *            Masood Khosroshahy < m.khosroshahy@iee.org>
 *
 */

#ifndef QAM_MODE_H
#define QAM_MODE_H

#include "transmissionMode.h"

class NoFecQamMode : public NoFecTransmissionMode
{
  public:
    NoFecQamMode (double signal_spread, uint32_t rate, double cod_rate, unsigned int m);
    virtual ~NoFecQamMode ();
    double getChunkSuccessRate (double snr, unsigned int nbits,unsigned char *buffer, double bitrate) ;

    uint32_t getBitNumbersPerModulationSymbol (void) const;

    double calculateBER(double snir, double bandwidth, double bitrate) { return 0;};

  private:
    unsigned int m;
};

class FecQamMode : public FecTransmissionMode
{
  public:
    FecQamMode (double signalSpread, int32_t rate, double codingRate, unsigned int M, unsigned int dFree, unsigned int adFree, unsigned int adFreePlusOne);
    FecQamMode (double signal_spread,uint32_t rate,double coding_rate,unsigned int M);
    virtual ~FecQamMode ();

    double getChunkSuccessRate (double snr, unsigned int nbits,unsigned char *buffer, double bitrate);
    double calculateBER(double snir, double bandwidth, double bitrate) { return 0;};

    uint32_t getBitNumbersPerModulationSymbol (void) const;

  private:
    unsigned int m;
    unsigned int dFree;
    unsigned int adFree;
    unsigned int adFreePlusOne;
};

#endif /* QAM_MODE_H */
