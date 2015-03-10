/*
 * MyUtil.cpp
 *
 *  Created on: Aug 24, 2011
 *      Author: martin
 */

#include <omnetpp.h>
#include "MyUtil.h"
#include "FiWiTrafficMatrix.h"
#include "PONUtil.h"
#include <cassert>
#include <vector>

using namespace std;

void MyUtil::myReplace(std::string& str, const std::string& oldStr, const std::string& newStr)
{
  size_t pos = 0;
  while((pos = str.find(oldStr, pos)) != std::string::npos)
  {
     str.replace(pos, oldStr.length(), newStr);
     pos += newStr.length();
  }
}

std::vector<int> MyUtil::extractIntVect(const std::string& s)
{
	vector<int> vs;

	cStringTokenizer tokenizer(s.c_str());
	while (tokenizer.hasMoreTokens())
		vs.push_back(atoi(tokenizer.nextToken()));

	return vs;
}

cModule * MyUtil::findModuleUp(cSimpleModule* thisModule, const char * name, int index)
{
	cModule* mod = NULL;

	for (cModule *curmod=thisModule; !mod && curmod; curmod=curmod->getParentModule())
	     mod = curmod->getSubmodule(name, index);

	return mod;
}

MACAddress MyUtil::resolveDestMACAddress(cModule* thisModule, const std::string& addr)
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
			{
				assert(false);
			}
		}

		EV << "PATATE 1" << endl;

		cModule* module = destStation->getSubmodule("mac");

		EV << "PATATE 2" << endl;

		if ( ! module)
		{
			module = destStation->getSubmodule("wlan");

			EV << "PATATE 3" << endl;

			if ( ! module)
			{
				assert(false);
			}
			else
			{
				module = module->getSubmodule("mac");

				EV << "PATATE 5" << endl;

				if ( ! module)
				{
					assert(false);
				}
			}
		}

		EV << "PATATE 6" << endl;

		destMACAddress.setAddress(module->par("address"));
	}

	return destMACAddress;
}

cModule * MyUtil::findModuleUp(cModule* thisModule, const char * name)
{
	cModule* mod = NULL;

	for (cModule *curmod=thisModule; !mod && curmod; curmod=curmod->getParentModule())
	     mod = curmod->getSubmodule(name);

	return mod;
}

cModule * MyUtil::getNeighbourOnGate(cModule* thisModule, const char * g)
{
	if ( ! thisModule->gate(g))
	{
		return NULL;
	}

	if ( ! thisModule->gate(g)->getNextGate())
	{
		return NULL;
	}

	return thisModule->gate(g)->getNextGate()->getOwnerModule();
}

cModule * MyUtil::getNeighbourOnGate(cModule* thisModule, const char * g, int index)
{
	if ( ! thisModule)
		return NULL;

	if ( ! thisModule->gate(g, index))
	{
		return NULL;
	}

	if ( ! thisModule->gate(g, index)->getNextGate())
		return NULL;

	return thisModule->gate(g, index)->getNextGate()->getOwnerModule();
}

FiWiRoutingTable* MyUtil::getRoutingTable(cSimpleModule* thisModule)
{
	FiWiRoutingTable* table = dynamic_cast<FiWiRoutingTable*>(MyUtil::findModuleUp(thisModule, "fiWiRoutingTable"));

	if ( ! table)
	{
		thisModule->error("MyUtil::getRoutingTable - No fiWiRoutingTable found.");
	}

	return table;
}

FiWiTrafficMatrix* MyUtil::getTrafficMatrix(cSimpleModule* thisModule)
{
	FiWiTrafficMatrix* matrix = dynamic_cast<FiWiTrafficMatrix*>(MyUtil::findModuleUp(thisModule, "fiWiTrafficMatrix"));

	if ( ! matrix)
	{
		thisModule->error("MyUtil::getTrafficMatrix - No fiWiTrafficMatrix found.");
	}

	return matrix;
}

std::string MyUtil::getSimulationType(cSimpleModule* thisModule)
{
	return PONUtil::getFiWiGeneralConfigs(thisModule)->getSimulationType();
}

double MyUtil::randomPoisson(double lambda)
{
	double n;

	if (lambda <= 0)
		return 0;

	while ( (n = poisson((double)lambda)) <= 0)
	{

	}

	return n;
}



