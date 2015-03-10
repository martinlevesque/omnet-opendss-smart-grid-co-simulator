/*
 * MyUtil.h
 *
 *  Created on: Aug 24, 2011
 *      Author: martin
 */

#include <omnetpp.h>

#include "FiWiRoutingTable.h"
#include "FiWiTrafficMatrix.h"
#include <string>
#include <vector>

#ifndef MYUTIL_H_
#define MYUTIL_H_

class MyUtil
{
public:
	static cModule * findModuleUp(cSimpleModule* thisModule, const char * name, int index);
	static cModule * findModuleUp(cModule* thisModule, const char * name);
	static MACAddress resolveDestMACAddress(cModule* thisModule, const std::string& addr);
	static FiWiRoutingTable* getRoutingTable(cSimpleModule* thisModule);
	static cModule * getNeighbourOnGate(cModule* thisModule, const char * g);
	static cModule * getNeighbourOnGate(cModule* thisModule, const char * g, int index);
	static FiWiTrafficMatrix* getTrafficMatrix(cSimpleModule* thisModule);
	static std::string getSimulationType(cSimpleModule* thisModule);
	static double randomPoisson(double lambda);
	static void myReplace(std::string& str, const std::string& oldStr, const std::string& newStr);
	static std::vector<int> extractIntVect(const std::string& s);
};

#endif /* MYUTIL_H_ */
