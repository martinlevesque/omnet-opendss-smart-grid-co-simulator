/*
 * OpenDSS.cpp
 *
 *  Created on: Oct 25, 2011
 *      Author: martin
 */

#include <string>
#include "OpenDSS.h"
#include "PowerSysNode.h"
#include <map>
#include <sstream>
#include <stdlib.h>
#include <locale>
#include <fstream>
#include "MACAddress.h"
#include "MyUtil.h"

using namespace std;

double PowerSystemKnowledge::totalBaseLoad()
{
	double total = 0;

	for (std::map<std::string, PowerSysNode>::iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		total += it->second.getBaseLoad();
	}

	return total;
}

OpenDSS::OpenDSS()
{
	PowerSysTotalLosses = RawStat("PowerSysTotalLosses", "event", 1);
	PowerSysTotalLossesVar = RawStat("PowerSysTotalLossesVar", "event", 1);
	PowerSysTotalLineLosses = RawStat("PowerSysTotalLineLosses", "event", 1);
	PowerSysTotalLineLossesVar = RawStat("PowerSysTotalLineLossesVar", "event", 1);
	PowerSysTotalLoad = RawStat("PowerSysTotalLoad", "event", 1);
	PowerSysTotalLoadVar = RawStat("PowerSysTotalLoadVar", "event", 1);
	PowerSysTotalPower = RawStat("PowerSysTotalPower", "event", 1);
	PowerSysTotalPowerVar = RawStat("PowerSysTotalPowerVar", "event", 1);
	ChargingStationVoltage = RawStat("ChargingStationVoltage", "event", 1);
	ChargingStationLoad = RawStat("ChargingStationLoad", "event", 1);
	V2GLoad = RawStat("V2GLoad", "event", 1);
	SolarLoad = RawStat("SolarLoad", "event", 1);
}

void OpenDSS::updatePowerSystemStats(double time)
{
	PowerSysTotalLosses.addStat(simTime().dbl(), getTotalPowerLosses(), 0);
	PowerSysTotalLossesVar.addStat(simTime().dbl(), getTotalPowerLossesVar(), 0);
	PowerSysTotalLineLosses.addStat(simTime().dbl(), getTotalPowerLineLosses(), 0);
	PowerSysTotalLineLossesVar.addStat(simTime().dbl(), getTotalPowerLineLossesVar(), 0);
	PowerSysTotalLoad.addStat(simTime().dbl(), getTotalPowerLoad(), 0);
	PowerSysTotalLoadVar.addStat(simTime().dbl(), getTotalPowerLoadVar(), 0);
	PowerSysTotalPower.addStat(simTime().dbl(), getTotalPower(), 0);
	PowerSysTotalPowerVar.addStat(simTime().dbl(), getTotalPowerVar(), 0);
	V2GLoad.addStat(simTime().dbl(), totalV2GLoad(), 0);

	history[time] = this->infosGrid;

	// Charging station statistics:
	for (std::map<std::string, PowerSysNode>::iterator it = infosGrid.chargingStationNodes.begin();
			it != infosGrid.chargingStationNodes.end(); ++it)
	{
		// Charging station stats:
		ChargingStationVoltage.log(simTime().dbl(), it->second.getVoltage(), it->first);
		ChargingStationLoad.log(simTime().dbl(), it->second.getPEVLoad(), it->first);

		// Charging station solar load:
		SolarLoad.log(simTime().dbl(), SolarLoadAtChargingStation(it->first, simTime().dbl()), it->first);
	}
}

double OpenDSS::SolarLoadAtChargingStation(const std::string& nodeId, double time)
{
	double solarLoad = 0;

	// If solar panel, take load in the distribution model based on hour and node:
	if (OpenDSS::Instance().withChargingStationSolarPanels)
	{
		int hour = (int)(((time - 2) / OpenDSS::Instance().NbSecondsPerHour)) % 24;

		solarLoad = OpenDSS::Instance().solarDistributionAt[nodeId][hour];
	}

	if (solarLoad <= 0)
		solarLoad = 0.00000000001;

	return solarLoad;
}

OpenDSS& OpenDSS::Instance()
{
	static OpenDSS s;

	return s;
}

double OpenDSS::totalV2GLoad()
{
	double load = 0.0;

	for (std::map<std::string, MACAddress>::iterator it = powerNodeToMacAddress.begin(); it != powerNodeToMacAddress.end(); ++it)
	{
		load += OpenDSS::Instance().nbPEVsDischargingAt(it->first) * OpenDSS::Instance().PEVkWPerHour(it->first);
	}

	return load;
}

int OpenDSS::totalPEVsCharging()
{
	int nb = 0;

	for (std::map<std::string, MACAddress>::iterator it = powerNodeToMacAddress.begin(); it != powerNodeToMacAddress.end(); ++it)
	{
		nb += this->nbPEVsAt(it->first);
	}

	for (std::map<std::string, PowerSysNode>::iterator it = infosGrid.chargingStationNodes.begin(); it != infosGrid.chargingStationNodes.end(); ++it)
	{
		nb += it->second.getNbPEVs();
	}

	return nb;
}

void OpenDSS::rawBaseLoadUpdate(const string& p_nodeName, double p_load)
{
	infosGrid.nodes[p_nodeName].setBaseLoad(p_load);
	infosGrid.nodes[p_nodeName].setName(p_nodeName);
}

double OpenDSS::currentBaseLoadAt(const string& p_nodeName)
{
	return infosGrid.nodes[p_nodeName].getBaseLoad();
}

void OpenDSS::incrementNbPEVsAt(const std::string& nodeName)
{
	infosGrid.nodes[nodeName].setNbPEVs(infosGrid.nodes[nodeName].getNbPEVs() + 1);
}

void OpenDSS::setNbPEVsAt(const std::string& nodeName, int nb)
{
	infosGrid.nodes[nodeName].setNbPEVs(nb);
}

void OpenDSS::decrementNbPEVsAt(const std::string& nodeName)
{
	infosGrid.nodes[nodeName].setNbPEVs(infosGrid.nodes[nodeName].getNbPEVs() - 1);
}

void OpenDSS::setNbPEVsAtChargingStation(const std::string& nodeName, int nb)
{
	infosGrid.chargingStationNodes[nodeName].setNbPEVs(nb);
}

int OpenDSS::nbPEVsAtChargingStation(const std::string& nodeName)
{
	return infosGrid.chargingStationNodes[nodeName].getNbPEVs();
}

double OpenDSS::getStateOfChargeOf(const std::string& nodeName)
{
	return infosGrid.nodes[nodeName].getStateOfCharge();
}

void OpenDSS::setStateOfChargeOf(const std::string& nodeName, double soc)
{
	infosGrid.nodes[nodeName].setStateOfCharge(soc);
}

void OpenDSS::setPEVDischargeStartTime(const std::string& nodeId, double time)
{
	infosGrid.nodes[nodeId].setPEVDischargeStartTime(time);
}

double OpenDSS::getPEVDischargeStartTime(const std::string& nodeId)
{
	return infosGrid.nodes[nodeId].getPEVDischargeStartTime();
}

void OpenDSS::setPEVStartTime(const std::string& nodeId, double time)
{
	infosGrid.nodes[nodeId].setPEVStartTime(time);
}

double OpenDSS::getPEVStartTime(const std::string& nodeId)
{
	return infosGrid.nodes[nodeId].getPEVStartTime();
}

int OpenDSS::nbPEVsAt(const std::string& nodeName)
{
	return infosGrid.nodes[nodeName].getNbPEVs();
}

int OpenDSS::nbPEVsDischargingAt(const std::string& nodeName)
{
	return infosGrid.nodes[nodeName].getNbPEVsDischarging();
}

void OpenDSS::setNbPEVsDischargingAt(const std::string& nodeName, int nb)
{
	infosGrid.nodes[nodeName].setNbPEVsDischarging(nb);
}

void OpenDSS::setPEVBatteryKwHPerHour(const std::string& nodeName, double kwhPerHour)
{
	infosGrid.nodes[nodeName].setPEVKwH(kwhPerHour);
}

void OpenDSS::setPEVBatteryKwHPerHourChargingStation(const std::string& nodeName, double kwhPerHour)
{
	infosGrid.chargingStationNodes[nodeName].setPEVKwH(kwhPerHour);
}

bool OpenDSS::isChargingStationNode(const std::string& nodeId)
{
	for (std::map<std::string, PowerSysNode>::iterator it = infosGrid.chargingStationNodes.begin(); it != infosGrid.chargingStationNodes.end(); ++it)
	{
		if (nodeId == it->first)
			return true;
	}

	return false;
}

void OpenDSS::recalculateWithOpenDSSParam(PowerSystemKnowledge& infos)
{
	static RawStat ss = RawStat("NbOpenDSSCalls", "time-sum", 1);
	ss.addStat(simTime().dbl(), 1, 0);

	// Calls open DSS

	string parameters = genParametersStr(infos);

	string chargingStationParameters = "";

	// Loop through the charging station nodes:
	for (std::map<std::string, PowerSysNode>::iterator it = OpenDSS::Instance().infosGrid.chargingStationNodes.begin(); it != OpenDSS::Instance().infosGrid.chargingStationNodes.end(); ++it)
	{
		double load = it->second.getPEVLoad();
		stringstream tmp (stringstream::in | stringstream::out);

		string nodeId = it->first;

		//////////////////////////
		// Charging station load:
		tmp << "n" << nodeId;
		tmp << "=";
		tmp << load;
		tmp << "&";

		chargingStationParameters += tmp.str();

		////////////////////////
		// Solar panel load:
		tmp.clear();

		double solarLoad = SolarLoadAtChargingStation(nodeId, simTime().dbl());

		tmp << "s" << nodeId;
		tmp << "=";
		tmp << solarLoad;
		tmp << "&";

		chargingStationParameters += tmp.str();
	}

	//static RawStat s = RawStat("chargingStationParams", "event", 1);
	//s.log(simTime().dbl(), 0, chargingStationParameters);

	locale loc;                 // the "C" locale
	const collate<char>& coll = use_facet<collate<char> >(loc);

	string hashParams = parameters + "type=" + OpenDSS::Instance().openDSSConfiguration + chargingStationParameters;

	long valHash = coll.hash(hashParams.data(), hashParams.data() + hashParams.length());

	stringstream fileCache (stringstream::in | stringstream::out);
	fileCache << string("confsPEVResidentialOnly/opendss-cache/");
	fileCache << valHash;

	ifstream resFile(fileCache.str().c_str());
	string fileWithResult = "";

	if (resFile)
	{
		// in cache !
		resFile.close();
		fileWithResult = fileCache.str();
	}
	else
	{
		string command = string("wget -p -O /tmp/res_open_dss.txt --post-data '") + parameters +
				string("' \"http://opendss-calculator.com/my/run_opendss.php?type=") +
				OpenDSS::Instance().openDSSConfiguration + string("&") + chargingStationParameters +
				string("\"");

		//static RawStat s2 = RawStat("commandOpenDSS", "event", 1);

		//s2.log(simTime().dbl(), 0, command);

		system(command.c_str());

		ifstream fileCalc("/tmp/res_open_dss.txt");
		string line;

		ofstream outCache;
		outCache.open (fileCache.str().c_str());


		while (getline(fileCalc, line))
		{
			outCache << line << "\n";
		}

		outCache.close();
		fileCalc.close();

		fileWithResult = "/tmp/res_open_dss.txt";
	}

	ifstream streamResult(fileWithResult.c_str());

	streamResult >> infos.totalLosses;
	streamResult >> infos.totalLossesVar;
	streamResult >> infos.totalLineLosses;
	streamResult >> infos.totalLineLossesVar;
	streamResult >> infos.totalLoad;
	streamResult >> infos.totalLoadVar;
	streamResult >> infos.totalPower;
	streamResult >> infos.totalPowerVar;

	if (OpenDSS::Instance().openDSSConfiguration == "homeAndChargingStation" ||
			OpenDSS::Instance().openDSSConfiguration == "homeChargingStationAndV2G" ||
			OpenDSS::Instance().openDSSConfiguration == "homeChargingStationAndV2GAndRES")
	{
		// In this case, we need to read two charging station nodes!
		string node1, node2;
		double voltage1, voltage2;
		streamResult >> node1;
		streamResult >> voltage1;
		streamResult >> node2;
		streamResult >> voltage2;
		infos.chargingStationNodes[node1].setVoltage(voltage1);
		infos.chargingStationNodes[node2].setVoltage(voltage2);
	}

	while (streamResult.good())
	{
		string nodeId;
		double voltage;

		streamResult >> nodeId;
		streamResult >> voltage;

		infos.nodes[nodeId].setVoltage(voltage);
	}

	streamResult.close();
}



string OpenDSS::genParametersStr(const PowerSystemKnowledge& infos)
{
	stringstream res (stringstream::in | stringstream::out);

	for (map<string, PowerSysNode>::const_iterator iter = infos.nodes.begin(); iter != infos.nodes.end(); ++iter )
	{
		if (iter->second.getName() == PowerSysNode::defaultNoName() || isChargingStationNode(iter->second.getName()))
			continue;

		// Network load:
		// divided by 2, ratio where 2 is the maximum.
		res << iter->second.getName() << "=" << iter->second.getBaseLoad() / 2.0 << "&";

		// PEV Load:
		res << iter->second.getName() << "_EV=" << iter->second.getPEVLoad() << "&";


		if (OpenDSS::Instance().openDSSConfiguration == "homeChargingStationAndV2G" ||
				OpenDSS::Instance().openDSSConfiguration == "homeChargingStationAndV2GAndRES")
		{
			double dischargeLoad = iter->second.getNbPEVsDischarging() == 1 ? iter->second.getPEVDischargingLoad() : 0.00000000001;

			if (dischargeLoad <= 0)
				dischargeLoad = 0.00000000001;

			res << iter->second.getName() << "_DIS=" << dischargeLoad << "&";
		}
	}

	return res.str();
}

void OpenDSS::recalculateWithOpenDSS()
{
	recalculateWithOpenDSSParam(infosGrid);
}

string OpenDSS::printNodes()
{
	stringstream res (stringstream::in | stringstream::out);

	for (map<string, PowerSysNode>::iterator iter = infosGrid.nodes.begin(); iter != infosGrid.nodes.end(); ++iter )
	{
		res << iter->second.getName() << " load = " << iter->second.getBaseLoad() << " voltage = " << iter->second.getVoltage() << endl;
	}

	return res.str();
}

void OpenDSS::setMapping(MACAddress addr, std::string powerNodeId)
{
	macAddressToPowerNode[addr] = powerNodeId;
	powerNodeToMacAddress[powerNodeId] = addr;
}

string OpenDSS::getPowerNodeFromMACAddress(MACAddress addr)
{
	return macAddressToPowerNode[addr];
}

MACAddress OpenDSS::getMACAddressFromPowerNode(std::string nodeId)
{
	return powerNodeToMacAddress[nodeId];
}

double OpenDSS::getCurrentVoltageOf(MACAddress addr)
{
	string nodeId = getPowerNodeFromMACAddress(addr);

	return infosGrid.nodes[nodeId].getVoltage();
}

double OpenDSS::getCurrentLoadOf(MACAddress addr)
{
	string nodeId = getPowerNodeFromMACAddress(addr);

	return infosGrid.nodes[nodeId].getBaseLoad() + infosGrid.nodes[nodeId].getPEVLoad();
}

double OpenDSS::getTotalPowerLosses()
{
	return infosGrid.totalLosses;
}

double OpenDSS::getTotalPowerLossesVar()
{
	return infosGrid.totalLossesVar;
}

double OpenDSS::getTotalPowerLineLosses()
{
	return infosGrid.totalLineLosses;
}

double OpenDSS::getTotalPowerLineLossesVar()
{
	return infosGrid.totalLineLossesVar;
}

double OpenDSS::getTotalPowerLoad()
{
	return infosGrid.totalLoad;
}

double OpenDSS::getTotalPowerLoadVar()
{
	return infosGrid.totalLoadVar;
}

double OpenDSS::getTotalPower()
{
	return infosGrid.totalPower;
}

double OpenDSS::getTotalPowerVar()
{
	return infosGrid.totalPowerVar;
}

std::string OpenDSS::LVNameOf(const std::string& nodeName)
{
	string nameCopy = nodeName;

	for (int i = 19; i >= 1; --i)
	{
		char tmp[100];
		sprintf(tmp, "_%d", i);
		string sTmp = tmp;

		MyUtil::myReplace(nameCopy, sTmp, "");
	}

	return nameCopy;
}



std::set<std::string> OpenDSS::getLVs()
{
	set<string> lvs;

	for (map<std::string, MACAddress>::iterator it = powerNodeToMacAddress.begin(); it != powerNodeToMacAddress.end(); ++it)
	{
		string nodeId = it->first;

		string lvName = LVNameOf(nodeId);

		lvs.insert(lvName);
	}

	return lvs;
}

double OpenDSS::randPEVArrivalHome(double minTime, double secondsPerHour)
{
	int maxIndex = 0;

	for (int i = 0; i < (int)arrivalsHome.size(); ++i)
	{
		if (arrivalsHome[i].maxIndex > maxIndex)
			maxIndex = arrivalsHome[i].maxIndex;
	}

	int resMinTime = 0;

	int cnt = 0;

	do
	{
		double randIndex = uniform(0, maxIndex);


		for (int i = 0; i < (int)arrivalsHome.size(); ++i)
		{
			if (randIndex >= arrivalsHome[i].minIndex && randIndex <= arrivalsHome[i].maxIndex)
			{
				resMinTime = 2 + arrivalsHome[i].hour * secondsPerHour;
				break;
			}
		}

		if (++cnt >= 50)
			return 0;
	}
	while(resMinTime <= minTime);

	int resMaxTime = resMinTime + secondsPerHour;

	return uniform(resMinTime, resMaxTime);
}

double OpenDSS::randPEVDeparture(double minTime, double secondsPerHour)
{
	int maxIndex = 0;

	for (int i = 0; i < (int)pevDepartures.size(); ++i)
	{
		if (pevDepartures[i].maxIndex > maxIndex)
			maxIndex = pevDepartures[i].maxIndex;
	}

	int resMinTime = 0;

	int cnt = 0;

	do
	{
		double randIndex = uniform(0, maxIndex);

		for (int i = 0; i < (int)pevDepartures.size(); ++i)
		{
			if (randIndex >= pevDepartures[i].minIndex && randIndex <= pevDepartures[i].maxIndex)
			{
				resMinTime = 2 + pevDepartures[i].hour * secondsPerHour;
				break;
			}
		}

		if (++cnt >= 50)
			return 0;
	}
	while(resMinTime <= minTime);

	int resMaxTime = resMinTime + secondsPerHour;

	return uniform(resMinTime, resMaxTime);
}

double OpenDSS::randPEVDistanceToDo()
{
	int maxIndex = 0;

	for (int i = 0; i < (int)pevDistanceDistribution.size(); ++i)
	{
		if (pevDistanceDistribution[i].maxIndex > maxIndex)
			maxIndex = pevDistanceDistribution[i].maxIndex;
	}

	int resMinDistance = 0;

	double randIndex = uniform(0, maxIndex);

	for (int i = 0; i < (int)pevDistanceDistribution.size(); ++i)
	{
		if (randIndex >= pevDistanceDistribution[i].minIndex && randIndex <= pevDistanceDistribution[i].maxIndex)
		{
			resMinDistance = pevDistanceDistribution[i].distanceMin;
			break;
		}
	}

	int resMaxDistance = resMinDistance + 5; // 5 miles



	return uniform(resMinDistance, resMaxDistance);
}

//
/*
void OpenDSS::testWithOpenDSS(const string& nodeName, )
{
}
*/
