//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include <omnetpp.h>
#include "RangeArea.h"
#include <string>
#include <set>
#include <algorithm>
#include <vector>
#include <math.h>
#include "Ieee80211Consts.h"
#include "MyUtil.h"

using namespace std;

Define_Module(RangeArea);

void RangeArea::initialize()
{
	bitrate = par("bitrate");

	curBitsWithoutError = 0;

	EV << "RANGE AREA BITRATE = " << bitrate << endl;
}

std::vector<Ieee80211Frame*> RangeArea::frameCollides(Ieee80211Frame* frame)
{
	std::vector<Ieee80211Frame*> framesInCollision;

	// First find different frame names
	for (vector<RangeAreaChannelReservation>::iterator it = frames.begin(); it != frames.end(); ++it)
	{
		if (simTime() < it->end && string(frame->getFullName()) != string(it->frame->getFullName()))
		{
			framesInCollision.push_back(it->frame);
		}
	}

	if (framesInCollision.size() > 0)
	{
		// means frame collides too
		framesInCollision.push_back(frame);
	}

	// Then add all frames having these names!!!
	for (int c = 0; c < (int)frames.size(); ++c)
	{
		for (int i = 0; i < (int)framesInCollision.size(); ++i)
		{
			if (string(framesInCollision[i]->getFullName()) == string(frames[c].frame->getFullName()) &&
					find(framesInCollision.begin(), framesInCollision.end(), frames[c].frame) == framesInCollision.end())
			{
				framesInCollision.push_back(frames[c].frame);
			}
		}
	}

	return framesInCollision;
}

void RangeArea::handleMessage(cMessage *msg)
{

}

void RangeArea::finish ()
{
}

void RangeArea::print()
{
	EV << "PRINT .. nb frames = " << frames.size() << endl;

	for (vector<RangeAreaChannelReservation>::iterator it = frames.begin(); it != frames.end(); ++it)
	{
		EV << "		cur reservation.. " << it->frame->getFullName() << " id = " << it->frame->getId() << " " << it->begin << " - " << it->end << endl;
	}
}

void RangeArea::erase(Ieee80211Frame* frame)
{
	EV << "IN ERASE RANGE AREA..." << endl;
	EV << "frame = " << frame->getId() << endl;
	print();
	RangeAreaChannelReservation r;
	r.frame = frame;

	vector<RangeAreaChannelReservation>::iterator it = find(frames.begin(), frames.end(), r);

	ASSERT(it != frames.end());

	if (it != frames.end())
	{
		frames.erase(it);
		EV << "ERASEDDDDDDDDDDDDDD :D" << endl;
	}

	print();
}

void RangeArea::useChannel(Ieee80211Frame* frame, double delay)
{
	EV << "USE CHANNEL ?" << endl;
	print();

	///////////////
	// BER stuff


	// Only consider data frames
	ListEtherFrames* framesInFrame = dynamic_cast<ListEtherFrames*>(frame->getEncapsulatedPacket());

	if ( ! framesInFrame || bitsAllowedWithoutError == 0)
	{
		return;
	}

	if (selectiveBERAck) // Selectively mark as error each packet
	{
		for (int i = 0; i < (int)framesInFrame->getFramesArraySize(); ++i)
		{
			EtherFrame& f = framesInFrame->getFrames(i);

			int l = MyUtil::getTrafficMatrix(this)->getReqLength() * 8;

			for (int i = 0; i < l && BER > 0; ++i)
			{
				long r = (long)uniform(0, bitsAllowedWithoutError);

				if (r == 10)
				{
					f.setKind(BITERROR);
					break;
				}
			}
		}
	}
	else // NOT selective, drop all or nothing
	{
		bool dropAll = false;

		for (int i = 0; i < (int)framesInFrame->getFramesArraySize(); ++i)
		{
			int l = MyUtil::getTrafficMatrix(this)->getReqLength() * 8;

			// NEW CALC:
			for (int i = 0; i < l && BER > 0; ++i)
			{
				long r = (long)uniform(0, bitsAllowedWithoutError);

				if (r == 10)
				{
					dropAll = true;
					break;
				}
			}

			if (dropAll)
				break;
			// end new calc

			/* OLD
			curBitsWithoutError += l;

			EV << "RangeArea - curBitsWithoutError = " << curBitsWithoutError << " bitsAllowedWithoutError = " << bitsAllowedWithoutError << endl;

			if (curBitsWithoutError >= bitsAllowedWithoutError)
			{
				curBitsWithoutError = 0;
				dropAll = true;
			}
			*/

		}



		EV << " 	dropAll = " << dropAll << endl;

		if (dropAll)
		{
			for (int i = 0; i < (int)framesInFrame->getFramesArraySize(); ++i)
			{
				EtherFrame& f = framesInFrame->getFrames(i);
				EV << " 	should drop... to " << BITERROR << endl;
				f.setKind(BITERROR);
				EV << " 	should drop... len = " << f.getByteLength() << " real " << f.getKind() << endl;
			}
		}
	}
}

double RangeArea::delayOf(double bits)
{
	return bits / bitrate;
}

void RangeArea::setBER(double p_ber)
{
	BER = p_ber;

	// bits allowed without error is expressed by: Bitrate * (1 - BER)
	// bitsAllowedWithoutError = floor(((1 - BER) * bitrate) + 0.5);

	if (BER == 0)
		bitsAllowedWithoutError = 0;
	else
		bitsAllowedWithoutError = floor((1 / BER) + 0.5);
}

std::vector<Ieee80211Frame*> RangeArea::channelBusy(Ieee80211Frame* frame)
{
	EV << "CHANNEL BUSY ?" << endl;
	print();
	vector<Ieee80211Frame*> framesInCollision = frameCollides(frame);

	//if (framesInCollision.size() > 0)
	//{
	RangeAreaChannelReservation r;
	r.frame = frame;
	r.begin = simTime();
	r.end = r.begin + delayOf(frame->getBitLength());

	ASSERT(find(frames.begin(), frames.end(), r) == frames.end());

	//if ( ! alreadyInlist(r))
	frames.push_back(r);
	//}

	return framesInCollision;
}
