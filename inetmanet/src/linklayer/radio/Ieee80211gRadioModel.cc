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

#include <fstream>
#include <sstream>
#include <string>
#include "Ieee80211gRadioModel.h"
#include "Ieee80211Consts.h"
#include "FWMath.h"




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
unsigned long comb(int n,int k){
    unsigned long long p1=1, p2=1,i;
    for(i=0; i<=k-1; i++)
        p1=p1*(n-i);
    for(i=2; i<=k; i++)
        p2=p2*i;
    return p1/p2;
}
*/

static unsigned short comb(int n,int k)
{
    ASSERT(n>=1 && n<=14 && k>=1 && k<=14);
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
//              +2435*Pd(11,prob)+ 9174*Pd(12,prob)+34701*Pd(13,prob)+131533*Pd(14,prob)+499312*Pd(15,prob);
        break;
    case 3:
        //(8, 31, 160, 892, 4512, 23297, 120976, 624304, 3229885, 16721329,
        probc=8*Pd(5,prob)+31*Pd(6,prob)+150*Pd(7,prob);//+892*Pd(8,prob)+4512*Pd(9,prob)
//          +23297*Pd(10,prob)+120976*Pd(11,prob)+624304*Pd(12,prob)+ 3229885*Pd(13,prob)+ 16721329*Pd(14,prob);
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





Register_Class(Ieee80211gRadioModel);

double Ieee80211gRadioModel::getPer(double speed, double tsnr, int tlen)
{

    BerList *berlist;

    if (phyOpMode=='b')
    {
        if (speed<1)
            berlist = &r1m;
        else if (speed<=2&& speed<5)
            berlist = &r2m;
        else if (speed<=5&& speed<11)
            berlist = &r5m;
        else
            berlist = &r11m;
    }
    else
    {
        if (speed<9)
            berlist = &r6m;
        else if (speed<=9&& speed<12)
            berlist = &r9m;
        else if (speed<=12&& speed<18)
            berlist = &r12m;
        else if (speed<=18&& speed<24)
            berlist = &r18m;
        else if (speed<=24&& speed<36)
            berlist = &r24m;
        else if (speed<=36&& speed<48)
            berlist = &r36m;
        else if (speed<=48&& speed<54)
            berlist = &r48m;
        else
            berlist = &r54m;
    }
    LongBer * pre;
    LongBer * pos;
    unsigned int j;
    for (j=0; j<berlist->size(); j++)
    {
        pos = *(berlist->begin()+j);
        if (pos->longpkt >=tlen)
        {
            break;
        }
    }

    if (j==0)
        pre = NULL;
    else
    {
        if (j==berlist->size())
            pre = *(berlist->begin()+j-2);
        else
            pre = *(berlist->begin()+j-1);
    }
    SnrBer snrdata1;
    SnrBer snrdata2;
    SnrBer snrdata3;
    SnrBer snrdata4;
    snrdata3.snr=-1;
    snrdata3.ber=-1;
    snrdata4.snr=-1;
    snrdata4.ber=-1;
    if (pos->snrlist[(pos->snrlist.size()-1)].snr<=tsnr)
    {
        snrdata1 = pos->snrlist[pos->snrlist.size()-1];
        snrdata2 = pos->snrlist[pos->snrlist.size()-1];
    }
    else
    {
        for (j=0; j<pos->snrlist.size(); j++)
        {
            snrdata1 = pos->snrlist[j];
            if (tsnr<=snrdata1.snr)
              break;
        }
        if (j==0)
        {
            snrdata2.snr=-1;
            snrdata2.ber=-1;
        }
        else
        {
            if (j==pos->snrlist.size())
                snrdata2 = *(pos->snrlist.begin()+j-2);
            else
                snrdata2 = *(pos->snrlist.begin()+j-1);
        }
    }
    if (pre==NULL)
        pre = pos;
    if (pre->snrlist[(pre->snrlist.size()-1)].snr<=tsnr)
    {
        snrdata3 = pre->snrlist[pos->snrlist.size()-1];
        snrdata4 = pre->snrlist[pos->snrlist.size()-1];
    }
    else
    {
        for (j=0; j<pre->snrlist.size(); j++)
        {
            snrdata3 = pre->snrlist[j];
            if (tsnr<=snrdata3.snr)
                break;
        }
        if (j!=0)
        {
            if (j==pre->snrlist.size())
                snrdata4 =*(pre->snrlist.begin()+j-2);
            else
                snrdata4 =*(pre->snrlist.begin()+j-1);
        }
    }


    if (snrdata2.snr==-1)
    {
        snrdata2.snr=snrdata1.snr;
        snrdata2.ber=snrdata1.ber;
    }
    if (snrdata4.snr==-1)
    {
        snrdata4.snr=snrdata3.snr;
        snrdata4.ber=snrdata3.ber;
    }
    double per1,per2,per;
    per1 = snrdata1.ber;
    per2 = snrdata3.ber;

    if (tsnr<= snrdata1.snr)
    {
        if (snrdata2.snr!=snrdata1.snr)
            per1 = snrdata1.ber +  (snrdata2.ber-snrdata1.ber)/(snrdata2.snr-snrdata1.snr)*(tsnr- snrdata1.snr);
    }

    if (tsnr<= snrdata3.snr)
    {
        if (snrdata3.snr!=snrdata4.snr)
            per2 = snrdata3.ber +  (snrdata4.ber-snrdata3.ber)/(snrdata4.snr-snrdata3.snr)*(tsnr- snrdata3.snr);
    }


    if (per1!=-1 && per2!=-1)
    {
        if (pos->longpkt != pre->longpkt)
            per = per2 +  (per1- per2)/(pos->longpkt - pre->longpkt)*(tlen-pre->longpkt);
        else
            per = per2;
    }
    else if (per1!=-1)
    {
        per = per1;
    }
    else if (per2!=-1)
    {
        per = per2;
    }
    else {EV << "No PER available";}
    return per;

}


void Ieee80211gRadioModel::parseFile(const char *filename)
{

    std::ifstream in(filename, std::ios::in);

    if (in.fail())
        opp_error("Cannot open file '%s'",filename);
    std::string line;
    std::string subline;

    while (std::getline(in, line))
    {
        // '#' line
        std::string::size_type found=line.find('#');
        if (found == 0)
            continue;
        if (found != std::string::npos)
            subline = line;
        else
            subline = line.substr(0,found);
        found=subline.find("$self");
        if (found == std::string::npos)
            continue;
        // Node Id
        found = subline.find("add");
        if (found== std::string::npos)
            continue;
        // Initial position

        std::string::size_type pos1 = subline.find("Mode");
        std::string::size_type pos2 = subline.find("Mb");
        BerList *berlist;
        std::string  substr = subline.substr(pos1+4,pos2-(pos1+4));
//                int speed = std::atof (subline.substr(pos1+4,pos2).c_str());

        if (!strcmp(substr.c_str(),"1"))
            berlist = &r1m;
        else if (!strcmp(substr.c_str(),"2"))
            berlist = &r2m;
        else if (!strcmp(substr.c_str(),"5_5"))
            berlist = &r5m;
        else if (!strcmp(substr.c_str(),"6"))
            berlist = &r6m;
        else if (!strcmp(substr.c_str(),"9"))
            berlist = &r9m;
        else if (!strcmp(substr.c_str(),"11"))
            berlist = &r11m;
        else if (!strcmp(substr.c_str(),"12"))
            berlist = &r12m;
        else if (!strcmp(substr.c_str(),"18"))
            berlist = &r18m;
        else if (!strcmp(substr.c_str(),"24"))
            berlist = &r24m;
        else if (!strcmp(substr.c_str(),"36"))
            berlist = &r36m;
        else if (!strcmp(substr.c_str(),"48"))
            berlist = &r48m;
        else if (!strcmp(substr.c_str(),"54"))
            berlist = &r54m;
        else
            continue;

        std::string parameters = subline.substr (pos2+3,std::string::npos);
        std::stringstream linestream(parameters);
        int pkSize;
        double snr;
        double ber;
        linestream >> pkSize;
        linestream >> snr;
        linestream >> ber;
        snr =  dB2fraction(snr);
        if (pkSize<128)
            pkSize=128;
        else if (128<=pkSize && pkSize<256)
            pkSize=128;
        else if (256<=pkSize&& pkSize<512)
            pkSize=256;
        else if (512<=pkSize&& pkSize<1024)
            pkSize=512;
        else if (1024<=pkSize && pkSize<1500)
            pkSize=1024;
        else
            pkSize=1500;
        if (berlist->size()==0)
        {
            LongBer * l = new LongBer;
            l->longpkt = pkSize;
            berlist->push_back(l);
        }
        LongBer * l;
        bool inList = false;
        for (unsigned int j=0; j<berlist->size(); j++)
        {
            l = *(berlist->begin()+j);
            if (l->longpkt==pkSize)
            {
                inList=true;
                break;
            }
        }
        if (!inList)
        {
            l = new LongBer;
            l->longpkt = pkSize;
            berlist->push_back(l);
        }
        if (l->snrlist.size()==0)
        {
            SnrBer snrdata;
            snrdata.snr=snr;
            snrdata.ber=ber;
            l->snrlist.push_back(snrdata);
        }
        else
            for (unsigned int j=0; j<l->snrlist.size(); j++)
            {
                SnrBer snrdata = l->snrlist[j];
                if (snr<snrdata.snr)
                {
                    SnrBer snrdata2;
                    snrdata2.snr=snr;
                    snrdata2.ber=ber;
                    l->snrlist.insert(l->snrlist.begin()+j,snrdata2);
                    break;
                }
                else if (j+1==l->snrlist.size())
                {
                    SnrBer snrdata2;
                    snrdata2.snr=snr;
                    snrdata2.ber=ber;
                    l->snrlist.push_back(snrdata2);
                    break;
                }
            }

    }

    in.close();

    // exist data?
}

Ieee80211gRadioModel::~Ieee80211gRadioModel()
{
// B
    while (r1m.size()>0)
    {
        LongBer *p = r6m.back();
        r6m.pop_back();
        p->snrlist.clear();
        delete p;

    }
    while (r2m.size()>0)
    {
        LongBer *p = r6m.back();
        r6m.pop_back();
        p->snrlist.clear();
        delete p;

    }
    while (r5m.size()>0)
    {
        LongBer *p = r6m.back();
        r6m.pop_back();
        p->snrlist.clear();
        delete p;

    }

    while (r11m.size()>0)
    {
        LongBer *p = r6m.back();
        r6m.pop_back();
        p->snrlist.clear();
        delete p;

    }
// A and G
    while (r6m.size()>0)
    {
        LongBer *p = r6m.back();
        r6m.pop_back();
        p->snrlist.clear();
        delete p;

    }
    while (r9m.size()>0)
    {
        LongBer *p = r9m.back();
        r9m.pop_back();
        p->snrlist.clear();
        delete p;

    }
    while (r12m.size()>0)
    {
        LongBer *p = r12m.back();
        r12m.pop_back();
        p->snrlist.clear();
        delete p;

    }
    while (r18m.size()>0)
    {
        LongBer *p = r18m.back();
        r18m.pop_back();
        p->snrlist.clear();
        delete p;

    }
    while (r24m.size()>0)
    {
        LongBer *p = r24m.back();
        r24m.pop_back();
        p->snrlist.clear();
        delete p;

    }
    while (r36m.size()>0)
    {
        LongBer *p = r36m.back();
        r36m.pop_back();
        p->snrlist.clear();
        delete p;

    }
    while (r48m.size()>0)
    {
        LongBer *p = r48m.back();
        r48m.pop_back();
        p->snrlist.clear();
        delete p;

    }
    while (r54m.size()>0)
    {
        LongBer *p = r54m.back();
        r54m.pop_back();
        p->snrlist.clear();
        delete p;

    }
    if (parseTable)
        delete parseTable;
}


void Ieee80211gRadioModel::initializeFrom(cModule *radioModule)
{
    snirThreshold = dB2fraction(radioModule->par("snirThreshold"));

    if (strcmp("RAYLEIGH",radioModule->par("channelModel").stringValue())==0)
        channelModel='r';
    else if (strcmp("AWGN",radioModule->par("channelModel").stringValue())==0)
        channelModel='a';
    else
        channelModel='r';

    if (strcmp("b",radioModule->par("phyOpMode").stringValue())==0)
        phyOpMode='b';
    else if (strcmp("g",radioModule->par("phyOpMode").stringValue())==0)
        phyOpMode='g';
    else
        phyOpMode='g';

    PHY_HEADER_LENGTH=radioModule->par("PHY_HEADER_LENGTH");
    if (PHY_HEADER_LENGTH<0)
        PHY_HEADER_LENGTH=26e-6;

    parseTable = NULL;

    snirVector.setName("snirVector");
    i=0;
    const char *fname = radioModule->par("berTableFile");
    std::string name (fname);
    if (!name.empty())
    {
        if (radioModule->par("useBerClass"))
        {
            parseTable = new BerParseFile(phyOpMode);
            parseTable->parseFile(fname);
        }
        else
            parseFile(fname);
        fileBer=true;
    }
    else
        fileBer=false;
}



double Ieee80211gRadioModel::calculateDuration(AirFrame *airframe)
{
    double duration;

    if (phyOpMode=='g')
        duration=4*ceil((16+airframe->getBitLength()+6)/(airframe->getBitrate()/1e6*4))*1e-6 + PHY_HEADER_LENGTH;
    else
        // The physical layer header is sent with 1Mbit/s and the rest with the frame's bitrate
        duration=airframe->getBitLength()/airframe->getBitrate() + 192/BITRATE_HEADER;
    EV<<"Radio:frameDuration="<<duration*1e6<<"us("<<airframe->getBitLength()<<"bits)"<<endl;
    return duration;
}


bool Ieee80211gRadioModel::isReceivedCorrectly(AirFrame *airframe, const SnrList& receivedList)
{
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
    else if (isPacketOK(snirMin, frame->getBitLength(), airframe->getBitrate()))
    {
        EV << "packet was received correctly, it is now handed to upper layer...\n";
        return true;
    }
    else
    {
        EV << "Packet has BIT ERRORS! It is lost!\n";
        return false;
    }
}


bool Ieee80211gRadioModel::isPacketOK(double snirMin, int lengthMPDU, double bitrate)
{
    double berHeader, berMPDU;

    berHeader = 0.5 * exp(-snirMin * BANDWIDTH / BITRATE_HEADER);

    // if PSK modulation
    if (bitrate == 1E+6 || bitrate == 2E+6)
        berMPDU = 0.5 * exp(-snirMin * BANDWIDTH / bitrate);
    // if CCK modulation (modeled with 16-QAM)
    else if (bitrate == 5.5E+6)
        berMPDU = 0.5 * (1 - 1 / sqrt(pow(2.0, 4))) * erfc(snirMin * BANDWIDTH / bitrate);
    else if (bitrate == 11E+6)                       // CCK, modelled with 256-QAM
        berMPDU = 0.25 * (1 - 1 / sqrt(pow(2.0, 8))) * erfc(snirMin * BANDWIDTH / bitrate);

    else //802.11g rates
    {
        //PLCP Header 24bits, BPSK, r=1/2, 6Mbps
        berHeader=ber_bpsk(snirMin, BANDWIDTH , 6E+6, channelModel);
        berHeader=Pb(1, berHeader);

        if (fileBer)
            berMPDU = 0;
        else
            switch ((int)bitrate)
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


    if (berHeader > 1.0 || berMPDU > 1.0)
        return false;// error in MPDU
    // probability of no bit error in the PLCP header
    double headerNoError;
    if (phyOpMode=='g')
        headerNoError = pow(1.0 - berHeader, 24);//PLCP Header 24bit(without SERVICE), 6Mbps
    else
        headerNoError = pow(1.0 - berHeader, HEADER_WITHOUT_PREAMBLE);

    // probability of no bit error in the MPDU
    double MpduNoError;
    if (fileBer)
        if (parseTable)
            MpduNoError=1-parseTable->getPer(bitrate,snirMin,lengthMPDU/8);
        else
            MpduNoError=1-getPer(bitrate,snirMin,lengthMPDU/8);
    else
        MpduNoError = pow(1.0 - berMPDU, lengthMPDU);
    EV << "berHeader: " << berHeader << " berMPDU: " <<berMPDU <<" lengthMPDU: "<<lengthMPDU<<" PER: "<<1-MpduNoError<<endl;
    double rand = dblrand();

    if (rand > headerNoError)
        return false; // error in header
    else if (dblrand() > MpduNoError)
        return false;  // error in MPDU
    else
        return true; // no error
}

double Ieee80211gRadioModel::dB2fraction(double dB)
{
    return pow(10.0, (dB / 10));
}


