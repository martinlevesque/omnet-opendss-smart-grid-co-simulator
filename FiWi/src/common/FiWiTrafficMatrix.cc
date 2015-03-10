/*
 * Copyright (C) 2003 Andras Varga; CTIE, Monash University, Australia
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include <omnetpp.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "FiWiTrafGen.h"
#include "Ieee802Ctrl_m.h"
#include "EtherApp_m.h"
#include "MyUtil.h"
#include "EtherMAC.h"
#include "EtherMacVlan.h"
#include "EPON_mac.h"
#include "FiWiRoutingTable.h"
#include "FiWiTrafficMatrix.h"
#include <string>
#include <algorithm>
#include "FiWiGeneralConfigs.h"

#include <iostream>
#include <fstream>

using namespace std;

Define_Module (FiWiTrafficMatrix);

void FiWiTrafficMatrix::initialize(int stage)
{
	if (stage != 5)
		return;

	trafFile = par("trafFile").str();
	bool withGroupedPackets = par("withGroupedPackets").boolValue();
	reqLength = par("reqLength").longValue();

	trafFile = trafFile.substr(1, trafFile.size() - 2);

	ifstream configFileStream;

	configFileStream.open(trafFile.c_str());


	if (configFileStream.is_open())
	{
		while ( ! configFileStream.eof())
		{
			TrafficPair traf;

			string src, dest;

			configFileStream >> src;
			configFileStream >> dest;
			configFileStream >> traf.lambda;
			configFileStream >> traf.trafficClass;

			if (src == "" || dest == "")
			{
				break;
			}

			if (withGroupedPackets && FiWiGeneralConfigs::aggregationTresholdBytes > 0)
			{
				// Then, we need to change the lambda, to generate bursts for 802.11n aggregates
				int nb = FiWiGeneralConfigs::aggregationTresholdBytes / reqLength;
				traf.lambda /= (double)nb;
			}

			traf.src = resolveDestMACAddress(src);
			traf.dest = resolveDestMACAddress(dest);
			EV << "TRAF MATRIX src = " << traf.src << endl;
			EV << "TRAF MATRIX dest = " << traf.dest << endl;
			EV << "TRAF MATRIX lambda = " << traf.lambda << endl;
			EV << "TRAF MATRIX traf.trafficClass = " << traf.trafficClass << endl;

			trafficMatrix[traf.src].push_back(traf);
		}
	}
	else
	{
		error("FiWiTrafficMatrix::initialize can't read file conf");
	}

	configFileStream.close();
}

MACAddress FiWiTrafficMatrix::resolveDestMACAddress(const string& addr)
{
    MACAddress destMACAddress;

	// try as mac address first, then as a module
	if (!destMACAddress.tryParse(addr.c_str()))
	{
		cModule *destStation = simulation.getModuleByPath(addr.c_str());
		if (!destStation)
		{
			if (FiWiGeneralConfigs::DEBUG_MODE())
				return destMACAddress;
			else
				error("cannot resolve MAC address '%s': not a 12-hex-digit MAC address or a valid module path name", addr.c_str());
		}
		cModule* module = destStation->getSubmodule("mac");

		if ( ! module)
		{
			module = destStation->getSubmodule("wlan");

			if ( ! module)
			{
				error("module '%s' has no 'mac' submodule", addr.c_str());
			}
			else
			{
				module = module->getSubmodule("mac");

				if ( ! module)
				{
					error("module '%s' has no 'mac' submodule", addr.c_str());
				}
			}
		}

		destMACAddress.setAddress(module->par("address"));
	}

    return destMACAddress;
}

void FiWiTrafficMatrix::handleMessage(cMessage *msg)
{

}



void FiWiTrafficMatrix::finish()
{
}

