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
#include "MPCPTools.h"
#include <map>
#include <string>
#include "Ieee80211Frame_m.h"

#ifndef __FIWI_RANGE_AREA_H__
#define __FIWI_RANGE_AREA_H__

struct RangeAreaChannelReservation
{
	simtime_t begin;
	simtime_t end;
	Ieee80211Frame* frame;

	bool operator==(const RangeAreaChannelReservation& r2) const
	{
		return frame == r2.frame;
	}
};

/**
 *
 *
 */
class RangeArea : public cSimpleModule
{
	private:
		double bitrate;
		double BER;
		double curBitsWithoutError;
		double bitsAllowedWithoutError;
		std::vector<RangeAreaChannelReservation> frames;
		bool selectiveBERAck;

		void print();

	protected:
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
		virtual void finish();

	public:
		std::vector<Ieee80211Frame*> frameCollides(Ieee80211Frame* frame);
		void useChannel(Ieee80211Frame* frame, double delay);
		std::vector<Ieee80211Frame*> channelBusy(Ieee80211Frame* frame);
		double delayOf(double bits);
		void erase(Ieee80211Frame* frame);

		void setBER(double p_ber);
		void setSelectiveBERAck(bool p_selectiveBERAck) { selectiveBERAck = p_selectiveBERAck; }
};

#endif
