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

#ifndef __FIWI_ROUTING_TABLE_H__
#define __FIWI_ROUTING_TABLE_H__

#include <omnetpp.h>
#include <set>
#include <map>
#include "MACAddress.h"
#include <string>
#include <utility>
#include <vector>
#include "BasicIeee80211Mac.h"

enum FiWiNodeType
{
	FIWI_NODE_TYPE_PON,
	FIWI_NODE_TYPE_WIRELESS,
	FIWI_NODE_TYPE_ANY
};

struct FiWiNode
{
	MACAddress addr;
	FiWiNodeType nodeType;

	FiWiNode(MACAddress p_addr, FiWiNodeType p_nodeType)
	{
		addr = p_addr;
		nodeType = p_nodeType;
	}

	FiWiNode(const FiWiNode& n)
	{
		addr = n.addr;
		nodeType = n.nodeType;
	}

	FiWiNode()
	{

	}

	std::string toString() const
	{
		return addr.str();
	}

	bool operator==(const FiWiNode& other)
	{
		return addr == other.addr;
	}
};

struct FiWiNodeComp
{
  bool operator() (const FiWiNode& n1, const FiWiNode& n2) const
  {
	  return n1.addr < n2.addr;
  }
};

/**
 *
 */
class FiWiRoutingTable : public cSimpleModule
{
  protected:
    virtual void handleMessage(cMessage *msg);

  public:
    void addAdjacent(FiWiNode n1, FiWiNode n2);
    void print();
    void setOltAddr(MACAddress a) { oltAddr = a; }
    MACAddress getOltAddr() { return oltAddr; }
    MACAddress getDMSAddr() { return dmsAddr; }
    void setDMSAddr(MACAddress addr) { dmsAddr = addr; }

    void addNodeInWiZone(int zone, MACAddress node)
    {
    	wiZones[zone].push_back(node);
    }

    int zoneOf(MACAddress node);

    std::pair<bool, FiWiNode> nextHop(MACAddress currentNode, MACAddress dest, short fiwiType);

    bool nodeExists(MACAddress addr);
    FiWiNodeType nodeTypeOf(MACAddress addr);
    int cntNbPONNodesInPath(const std::vector<FiWiNode>& path);
    int cntNbWiCollocatedInPath(const std::vector<FiWiNode>& path);
    int nbHopsWireless(const std::vector<FiWiNode>& path);
    int maxNbNodesInWiZone(const std::vector<FiWiNode>& path);

    void setAreWirelessCollocated(MACAddress a1, MACAddress a2)
	{
		wirelessCollocated[a1][a2] = true;
		wirelessCollocated[a2][a1] = true;
	}

    bool areWirelessCollocated(MACAddress a1, MACAddress a2)
    {
    	return wirelessCollocated[a1][a2];
    }

    std::map<int, std::vector<BasicIeee80211Mac*> > wiMacZones;

  private:

    std::pair<bool, FiWiNode> nextHopWithExplicitRoutingTable(std::map<MACAddress, std::map<MACAddress, MACAddress> >& routing, MACAddress currentNode, MACAddress dest, short fiwiType);

    void fillRoutingTableOf(MACAddress source);
    MACAddress getClosestUnmarkedNode(std::map<MACAddress, int>& distance, std::map<MACAddress, bool>& mark);
    void buildShortestPathTo(std::map<MACAddress, MACAddress>& predecessor, MACAddress source, MACAddress node, std::vector<MACAddress>& path);

    virtual void initialize(int stage);
    virtual int numInitStages() const {return 9;}

    int nbHops(const std::vector<FiWiNode>& path);

    std::map<int, std::vector<MACAddress> > wiZones;


    // src, dest -> nexthop
    std::map<MACAddress, std::map<MACAddress, MACAddress> > routingTable;

    void printRoutes(const std::vector<std::vector<FiWiNode> >& routes);

    void printAdjNodes(const std::set<FiWiNode, FiWiNodeComp>& nodes);

    MACAddress oltAddr;
    MACAddress dmsAddr;

    // Node -> Adjacents
    std::map<FiWiNode, std::set<FiWiNode, FiWiNodeComp>, FiWiNodeComp > nodes;
    std::map<MACAddress, FiWiNodeType> nodeTypes;
    std::map<MACAddress, std::map<MACAddress, bool> > wirelessCollocated;
};

#endif
