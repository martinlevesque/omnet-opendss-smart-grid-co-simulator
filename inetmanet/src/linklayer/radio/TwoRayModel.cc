//
// Copyright (C) 2007 Andras Varga, Ahmed Ayadi
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
// Authors:   Ahmed Ayadi < Ahmed.Ayadi@ensi.rnu.tn>
//            Masood Khosroshahy < m.khosroshahy@iee.org>

#include "TwoRayModel.h"
#include "FWMath.h"

Register_Class(TwoRayModel);

void TwoRayModel::initializeFrom(cModule *radioModule)
{
    EV<< "TwoRayModel selected "<< endl;
    systemLoss = radioModule->par("systemLoss");
    receiverAntennaGain = radioModule->par("receiverAntennaGain");
    ht = radioModule->par("ht");
    hr = radioModule->par("hr");
    EV << "ht = "  <<ht;
    EV << "hr = "  <<hr;
}

double TwoRayModel::calculateReceivedPower(double pSend, double carrierFrequency, double distance)
{
    double numerator = pSend * receiverAntennaGain * pow(ht,2) * pow(hr,2);
    double denominator = pow(distance, 4) * systemLoss;
    double prec = (numerator/denominator);
    if (prec > pSend)
        prec = pSend;
    return prec;
}

