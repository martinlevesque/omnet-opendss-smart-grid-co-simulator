//
// Copyright (C) 2006 Andras Varga, Levente Meszaros
// Based on the Mobility Framework's SnrEval by Marc Loebbers
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

#ifndef Ieee80211aRadioModel_H
#define Ieee80211aRadioModel_H

#include "bpskMode.h"

#include "IRadioModel.h"


#include <vector>
#include <list>

#include "IRadioModel.h"
#include "IModulation.h"
#include "BerParseFile.h"
/**
 * Radio model for IEEE 802.11. The implementation is largely based on the
 * Mobility Framework's SnrEval80211 and Decider80211 modules.
 * See the NED file for more info.
 */

class INET_API Ieee80211aRadioModel : public IRadioModel
{
  protected:
    BerParseFile *parseTable;
    char phyOpMode,channelModel;
    double snirThreshold;
    cOutVector snirVector;
    int i;

    typedef std::vector<TransmissionMode *> Modes;
    typedef std::vector<TransmissionMode *>::const_iterator ModesCI;

    long headerLengthBits;
    double perDb;
    double bandwidth;

    Modes modes;
    FILE *error_masks ;
    cOutVector perVector;

  public:
    virtual ~Ieee80211aRadioModel();

    virtual void initializeFrom(cModule *radioModule);

    virtual double calculateDuration(AirFrame *airframe);

    virtual bool isReceivedCorrectly(AirFrame *airframe, const SnrList& receivedList);

    // used by the Airtime Link Metric computation
    virtual bool haveTestFrame() {return false;}
    virtual double calculateDurationTestFrame(AirFrame *airframe) {return 0;}
    virtual double getTestFrameError(double snirMin, double bitrate) {return 0;}
    virtual int    getTestFrameSize() {return 0;}

  protected:
    // utility
    virtual bool isPacketOK(unsigned char  *buffer, double snirMin, int length, double bitrate);
    // utility
    virtual double dB2fraction(double dB);

    void configure80211a (void);

    TransmissionMode * getMode (double bitrate) const;
    void addTransmissionMode (TransmissionMode *mode);
};

#endif

