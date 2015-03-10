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
 * Authors:   Ahmed Ayadi < Ahmed.Ayadi@ensi.rnu.tn>
 *            Masood Khosroshahy < m.khosroshahy@iee.org>
 */


#include "qamMode.h"

#include <math.h>

NoFecQamMode::NoFecQamMode (double signalSpread, uint32_t rate, double cod_rate, unsigned int M)
        : NoFecTransmissionMode (signalSpread, rate, cod_rate),
        m (M)
{}

NoFecQamMode::~NoFecQamMode ()
{}

double NoFecQamMode::getChunkSuccessRate (double snr, unsigned int nbits,unsigned char *buffer, double bitrate)
{
    double csr;
    /**
     * 0: "[PER Calculation Method: Uniform Error Distribution]"
     */
    switch (PER_CALCULATION_METHOD)
    {
    case 0 :
    {
        double ber = getQamBer (snr, m);
        if (ber == 0)
        {
            return 1;
        }
        csr = pow ((double)1 - ber,(int)nbits);
    }
    break;

    default:
    {
        csr = 0;
    }
    }

    return csr;
}

uint32_t NoFecQamMode::getBitNumbersPerModulationSymbol (void) const
{
    // Ref. [Std00, Table.78]
    if (m == 4)
        return 2 ;
    else if (m == 16)
        return 4 ;
    else if (m == 64)
        return 6 ;
    else return 0; // i.e., there is a problem.
}

FecQamMode::FecQamMode (double signalSpread, int32_t rate, double codingRate, unsigned int M, unsigned int dFree, unsigned int adFree, unsigned int adFreePlusOne)
        : FecTransmissionMode (signalSpread, rate, codingRate),
        m (M), dFree (dFree),
        adFree (adFree),
        adFreePlusOne (adFreePlusOne)
{}

FecQamMode::FecQamMode (double signalSpread,uint32_t rate,double codingRate,unsigned int M)
        : FecTransmissionMode (signalSpread, rate, codingRate),
        m (M)
{

    if (codingRate == 0.5)
    {    // Ref. [FOO98, Table.A1]
        adFree = 11;
        coderOutputBits = 2;
        codingRate = 0.5 ;
        dFree = 10;
        puncturingPeriod = 1;

        Ck[0] = 36;
        Ck[1] = 0;
        Ck[2] = 211;
        Ck[3] = 0;
        Ck[4] = 1404;
        Ck[5] = 0;
        Ck[6] = 11633;
        Ck[7] = 0;
        Ck[8] = 77433;
        Ck[9] = 0;
    }
    else if (codingRate == 0.75)
    {   // Ref. [FOO98, Table.B.30]
        codingRate = 0.75 ;
        adFree = 8;
        coderOutputBits = 4;
        dFree = 5;
        puncturingPeriod = 3;

        Ck[0] = 42;
        Ck[1] = 201;
        Ck[2] = 1492;
        Ck[3] = 10469;
        Ck[4] = 62935;
        Ck[5] = 379546;
        Ck[6] = 2252394;
        Ck[7] = 13064540;
        Ck[8] = 75080308;
        Ck[9] = 427474864;
    }
    else if (codingRate == 0.666)
    {    // Ref. [FOO98, Table.B.29]
        codingRate = 0.666;
        adFree = 1;
        coderOutputBits = 3;
        dFree = 6;
        puncturingPeriod = 2;

        Ck[0] = 3;
        Ck[1] = 70;
        Ck[2] = 285;
        Ck[3] = 1276;
        Ck[4] = 6160;
        Ck[5] = 27128;
        Ck[6] = 117019;
        Ck[7] = 498835;
        Ck[8] = 2103480;
        Ck[9] = 8781268;
    }
    else EV << "dFree, puncturingPeriod and Ck values are not set properly in qam-mode.cc" << endl;
}

FecQamMode::~FecQamMode ()
{}

double FecQamMode::getChunkSuccessRate (double snr, unsigned int nbits, unsigned char *buffer, double bitrate)
{
    double csr, Pb;


    double ber = getQamBer (snr, m);
    EV<<"Bit Error Probability ( Instant Value ) = "<<ber<<endl;


    /**
     * 0: "[PER Calculation Method (Error Distribution at the Viterbi Decoder's Output: Uniform)]"
     * 1: "[PER Calculation Method (Error Distribution at the Viterbi Decoder's Output: Non-Uniform)]"
     */
    double EER = 1;

    switch (PER_CALCULATION_METHOD)
    {
    case 0 :
    {
        // Legacy code:
        /* first term */
        //double pd = calculate_pd (ber, m_dFree);
        //double pmu = m_adFree * pd;
        /* second term */
        //pd = calculate_pd (ber, m_dFree + 1);
        //pmu += m_adFree_plus_one * pd;
        //double pms = pow (1 - pmu, nbits);
        //csr = pms;

        Pb = calculatePb (ber, dFree, Ck, puncturingPeriod);
        if (Pb > ber)
            Pb = ber;
        //EV << "ber:" << ber << "Pb:" << Pb << endl;
        csr = pow ((double)(1 - Pb), (int)nbits);
        currentValues[0] = ber ;
        currentValues[1] =  Pb;
        currentValues[2] = nbits ;
        currentValues[3] =  csr;
        //EV << "snr:" << snr << " ber:" << ber << " Pb:" << Pb << " csr:" << csr << " nbits:" << nbits << endl;
        EV << "Packet Error Probability (Instant Value): " << 1-csr << endl;
        EV << "Current PHY Mode: " << bitrate/1000000 <<" Mb/s" <<endl;
        break;
    }
    break;

    case 1 : // New error distribution
    {
        Pb = calculatePb (ber, dFree, Ck, puncturingPeriod);
        if (Pb > ber)
            Pb = ber;

        EV<<"Bit Error Probability-After Decoder (Instant Value) = "<<Pb<<endl;

        // ATTENTION!
        // TEMP SOLUTION.
        // #############################################################
        // snr_moderated has better be replaced with snr.
        double snrModerated ;
        if ( snr < 70)
            snrModerated = snr;
        else snrModerated = 70;

        // Error Event Rate. Ref.[Kave Salamatian's Paper]
        // Between 9e155 and 8e155 for : Free space + no fading channel + BER(AWGN)
        // double EER_normalizing_factor = 9e155 ;
        // EER = adFree * pow( M_E , (codingRate * snr_moderated * dFree) )  / EER_normalizing_factor;

        // THESE TWO LINES MUST BE DELETED AFTER EER FORMULA IS CORRECTED.
        EER = 2 * Pb;
        snr = snrModerated ;
        // In TransmissionMode::generateErrorMasks(), "EER + 0.1" should be changed to "EER"
        // #############################################################

        // lambda = 1 / w  ,where w is the mean length of the errorless period
        // lambda: parameter of geometric distribution of errorless period length
        // lambda: success probability in geometric distribution
        // lambda = f (EER , memoryConstraintLength, coderOutputBits, snr, codingRate)
        // Ref.[Kave Salamatian's Paper]

        // v: memoryConstraintLength = 6 (number of shift registers in the encoder [Std00])
        int v = 6 ;
        double partA = 1/EER ;
        double partB = (v+1) + 1/( coderOutputBits*( snrModerated/2 - sqrt(2*snrModerated*codingRate) + codingRate ) ) ;
        double w = partA - partB ;

        if ( w < 1 )
            w = 1;
        double lambda = 1/w;

        // PER from Ref.[Kave Salamatian's Paper]
        csr = pow ( (double)(1 - lambda) , (int)nbits);

        EV << "Packet Error Probability (Instant Value): " << 1-csr << endl;
        EV << "Current PHY Mode: " <<bitrate/1000000<<" Mb/s"<<endl;

        currentValues[0] = ber ;
        currentValues[1] =  Pb;
        currentValues[2] = nbits ;
        currentValues[3] =  csr;
    }
    break;

    default:
    {
        csr = 0;
    }
    }


    return csr;
}

uint32_t FecQamMode::getBitNumbersPerModulationSymbol (void) const
{
    // Ref. [Std00, Table.78]
    if (m == 4)
        return 2 ;
    else if (m == 16)
        return 4 ;
    else if (m == 64)
        return 6 ;
    else return 0; // i.e., there is a problem.
}

