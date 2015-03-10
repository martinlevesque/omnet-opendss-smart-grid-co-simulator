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

#ifndef TRANSMISSION_MODE_H
#define TRANSMISSION_MODE_H


#define _USE_MATH_DEFINES

#include <stdio.h>

#include "IModulation.h"
#include "Ieee80211Consts.h"
//#ifdef  _WIN3
//#include "types.h"
//#endif


#ifndef M_E
#define M_E     2.7182818284590452353602874713527
#endif

#ifndef M_PI
#define M_PI 3.14
#endif

/**
 * Small-scale fading & multipath model:
 * Fading channel is very flexible and comprehensive and puts all the power of IT++ library
 * at your disposal. You may select a Rayleigh channel or a Rician one for simulating a slow
 * flat fading channel.
 * You can also set the normalized doppler frequecy (DopplerFrequency / SymbolRate)
 * Cases NOT covered:
 * The channel models a slow flat fading channel, i.e. the channel is neither frequency-selective,
 * nor of fast fading type. Please refer to the accompanying documentation for more info.
 */
#define IS_FADING_CHANNEL_USED 1
/**
 * Generating FADING_NUMBER_OF_SAMPLES of the fading process and storing
 * them in m_fading_process_coeffs matrix
 */
#define FADING_NUMBER_OF_SAMPLES 20000

/**
 * SIMULATION_BAUD_RATE is used to discretize Channel_Specification before assigning it
 * to the channel (A requirement of IT++). The discretization should be set to sampling
 * time, i.e. 1/SIMULATION_BAUD_RATE .
 * Baud Rate is actually symbol rate, i.e., considering the relation between modulation type
 * and number of bits in each modulated symbol. But here, by symbol, we mean OFDM symbol.
 * So the highest OFDM symbol rate in terms of number of bits is : (54000000/48)
 * We set this to the highest rate, lower rates are covered as a result.
 */
#define SIMULATION_BAUD_RATE (54000000/48)

/**
 * Doppler Freq.= SpeedOfObjects/Lambda
 * NORMALIZED_DOPPLER_FREQUENCY = Doppler Freq. / Baud Rate
 */

#define NORMALIZED_DOPPLER_FREQUENCY 0.01
/**
 * set_channel_profile (const vec &avg_power_dB="0", const ivec &delay_prof="0")
 * The average effect of the application of the fading process is set to 0 dB.
 * Please note that we choose the fading channel as the 2nd half of the model, where
 * the 1st half is one of the FreeSpace/2-Ray/Shadowing models.
 * The second argument sets the delays in the taps for Tapped Delay Line modeling of
 * frequency-selective channels. As we consider indoor 802.11 channel model flat, we just
 * consider one tap and set the delay to 0.
 */
#define AVERAGE_POWER_PROFILE_dB 0

/**
 * set_doppler_spectrum (DOPPLER_SPECTRUM *tap_spectrum)
 * set_LOS (const double relative_power, const double norm_doppler)
 * LOS component for the first tap (zero delay). Rice must be chosen as doppler spectrum.
 * Relative power (Rice factor) and normalized doppler.
 * Rice: the classical Jakes spectrum and a direct tap.
 */
#define FADING_CHANNEL_RICIAN_FACTOR 0

/**
 * Set to 1 if you want to generate error masks, otherwise to 0.
 */
#define IS_ERROR_MASK_GENERATED 1

/**
 * 1: "[BER: AWGN Channel] "
 * 2: "[BER: Slow-Fading Channel] "
 * 3: "[BER: Fading Channel] "
 * 4: "[BER: Fast-Fading Channel] "
 * 5: "[BER: AWGN Channel -Legacy Method] "
 *
 * TYPE_OF_CHANNEL_FOR_BER is used in:
 * - Phy80211::print_transmission_mode_status(void)
 * - NoFecTransmissionMode::get_bpsk_ber (double snr) const
 * - NoFecTransmissionMode::get_qam_ber (double snr, unsigned int m) const
 */
#define TYPE_OF_CHANNEL_FOR_BER 2

/**
 * This is used in BER calculation formula for Slow-Fading case.
 */
#define MIN_SNR_FOR_OUTAGE_PROB_IN_SLOW_FADING 1

/**
 * 0: "[PER Calculation Method (Error Distribution at the Viterbi Decoder's Output: Uniform)]"
 * 1: "[PER Calculation Method (Error Distribution at the Viterbi Decoder's Output: Non-Uniform)]"
 */
#define PER_CALCULATION_METHOD 1

/**
 * This sets the value of m_phy_rx_noise_db in network-interface-80211-factory.cc
 * It is used for bringing the range of the reception (or SNR) to a reasonable value.
 * In the same class, we have:
 * m_phy_tx_power_base_dbm = 14
 * m_phy_ed_threshold_dbm = -140
 */
#define PHY_RECEIVER_NOISE_LEVEL 17


class INET_API TransmissionMode : public IModulation
{
  public:
    virtual ~TransmissionMode () = 0;

    /* returns the half-width of the signal (Hz)
    */
    virtual double getSignalSpread (void) const = 0;

    /* returns the number of user bits per
     * second achieved by this transmission mode
     */
    virtual uint32_t getDataRate (void) const = 0;

    /* returns the number of raw bits per
     * second achieved by this transmission mode.
     * the value returned by getRate and getDataRate
     * will be different only if there is some
     * FEC overhead.
     */
    virtual uint32_t getRate (void) const = 0;

    /* @snr: the snr, (W/W)
     * @nbits: length of transmission (bits)
     *
     * Returns the probability that nbits be successfully transmitted.
     */
    virtual double getChunkSuccessRate (double snr, unsigned int nbits, unsigned char *buffer, double bitrate) = 0;
    virtual uint32_t getBitNumbersPerModulationSymbol (void) const = 0;  // Modulation Symbol: BPSK or M-QAM

    void generateErrorMasks(unsigned int nbits, double Pb, bool error_distribution_type, unsigned char *buffer, double EER, double snr);

    double CurrentGeneratedRandomNumberForMaskGeneration ;
    /**
     * element 0: currentBer;
     * element 1: currentPb;
     * element 2: currentNbitsInChunk;
     * element 3: currentCsr;
     */
    double currentValues[5]; // stores the above values.
    double getCurrentValues(int);

    uint32_t dFree, adFree, Ck[10], puncturingPeriod, coderOutputBits;
    double codingRate ;
};

class NoFecTransmissionMode : public TransmissionMode
{
  public:
    NoFecTransmissionMode (double signal_spread, uint32_t rate, double coding_rate);
    virtual ~NoFecTransmissionMode () = 0;
    virtual double getSignalSpread (void) const;
    virtual uint32_t getDataRate (void) const;
    virtual uint32_t getRate (void) const;

  private:
    double signalSpread;
    uint32_t rate;
  protected:
    double getBpskBer (double snr) const;
    double getQamBer (double snr, unsigned int m) const;
    double log2 (double v) const;

    double Qfunction (double x) const;
};

class FecTransmissionMode  : public NoFecTransmissionMode
{
  public:
    FecTransmissionMode (double signal_spread, uint32_t rate, double coding_rate);
    virtual ~FecTransmissionMode () = 0;
    virtual uint32_t getDataRate (void) const;

  protected:
    double calculatePdOdd (double ber, unsigned int d) const;
    double calculatePdEven (double ber, unsigned int d) const;
    double calculatePd (double ber, unsigned int d) const;

    double calculatePb (double ber, uint32_t d_free, uint32_t Ck[], uint32_t puncturing_period) const;

  private:
    uint32_t factorial (uint32_t k) const;
    double binomial (uint32_t k, double p, uint32_t n) const;

};

#endif /* TRANSMISSION_MODE_H */
