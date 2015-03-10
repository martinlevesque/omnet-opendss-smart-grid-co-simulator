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


#include "transmissionMode.h"

#include <math.h>
#include <cassert>

TransmissionMode::~TransmissionMode ()
{}

static uint32_t errorEventPosition = 0; //Position (first bit) of error burst
static uint32_t error_in_packet=1;
static uint32_t cptErrorLessPeriod=0;
static uint32_t cptErrorPeriod=0;
static uint32_t errorEventLength;
static int32_t errorLessEventLength;

double TransmissionMode::getCurrentValues(int x)
{
    /**
     * element 0: currentBer;
     * element 1: currentPb;
     * element 2: currentNbitsInChunk;
     * element 3: currentCsr;
     */
    return currentValues[x];
}

NoFecTransmissionMode::NoFecTransmissionMode (double signal_spread, uint32_t rate, double coding_rate)
        : signalSpread (signal_spread),
        rate (rate)
{
    for (int i = 0 ; i<5 ; i++)
        currentValues[i] = 0;
    codingRate = coding_rate;
}

NoFecTransmissionMode::~NoFecTransmissionMode ()
{}

double NoFecTransmissionMode::getSignalSpread (void) const
{
    return signalSpread;
}

uint32_t NoFecTransmissionMode::getDataRate (void) const
{
    return rate;
}

uint32_t NoFecTransmissionMode::getRate (void) const
{
    return rate;
}

double NoFecTransmissionMode::log2 (double val) const
{
    return log(val) / log(2.0);
}

double NoFecTransmissionMode::getBpskBer (double snr) const
{
    double ber;
    /**
     * 1: "[BER: AWGN Channel] "
     * 2: "[BER: Slow-Fading Channel] "
     * 3: "[BER: Fading Channel] "
     * 4: "[BER: Fast-Fading Channel] "
     * 5: "[BER: AWGN Channel -Legacy Method] "
     */
    switch (TYPE_OF_CHANNEL_FOR_BER)
    {
    case 1 :
    case 4 : // (Tc << Ts): Fast fading. The BER is calculated like AWGN case
    {
        double EbNo = snr * signalSpread / rate;
        ber = Qfunction(sqrt(2*EbNo));
    }
    break;

    case 2 :
    {
        double EbNo = snr * signalSpread / rate;
        ber = 1 - pow ( M_E , (-MIN_SNR_FOR_OUTAGE_PROB_IN_SLOW_FADING / EbNo ) );
    }
    break;

    case 3 :
    {
        double EbNo = snr * signalSpread / rate;
        ber = 0.5 * ( 1 - sqrt( EbNo / ( 1 + EbNo)) );
    }
    break;

    case 5 :
    {
        double EbNo = snr * signalSpread / rate;
        double z = sqrt(EbNo);
        ber = 0.5 * erfc(z);
    }
    break;

    default:
    {
        ber = 1;
    }
    }
    return ber;
}

double NoFecTransmissionMode::getQamBer (double snr, unsigned int m) const
{
    double ber;
    /**
     * 1: "[BER: AWGN Channel] "
     * 2: "[BER: Slow-Fading Channel] "
     * 3: "[BER: Fading Channel] "
     * 4: "[BER: Fast-Fading Channel] "
     * 5: "[BER: AWGN Channel -Legacy Method] "
     */
    switch (TYPE_OF_CHANNEL_FOR_BER)
    {
    case 1 :
    case 4 : // (Tc << Ts): Fast fading. The BER is calculated like AWGN case
    {
        double EbNo = snr * signalSpread / rate;
        if (m == 4)
        {
            double symbolErrorProb = 2*Qfunction(sqrt(2*EbNo)) - pow ( Qfunction(sqrt(2*EbNo)) , 2) ;
            ber = 0.5 * symbolErrorProb;
        }
        else if (m > 4)
        {
            double symbolErrorProbTemp1 = Qfunction(sqrt(3*log2(m)*EbNo/(m-1))) ;
            double symbolErrorProbTemp2 = 2*(sqrt((double)m) - 1) * symbolErrorProbTemp1 / sqrt((double)m) ;
            double symbolErrorProbTemp3 =  pow ( (1 - symbolErrorProbTemp2), 2);
            double symbolErrorProb =  1 - symbolErrorProbTemp3;
            ber = symbolErrorProb / log2(m);
        }
    }
    break;

    case 2 :
    {
        double EbNo = snr * signalSpread / rate;
        ber = 1 - pow ( M_E , (-MIN_SNR_FOR_OUTAGE_PROB_IN_SLOW_FADING / (log2(m) * EbNo) ) );
    }
    break;

    case 3 :
    {
        double EbNo = snr * signalSpread / rate;
        if (m == 4)
        {
            // The formula written completely, although the first part could be shortened.
            double alpha = 1 / ( log2(m) * EbNo * pow( sin( M_PI / m ), 2) );
            double symbolErrorProb = 1 - 1/m - 1/sqrt(1 + alpha) + atan( sqrt(1+ alpha) * tan( M_PI / m ) ) / (M_PI * sqrt(1 + alpha) ) ;
            ber = symbolErrorProb / log2(m);

        }
        else if (m > 4)
        {
            double alphaM = 4 * (sqrt((double)m) - 1) / sqrt ((double)m);
            double betaM = 3 / (m - 1);
            double symbolErrorProbTemp = 0.5 * betaM * log2(m) * EbNo;
            double symbolErrorProb = 0.5 * alphaM * ( 1 - sqrt( symbolErrorProbTemp / (1 + symbolErrorProbTemp) ) );
            ber = symbolErrorProb / log2(m);
        }
    }
    break;

    case 5 :
    {
        double EbNo = snr * signalSpread / rate;
        double z = sqrt ((1.5 * log2 (m) * EbNo) / (m - 1.0));
        double z1 = ((1.0 - 1.0 / sqrt ((double)m)) * erfc (z)) ;
        double z2 = 1 - pow ((1-z1), 2.0);
        ber = z2 / log2 (m);
    }
    break;

    default:
    {
        ber = 1;
    }

    }

    return ber;
}

double NoFecTransmissionMode::Qfunction (double x) const
{
    double q = 0.5 * erfc (x / sqrt((double)2)) ;
    return q;
}

FecTransmissionMode::FecTransmissionMode (double signal_spread, uint32_t rate, double codingRate)
        : NoFecTransmissionMode (signal_spread, rate, codingRate)
{

    for (int i = 0 ; i<5 ; i++)
        currentValues[i] = 0;
}

FecTransmissionMode::~FecTransmissionMode ()
{}

uint32_t FecTransmissionMode::getDataRate (void) const
{
    return (uint32_t)(NoFecTransmissionMode::getRate () * codingRate);
}

uint32_t FecTransmissionMode::factorial (uint32_t k) const
{
    uint32_t fact = 1;
    while (k > 0)
    {
        fact *= k;
        k--;
    }
    return fact;
}

double FecTransmissionMode::binomial (uint32_t k, double p, uint32_t n) const
{
    double retval = factorial (n) / (factorial (k) * factorial (n-k)) * pow ((double)p, (double)k) * pow ((double)(1-p), (double)(n-k));
    return retval;
}

double FecTransmissionMode::calculatePdOdd (double ber, unsigned int d) const
{
    assert ((d % 2) == 1);
    unsigned int dstart = (d + 1) / 2;
    unsigned int dend = d;
    double pd = 0;

    for (unsigned int i = dstart; i < dend; i++)
    {
        pd += binomial (i, ber, d);
    }
    return pd;
}
double FecTransmissionMode::calculatePdEven (double ber, unsigned int d) const
{
    assert ((d % 2) == 0);
    unsigned int dstart = d / 2 + 1;
    unsigned int dend = d;
    double pd = 0;

    for (unsigned int i = dstart; i < dend; i++)
    {
        pd +=  binomial (i, ber, d);
    }
    pd += 0.5 * binomial (d / 2, ber, d);

    return pd;
}

double FecTransmissionMode::calculatePd (double ber, unsigned int d) const
{
    double pd;
    if ((d % 2) == 0)
    {
        pd = calculatePdEven (ber, d);
    }
    else
    {
        pd = calculatePdOdd (ber, d);
    }
    return pd;
}


double FecTransmissionMode::calculatePb (double ber, uint32_t dFree, uint32_t Ck[], uint32_t puncturingPeriod) const
{
    /*
        EV << "dFree: " << dFree << endl;
        EV << "ber: " << ber << endl;
        EV << "puncturingPeriod: " << punctur!ingPeriod << endl;
        for (int i = 0 ; i < 10 ; i++)
        EV << "Ck[" << i << "]: " << Ck[i] << endl;
    */
    double Pb = 0;
    /**
     * ber: probability of bit error before the decoder
     * Pb: probabity of bit error after the decoder
     * Pk: The probability of selecting an incorrect path by the Viterbi decoder
     *    -Chernhoff upper bound . Ref. [Pro01, equ.8.2-31]
     * Pk = [4 ber (1 - ber)]^(k/2)
     */

    for (int i = 0 ; i < 10 ; i++)
        Pb = Pb + Ck[i] * pow( (double)(4 * ber * (1 - ber)), (int)((dFree + i)/2 ));

    return (Pb / puncturingPeriod);
}

