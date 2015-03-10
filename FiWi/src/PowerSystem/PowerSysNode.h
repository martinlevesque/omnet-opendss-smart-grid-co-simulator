/*
 * PowerSysNode.h
 *
 *  Created on: Oct 25, 2011
 *      Author: martin
 */

#include <string>

#ifndef POWERSYSNODE_H_
#define POWERSYSNODE_H_

class PowerSysNode
{
public:
	PowerSysNode();
	PowerSysNode(const std::string& p_name);
	virtual ~PowerSysNode();

	void setBaseLoad(double p_baseLoad) { baseLoad = p_baseLoad; }
	double getBaseLoad() const { return baseLoad; }

	double getPEVLoad() const;

	double getPEVDischargingLoad() const;

	void setNbPEVs(int p_nbPEVs) { nbPEVs = p_nbPEVs; }
	int getNbPEVs() const { return nbPEVs; }

	void setNbPEVsDischarging(int p_nbPEVs) { nbPEVsDischarging = p_nbPEVs; }
	int getNbPEVsDischarging() const { return nbPEVsDischarging; }

	std::string getName() const;
	void setName(std::string p_name);

	double getVoltage() const;
	void setVoltage(double p_voltage);

	double getPEVKwH() const;
	void setPEVKwH(double p_kW);

	double getStateOfCharge() const;
	void setStateOfCharge(double p_stateOfCharge);

	double getPEVStartTime() const;
	void setPEVStartTime(double time);

	double getPEVDischargeStartTime() const;
	void setPEVDischargeStartTime(double time);

	static std::string defaultNoName() { return "no name ?"; }

private:
	std::string name;
	double baseLoad;
	int nbPEVs;
	int nbPEVsDischarging;
	double voltage;
	double pevKwH;
	double stateOfCharge;
	double pevStartTime;
	double pevDischargeStartTime;
};

#endif /* POWERSYSNODE_H_ */
