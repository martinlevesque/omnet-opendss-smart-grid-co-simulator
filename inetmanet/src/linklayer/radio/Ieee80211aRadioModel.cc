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

#define IEEE80211A
#include "Ieee80211aRadioModel.h"
#include "Ieee80211aConsts.h"

#include "FWMath.h"

#include "Modulation.h"


/* added by Sorin Cocorada sorin.cocorada@gmail.com*/
static const unsigned short comb_calc[15][15]=
{
    {    1,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
    {    2,    1,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
    {    3,    3,    1,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
    {    4,    6,    4,    1,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
    {    5,   10,   10,    5,    1,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
    {    6,   15,   20,   15,    6,    1,    0,    0,    0,    0,    0,    0,    0,    0 },
    {    7,   21,   35,   35,   21,    7,    1,    0,    0,    0,    0,    0,    0,    0 },
    {    8,   28,   56,   70,   56,   28,    8,    1,    0,    0,    0,    0,    0,    0 },
    {    9,   36,   84,  126,  126,   84,   36,    9,    1,    0,    0,    0,    0,    0 },
    {   10,   45,  120,  210,  252,  210,  120,   45,   10,    1,    0,    0,    0,    0 },
    {   11,   55,  165,  330,  462,  462,  330,  165,   55,   11,    1,    0,    0,    0 },
    {   12,   66,  220,  495,  792,  924,  792,  495,  220,   66,   12,    1,    0,    0 },
    {   13,   78,  286,  715, 1287, 1716, 1716, 1287,  715,  286,   78,   13,    1,    0 },
    {   14,   91,  364, 1001, 2002, 3003, 3432, 3003, 2002, 1001,  364,   91,   14,    1 }
};

/*
int comb(int n,int k){
    int p1=1, p2=1,i;
    for(i=0; i<=k-1; i++)
        p1=p1*(n-i);
    for(i=2; i<=k; i++)
        p2=p2*i;
    return p1/p2;
}
*/

static unsigned short comb(int n,int k)
{
//    ASSERT((n>=1) && (n<=14) (k>=1) && (k<=14));
    return comb_calc[n-1][k-1];
}


static double Pd(int d, double Pb)
{
    long double sum=0.0;
    int k;
    if (d%2 == 1)
    {
        for (k=(d+1)/2; k<=d; k++)
            sum+=comb(d,k)*pow(Pb,k)*pow(1.0-Pb, d-k);
    }
    else
    {
        sum=0.5*comb(d,d/2)*pow(Pb, d/2)*pow(1.0-Pb,d/2);
        for (k=d/2+1; k<=d; k++)
            sum+=comb(d,k)*pow(Pb,k)*pow(1.0-Pb, d-k);
    }
    //printf("prob=%f d=%d sum=%f \n",Pb,d, sum);
    return sum;

    //return pow(4*Pb*(1-Pb), d/2);
}


static double Pb(int rate, double prob)
{
    long double probc;
    switch (rate)
    {

    case 1:
        //11, 0, 38, 0, 193, 0, 1331, 0, 7275, 0, 40406, 0, 234969, 0, 1337714, 0, 7594819, 0, 433775588, 0,
        probc=11*Pd(10,prob) + 38*Pd(12,prob) + 193*Pd(14,prob);//+1331*Pd(16,prob);
        break;

    case 2:
        //1, 16, 48, 158, 642, 2435, 9174, 34701, 131533, 499312,
        probc=Pd(6,prob)+16*Pd(7,prob)+48*Pd(8,prob);//+158*Pd(9,prob)+642*Pd(10,prob)
        //+2435*Pd(11,prob)+ 9174*Pd(12,prob)+34701*Pd(13,prob)+131533*Pd(14,prob)+499312*Pd(15,prob);
        break;

    case 3:
        //(8, 31, 160, 892, 4512, 23297, 120976, 624304, 3229885, 16721329,
        probc=8*Pd(5,prob)+31*Pd(6,prob)+150*Pd(7,prob);//+892*Pd(8,prob)+4512*Pd(9,prob)
        //+23297*Pd(10,prob)+120976*Pd(11,prob)+624304*Pd(12,prob)+ 3229885*Pd(13,prob)+ 16721329*Pd(14,prob);
        break;
    default:
        ;
    }

    return probc;
}


static double ber_bpsk(double snir, double bandwidth, double bitrate, char channelModel)
{
    double y=snir*bandwidth/bitrate;

    if (channelModel=='r')//Rayleigh
        return 0.5*(1.0-sqrt(y/(1.0+y)));

    return 0.5*erfc(sqrt(y));//awgn
}

static double ber_qpsk(double snir, double bandwidth, double bitrate, char channelModel)
{
    double y=snir*bandwidth/bitrate;

    if (channelModel=='r')//Rayleigh
        return 0.5*(1.0-sqrt(y/(1.0+y)));
    return 0.5*erfc(sqrt(y));//awgn
}

static double ber_16qam(double snir, double bandwidth, double bitrate, char channelModel)
{
    double y=snir*bandwidth/bitrate;

    if (channelModel=='r')//Rayleigh
        return ( 5.0/8.0-3.0/8.0*sqrt(2.0*y/(5.0+2.0*y))-1.0/4.0*sqrt(18.0*y/(5.0+18.0*y)) );
    return ( 0.375*erfc(sqrt(0.4*y))+0.25*erfc(3.0*sqrt(0.4*y)) );//awgn

}

static double ber_64qam(double snir, double bandwidth, double bitrate, char channelModel)
{
    double y=snir*bandwidth/bitrate;

    if (channelModel=='r')//Rayleigh
        return ( 13.0/24.0-7.0/24.0*sqrt(y/(7.0+y))-1.0/4.0*sqrt(9.0*y/(7.0+9.0*y)) );
    return 7.0/24.0*erfc(sqrt(y/7.0))+0.25*erfc(3.0*sqrt(y/7.0));//awgn
}


/* end added by Sorin Cocorada sorin.cocorada@gmail.com*/


Register_Class(Ieee80211aRadioModel);

Ieee80211aRadioModel::~Ieee80211aRadioModel()
{
    if (phyOpMode=='a')
    {
        for (ModesCI j = modes.begin (); j != modes.end (); j++)
        {
            delete (*j);
        }
        modes.erase (modes.begin (), modes.end ());

        if (IS_ERROR_MASK_GENERATED)
            fclose(error_masks);
    }
    if (parseTable)
        delete parseTable;
}

void Ieee80211aRadioModel::initializeFrom(cModule *radioModule)
{
    snirThreshold = dB2fraction(radioModule->par("snirThreshold"));
    //snirThreshold = dB2fraction(4);

    if (strcmp("b",radioModule->par("phyOpMode").stringValue())==0)
    	phyOpMode='b';
    else if (strcmp("g",radioModule->par("phyOpMode").stringValue())==0)
        phyOpMode='g';
    else if (strcmp("a",radioModule->par("phyOpMode").stringValue())==0)
        phyOpMode='a';
    else
        phyOpMode='b';

    parseTable = NULL;
    const char *fname = radioModule->par("berTableFile");
    std::string name (fname);
    if (!name.empty())
    {
        parseTable = new BerParseFile(phyOpMode);
        parseTable->parseFile(fname);
    }

    if (strcmp("RAYLEIGH",radioModule->par("channelModel").stringValue())==0)
        channelModel='r';
    else if (strcmp("AWGN",radioModule->par("channelModel").stringValue())==0)
        channelModel='a';
    else
        channelModel='r';


    snirVector.setName("snirVector");
    perVector.setName("PER");

    i=0;

    if (phyOpMode=='a')
    {
        configure80211a();
        error_masks = 0;

        if (IS_ERROR_MASK_GENERATED)
        {
            char str[80];
            sprintf(str,"%s%i%s","error_masks",(int)radioModule->par("ID"),".dat");
            error_masks= fopen(str, "w+");
            EV<<"Generation mask file : "<<str<<endl;
        }
    }
}

double Ieee80211aRadioModel::calculateDuration(AirFrame *airframe)
{
    double duration = 0;

    if (phyOpMode=='a')
        duration= (16+airframe->getBitLength()+6)/airframe->getBitrate()+PLCP_PREAMBLE_DELAY+PLCP_SIGNAL_DELAY+T_SYM/2;
    else if (phyOpMode=='g')
        duration=4*ceil((16+airframe->getBitLength()+6)/(airframe->getBitrate()/1e6*4))*1e-6 + 26e-6;
    else
        // The physical layer header is sent with 1Mbit/s and the rest with the frame's bitrate
        duration=airframe->getBitLength()/airframe->getBitrate() + 192/BITRATE_HEADER;
    EV<<"Radio:frameDuration="<<duration*1e6<<"us("<<airframe->getBitLength()<<"bits)"<<endl;
    return duration;
}

bool Ieee80211aRadioModel::isReceivedCorrectly(AirFrame *airframe, const SnrList& receivedList)
{

//    unsigned char packet_ok;
//    unsigned long length;
    unsigned char *buffer=NULL;

//  buffer=(unsigned char*)malloc(airframe->getBitLength()*sizeof(unsigned char));
    if (buffer)
        for (int j=0; j<airframe->getBitLength(); j++)
            //  buffer[i] = airframe->getBuffer(i);
            buffer[j] = 0;///Error, there is no getBuffer() to airframe ;


//   length = airframe->getBitLength()+PHY_HEADER_LENGTH_A+2;

//    length = length/8;
//    printf("\n  length = %d length sans phy=%d",length,airframe->getBitLength());

    // calculate snirMin
    double snirMin = receivedList.begin()->snr;
    for (SnrList::const_iterator iter = receivedList.begin(); iter != receivedList.end(); iter++)
        if (iter->snr < snirMin)
            snirMin = iter->snr;
#if OMNETPP_VERSION > 0x0400
    cPacket *frame = airframe->getEncapsulatedPacket();
#else
    cPacket *frame = airframe->getEncapsulatedMsg();
#endif
    EV << "packet (" << frame->getClassName() << ")" << frame->getName() << " (" << frame->info() << ") snrMin=" << snirMin << endl;

    if (i%1000==0)
    {
        snirVector.record(10*log10(snirMin));
        i=0;
    }
    i++;

    if (snirMin <= snirThreshold)
    {
        // if snir is too low for the packet to be recognized
        EV << "COLLISION! Packet got lost. Noise only\n";
        return false;
    }
    else if (isPacketOK(buffer,snirMin, frame->getBitLength(), airframe->getBitrate()))
    {
        EV << "packet was received correctly, it is now handed to upper layer...\n";
        // for(int i=0;i<airframe->getBitLength();i++)
        //      airframe->setBuffer(i,buffer[i]);
        return true;
    }
    else
    {
        EV << "Packet has BIT ERRORS! It is lost!\n";
        return false;
    }
    if (buffer)
        free(buffer);
}


//static unsigned char phy_header[PHY_HEADER_LENGTH_A];

bool Ieee80211aRadioModel::isPacketOK(unsigned char *buffer, double snirMin, int length, double bitrate)
{
    double berHeader, berMPDU;

    if (phyOpMode=='b')
    {
        berHeader = 0.5 * exp(-snirMin * BANDWIDTH / BITRATE_HEADER);

        // if PSK modulation
        if (bitrate == 1E+6 || bitrate == 2E+6)
            berMPDU = 0.5 * exp(-snirMin * BANDWIDTH / bitrate);
        // if CCK modulation (modeled with 16-QAM)
        else if (bitrate == 5.5E+6)
            berMPDU = 0.5 * (1 - 1 / sqrt(pow(2.0, 4))) * erfc(snirMin * BANDWIDTH / bitrate);
        else if (bitrate == 11E+6)                       // CCK, modelled with 256-QAM
            berMPDU = 0.25 * (1 - 1 / sqrt(pow(2.0, 8))) * erfc(snirMin * BANDWIDTH / bitrate);

    }
#if 0
    else if (phyOpMode=='a')
    {
        double per, psr = 1;
        unsigned char packet_ok=1;

        if (IS_ERROR_MASK_GENERATED) fprintf(error_masks,"]\n");
        psr = 1;
        EV<<"PHY_HEADER_LENGTH"<<PHY_HEADER_LENGTH_A<<endl;
        //psr *= getMode(bitrate)->getChunkSuccessRate (snirMin, PHY_HEADER_LENGTH_A, error_masks, HEADER_BITRATE);
        //psr *= getMode(bitrate)->getChunkSuccessRate (snirMin, length, error_masks, bitrate);
        psr *= getMode(bitrate)->getChunkSuccessRate (snirMin, length, buffer, bitrate);
        per = 1- psr;

        if (i%100==0) perVector.record(per);

        for (int i=0; i<PHY_HEADER_LENGTH_A; i++)
            if (phy_header[i]!=0)
                return(false);

        /*double rand = dblrand();
        if (rand > per)
            return (true);
        //if no error
        else
            return (false);
        */
        return (true);
    }
    else if (phyOpMode=='g')
#endif
        else if ((phyOpMode=='g') || (phyOpMode=='a'))
        {
            //802.11g rates
            //PLCP Header 24bits, BPSK, r=1/2, 6Mbps
            berHeader=ber_bpsk(snirMin, BANDWIDTH , 6E+6, channelModel);
            berHeader=Pb(1, berHeader);


            switch ((int)bitrate) //added by Sorin Cocorada
            {

            case (int)(6E+6)://6Mbps, r=1/2, BPSK
                berMPDU=ber_bpsk(snirMin, BANDWIDTH , bitrate, channelModel);
                berMPDU=Pb(1, berMPDU);
                break;

            case (int)(9E+6)://9Mbps, r=3/4, BPSK
                berMPDU=ber_bpsk(snirMin, BANDWIDTH , bitrate, channelModel);
                berMPDU=Pb(3, berMPDU);
                break;

            case (int)(12E+6)://12Mbps, r=1/2, QPSK
                berMPDU=ber_qpsk(snirMin, BANDWIDTH , bitrate, channelModel);
                berMPDU=Pb(1, berMPDU);
                break;

            case (int)(18E+6)://18Mbps, r=3/4, QPSK
                berMPDU=ber_qpsk(snirMin, BANDWIDTH , bitrate, channelModel);
                berMPDU=Pb(3, berMPDU);
                break;

            case (int)(24E+6)://24Mbps, r=1/2, 16QAM
                berMPDU=ber_16qam(snirMin, BANDWIDTH , bitrate, channelModel);
                berMPDU=Pb(1, berMPDU);
                break;

            case (int)(36E+6)://36Mbps, r=3/4, 16QAM
                berMPDU=ber_16qam(snirMin, BANDWIDTH , bitrate, channelModel);
                berMPDU=Pb(3, berMPDU);
                break;

            case (int)(48E+6)://48Mbps, r=2/3, 64QAM
                berMPDU=ber_64qam(snirMin, BANDWIDTH , bitrate, channelModel);
                berMPDU=Pb(2, berMPDU);
                break;

            case (int)(54E+6)://54Mbps, r=3/4, 64QAM
                berMPDU=ber_64qam(snirMin, BANDWIDTH , bitrate, channelModel);
                berMPDU=Pb(3, berMPDU);
                break;

            default:
                berMPDU=0;
            }

        }

    // probability of no bit error in the PLCP header
    double headerNoError;
    // if(phyOpMode=='g')//added by Sorin Cocorada
    if ((phyOpMode=='g') || (phyOpMode=='a'))//added by Sorin Cocorada
        headerNoError = pow(1.0 - berHeader, 24);//PLCP Header 24bit(without SERVICE), 6Mbps
    else
        headerNoError = pow(1.0 - berHeader, HEADER_WITHOUT_PREAMBLE);

    // probability of no bit error in the MPDU
    double MpduNoError;
    if (parseTable)
        MpduNoError=1-parseTable->getPer(bitrate,snirMin,length/8);
    else
        MpduNoError = pow(1.0 - berMPDU, length);
    EV << "berHeader: " << berHeader << " berMPDU: " <<berMPDU <<" length: "<<length<<" PER: "<<1-MpduNoError<<endl;
    double rand = dblrand();

    if (rand > headerNoError)
        return false; // error in header
    else if (dblrand() > MpduNoError)
        return false;  // error in MPDU
    else
        return true; // no error
}

double Ieee80211aRadioModel::dB2fraction(double dB)
{
    return pow(10.0, (dB / 10));
}

void Ieee80211aRadioModel::addTransmissionMode (TransmissionMode *mode)
{
    modes.push_back (mode);
}

TransmissionMode * Ieee80211aRadioModel::getMode (double rate) const
{
    //EV<<" getMode "<< rate<<endl;
    int mode;
    if (rate == 6E+6)       mode = 0;
    else if (rate == 9E+6)  mode = 1;
    else if (rate == 12E+6) mode = 2;
    else if (rate == 18E+6) mode = 3;
    else if (rate == 24E+6) mode = 4;
    else if (rate == 36E+6) mode = 5;
    else if (rate == 48E+6) mode = 6;
    else if (rate == 54E+6) mode = 7;
    return modes[mode];
}

void Ieee80211aRadioModel::configure80211a (void)
{
    // addTransmissionMode (new FecBpskMode (20e6,  6000000, 0.5 ));
//    addTransmissionMode (new FecBpskMode (20e6,  9000000, 0.75));
//   addTransmissionMode (new FecQamMode  (20e6, 12000000, 0.5,   4));
    //  addTransmissionMode (new FecQamMode  (20e6, 18000000, 0.75,  4));
    // addTransmissionMode (new FecQamMode  (20e6, 24000000, 0.5,   16));
    // addTransmissionMode (new FecQamMode  (20e6, 36000000, 0.75,  16));
    // addTransmissionMode (new FecQamMode  (20e6, 48000000, 0.666, 64));
    // addTransmissionMode (new FecQamMode  (20e6, 54000000, 0.75,  64));
}


