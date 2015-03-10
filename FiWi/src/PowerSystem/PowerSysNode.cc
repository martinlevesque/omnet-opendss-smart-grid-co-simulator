/*
 * PowerSysNode.cpp
 *
 *  Created on: Oct 25, 2011
 *      Author: martin
 */

#include "PowerSysNode.h"
#include <string>

using namespace std;

PowerSysNode::PowerSysNode()
{
	// TODO Auto-generated constructor stub
	name = PowerSysNode::defaultNoName();
	baseLoad = 0.0;
	nbPEVs = 0;
	nbPEVsDischarging = 0;
	voltage = 0.0;
	this->pevKwH = 0;
	setPEVStartTime(-1);
	setPEVDischargeStartTime(-1);
}

PowerSysNode::PowerSysNode(const std::string& p_name)
{
	name = p_name;
	baseLoad = 0.0;
	nbPEVs = 0;
	nbPEVsDischarging = 0;
	voltage = 0.0;
	this->pevKwH = 0;
	setPEVStartTime(-1);
	setPEVDischargeStartTime(-1);
}

PowerSysNode::~PowerSysNode()
{

}

string PowerSysNode::getName() const
{
	return name;
}

void PowerSysNode::setName(string p_name)
{
	name = p_name;
}

double PowerSysNode::getPEVLoad() const
{
	// 1 pev = 4 kW
	return (double)nbPEVs * pevKwH;
}

double PowerSysNode::getPEVDischargingLoad() const
{
	return (double)nbPEVsDischarging * pevKwH;
}

double PowerSysNode::getVoltage() const
{
	return voltage;
}

double PowerSysNode::getPEVKwH() const
{
	return this->pevKwH;
}

void PowerSysNode::setPEVKwH(double p_kW)
{
	this->pevKwH = p_kW;
}

void PowerSysNode::setVoltage(double p_voltage)
{
	voltage = p_voltage;
}

double PowerSysNode::getStateOfCharge() const
{
	return stateOfCharge;
}

void PowerSysNode::setStateOfCharge(double p_stateOfCharge)
{
	stateOfCharge = p_stateOfCharge;
}

void PowerSysNode::setPEVStartTime(double time)
{
	pevStartTime = time;
}

double PowerSysNode::getPEVStartTime() const
{
	return pevStartTime;
}

void PowerSysNode::setPEVDischargeStartTime(double time)
{
	pevDischargeStartTime = time;
}

double PowerSysNode::getPEVDischargeStartTime() const
{
	return pevDischargeStartTime;
}
