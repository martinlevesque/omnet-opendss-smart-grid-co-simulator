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

#include "FiWiRoutingTable.h"
#include <algorithm>
#include <vector>
#include <utility>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include "MyUtil.h"
#include "PONUtil.h"
#include "RawStat.h"
#include <string>

using namespace std;

Define_Module(FiWiRoutingTable);

void FiWiRoutingTable::initialize(int stage)
{
	if (stage == 7)
	{
		string customRouting = par("customRoutingFile").stringValue();

		for (std::map<FiWiNode, std::set<FiWiNode, FiWiNodeComp>, FiWiNodeComp >::iterator i = nodes.begin(); i != nodes.end(); ++i)
		{
			fillRoutingTableOf(i->first.addr);
		}

		if (customRouting == "")
		{

		}
		else
		{
			ifstream config;

			// Take a copy:
			std::map<MACAddress, std::map<MACAddress, MACAddress> > routingCopy = this->routingTable;

			// Clear old one:
			routingTable.clear();

			config.open(customRouting.c_str());

			if (config.is_open())
			{

				while ( ! config.eof())
				{
					string source;
					string dest;
					string nextHop;

					config >> source;
					config >> dest;
					config >> nextHop;



					pair<bool, FiWiNode> res;

					MACAddress cur = MyUtil::resolveDestMACAddress(this, source);

					EV << "STARTING !" << endl;

					do
					{
						res = nextHopWithExplicitRoutingTable(routingCopy, cur, MyUtil::resolveDestMACAddress(this, nextHop), FIWI_NODE_TYPE_ANY);

						if (res.first)
						{
							routingTable[cur][MyUtil::resolveDestMACAddress(this, dest)] = res.second.addr;
						}

						if (res.first)
							EV << "routing .. source = " << source << " dest = " << dest << " nexthop = " << nextHop << " real addr next hop in sim = " << res.second.toString() << " type = " << res.second.nodeType << endl;
						else
							EV << "routing .. source = " << source << " dest = " << dest << " nexthop = " << nextHop << " NOT FOUND "  << endl;

						cur = res.second.addr;
					}
					while (res.first && res.second.addr != MyUtil::resolveDestMACAddress(this, nextHop));
				}
			}

			config.close();
		}
	}
}

int FiWiRoutingTable::zoneOf(MACAddress node)
{
	for (std::map<int, std::vector<MACAddress> >::iterator it = wiZones.begin(); it != wiZones.end(); ++it)
	{
		for (int i = 0; i < (int)it->second.size(); ++i)
		{
			if (it->second[i] == node)
			{
				return it->first;
			}
		}
	}

	return -1;
}

void FiWiRoutingTable::buildShortestPathTo(map<MACAddress, MACAddress>& predecessor, MACAddress source, MACAddress node, std::vector<MACAddress>& path)
{
	if (node == source)
		path.push_back(node);
	else if(predecessor[node] == MACAddress::UNSPECIFIED_ADDRESS)
		EV << "No path from " << source << " to " << node << endl;
	else
	{
		buildShortestPathTo(predecessor, source, predecessor[node], path);

		path.push_back(node);
	}
}

MACAddress FiWiRoutingTable::getClosestUnmarkedNode(map<MACAddress, int>& distance, map<MACAddress, bool>& mark)
{
    int minDistance = 1000000;

    MACAddress closestUnmarkedNode;

    for (std::map<FiWiNode, std::set<FiWiNode, FiWiNodeComp>, FiWiNodeComp >::iterator n = nodes.begin(); n != nodes.end(); ++n)
    {
        if (( ! mark[n->first.addr]) && ( minDistance >= distance[n->first.addr]))
        {
            minDistance = distance[n->first.addr];
            closestUnmarkedNode = n->first.addr;
        }
    }

    return closestUnmarkedNode;
}

void FiWiRoutingTable::fillRoutingTableOf(MACAddress source)
{
	EV << "FiWiRoutingTable::fillRoutingTableOf OF " << source << endl;

	// Dijkstra
	map<MACAddress, map<MACAddress, int> > adjMatrix;

	for (std::map<FiWiNode, std::set<FiWiNode, FiWiNodeComp>, FiWiNodeComp >::iterator i = nodes.begin(); i != nodes.end(); ++i)
		for (std::map<FiWiNode, std::set<FiWiNode, FiWiNodeComp>, FiWiNodeComp >::iterator j = nodes.begin(); j != nodes.end(); ++j)
			adjMatrix[i->first.addr][j->first.addr] = 1000000;

	for (std::map<FiWiNode, std::set<FiWiNode, FiWiNodeComp>, FiWiNodeComp >::iterator i = nodes.begin(); i != nodes.end(); ++i)
	{
		for (std::set<FiWiNode, FiWiNodeComp>::iterator adj = i->second.begin(); adj != i->second.end(); ++adj)
		{
			int n = 0;

			if (i->first.addr == adj->addr)
			{
				adjMatrix[i->first.addr][adj->addr] = 0;
				continue;
			}

			if (FiWiRoutingTable::nodeTypeOf(i->first.addr) == FIWI_NODE_TYPE_PON && FiWiRoutingTable::nodeTypeOf(adj->addr) == FIWI_NODE_TYPE_PON)
			{
				n = 1;
			}
			else
			{
				vector<FiWiNode> path;
				path.push_back(i->first);
				path.push_back(*adj);

				n = nbHopsWireless(path);

				if (n > 1)
					n = 1;
			}

			adjMatrix[i->first.addr][adj->addr] = n;
		}
	}

	map<MACAddress, MACAddress> predecessor;
	map<MACAddress, int> distance;
	map<MACAddress, bool> mark;

	// INITIALIZE
	for (std::map<FiWiNode, std::set<FiWiNode, FiWiNodeComp>, FiWiNodeComp >::iterator i = nodes.begin(); i != nodes.end(); ++i)
	{
		mark[i->first.addr] = false;

		predecessor[i->first.addr] = MACAddress::UNSPECIFIED_ADDRESS;

		distance[i->first.addr] = 1000000;
	}

	distance[source] = 0;

	for (std::map<FiWiNode, std::set<FiWiNode, FiWiNodeComp>, FiWiNodeComp >::iterator n = nodes.begin(); n != nodes.end(); ++n)
	{
		MACAddress closestUnmarkedNode = getClosestUnmarkedNode(distance, mark);

		mark[closestUnmarkedNode] = true;

		for (std::set<FiWiNode, FiWiNodeComp>::iterator i = nodes[FiWiNode(closestUnmarkedNode, FIWI_NODE_TYPE_ANY)].begin();
				i != nodes[FiWiNode(closestUnmarkedNode, FIWI_NODE_TYPE_ANY)].end();
				++i)
		{
			if( ! mark[i->addr])
			{
				if (distance[i->addr] > distance[closestUnmarkedNode] + adjMatrix[closestUnmarkedNode][i->addr])
				{
					distance[i->addr] = distance[closestUnmarkedNode] + adjMatrix[closestUnmarkedNode][i->addr];

					predecessor[i->addr] = closestUnmarkedNode;
				}
			}
		}
	}

	for (std::map<FiWiNode, std::set<FiWiNode, FiWiNodeComp>, FiWiNodeComp >::iterator n = nodes.begin(); n != nodes.end(); ++n)
	{
		vector<MACAddress> path;

		buildShortestPathTo(predecessor, source, n->first.addr, path);

		if (path.size() >= 2)
		{
			MACAddress nextHop = path[1];
			EV << "NEXT HOP of " << source << " to " << n->first.addr << " is " << nextHop << endl;
			routingTable[source][n->first.addr] = nextHop;
		}
	}
}

void FiWiRoutingTable::handleMessage(cMessage *msg)
{
	delete msg;
}

void FiWiRoutingTable::addAdjacent(FiWiNode n1, FiWiNode n2)
{
	nodes[n1].insert(n2);
	nodes[n2].insert(n1);

	nodeTypes[n1.addr] = n1.nodeType;
	nodeTypes[n2.addr] = n2.nodeType;
}

bool FiWiRoutingTable::nodeExists(MACAddress addr)
{
	EV << "INSIDE FiWiRoutingTable::nodeExists" << endl;

	this->print();

	// If there is no connection, erf !
	return nodes[FiWiNode(addr, FIWI_NODE_TYPE_ANY)].size() > 0 ||
			nodes[FiWiNode(addr, FIWI_NODE_TYPE_PON)].size() > 0 ||
			nodes[FiWiNode(addr, FIWI_NODE_TYPE_WIRELESS)].size() > 0;
}

FiWiNodeType FiWiRoutingTable::nodeTypeOf(MACAddress addr)
{
	return nodeTypes[addr];
}

int FiWiRoutingTable::cntNbPONNodesInPath(const vector<FiWiNode>& path)
{
	int cnt = 0;

	for (int i = 0; i < (int)path.size(); ++i)
	{
		if (FiWiRoutingTable::nodeTypeOf(path[i].addr) == FIWI_NODE_TYPE_PON)
			++cnt;
	}

	return cnt;
}

int FiWiRoutingTable::cntNbWiCollocatedInPath(const vector<FiWiNode>& path)
{
	int cnt = 0;

	for (int i = 1; i < (int)path.size(); ++i)
	{
		if (areWirelessCollocated(path[i-1].addr, path[i].addr))
			++cnt;
	}

	return cnt;
}

int FiWiRoutingTable::maxNbNodesInWiZone(const vector<FiWiNode>& path)
{
	int max = 0;

	for (std::map<int, std::vector<MACAddress> >::iterator it = wiZones.begin(); it != wiZones.end(); ++it)
	{
		int curNb = 0;

		for (int i = 0; i < (int)it->second.size(); ++i)
		{
			for (int p = 0; p < (int)path.size(); ++p)
			{
				if (it->second[i] == path[p].addr)
				{
					++curNb;
				}
			}
		}

		if (curNb > max)
		{
			max = curNb;
		}
	}

	return max;
}

void FiWiRoutingTable::printRoutes(const vector<vector<FiWiNode> >& routes)
{
	EV << "nb routes.. = " << routes.size() << endl;

	for (int i = 0; i < (int)routes.size(); ++i)
	{
		for (int j = 0; j < (int)routes[i].size(); ++j)
		{
			EV << routes[i][j].addr << " ";
		}

		EV << endl;
	}
}

void FiWiRoutingTable::printAdjNodes(const std::set<FiWiNode, FiWiNodeComp>& nodes)
{
	EV << "Nb adj nodes = " << nodes.size() << endl;

	for (std::set<FiWiNode, FiWiNodeComp>::iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		EV << it->addr << " ";
	}

	EV << endl;
}

int FiWiRoutingTable::nbHops(const vector<FiWiNode>& path)
{
	int hopsPON = cntNbPONNodesInPath(path);

	if (hopsPON >= 2)
		hopsPON = 2;

	return nbHopsWireless(path) + hopsPON;
}

int FiWiRoutingTable::nbHopsWireless(const vector<FiWiNode>& path)
{
	int nb = 0;

	if (path.size() == 1 && nodeTypeOf(path[0].addr) == FIWI_NODE_TYPE_WIRELESS)
		return 1;

	for (int i = 0; i < (int)path.size(); ++i)
	{
		if (i > 0)
		{
			EV << "  " << path[i-1].addr << "(" << nodeTypeOf(path[i-1].addr) << ") -> " << path[i].addr << "(" << nodeTypeOf(path[i].addr) << ")" << endl;

			// If they are wi or ONU-MPP collocated, don't count it!
			if (areWirelessCollocated(path[i - 1].addr, path[i].addr) || nodeTypeOf(path[i - 1].addr) != FIWI_NODE_TYPE_WIRELESS)
			{
				continue;
			}

			++nb;
		}
	}

	return nb;
}

pair<bool, FiWiNode> FiWiRoutingTable::nextHopWithExplicitRoutingTable(std::map<MACAddress, std::map<MACAddress, MACAddress> >& routing, MACAddress currentNode, MACAddress dest, short fiwiType)
{

	pair<bool, FiWiNode> result;

	result.first = false;

	MACAddress nextHop = routing[currentNode][dest];

	if ( ! nextHop.isUnspecified())
	{
		result.first = true;
		result.second = FiWiNode(nextHop, FIWI_NODE_TYPE_ANY);
		result.second.nodeType = nodeTypeOf(nextHop);
	}

	return result;
}

pair<bool, FiWiNode> FiWiRoutingTable::nextHop(MACAddress currentNode, MACAddress dest, short fiwiType)
{
	return nextHopWithExplicitRoutingTable(this->routingTable, currentNode, dest, fiwiType);
}

void FiWiRoutingTable::print()
{
	EV << "FIWI ROUTING TABLE DUMP" << endl;

	for (std::map<FiWiNode, std::set<FiWiNode, FiWiNodeComp>, FiWiNodeComp >::iterator it = nodes.begin();
			it != nodes.end(); ++it)
	{
		for (std::set<FiWiNode, FiWiNodeComp>::iterator itAdjs = it->second.begin(); itAdjs != it->second.end(); ++itAdjs)
		{
			EV << "		" << it->first.toString() << " (" << nodeTypeOf((*it).first.addr) << ") -> " << itAdjs->toString() << "(" << nodeTypeOf((*itAdjs).addr) << ")" << endl;
		}
	}
}

/////////////////////////////////
/*



int Dijkstra::getClosestUnmarkedNode(){

    int minDistance = INFINITY;

    int closestUnmarkedNode;

    for(int i=0;i<numOfVertices;i++) {

        if((!mark[i]) && ( minDistance >= distance[i])) {

            minDistance = distance[i];

            closestUnmarkedNode = i;

        }

    }

    return closestUnmarkedNode;

}


void Dijkstra::calculateDistance(){

    initialize();

    int minDistance = INFINITY;

    int closestUnmarkedNode;

    int count = 0;

    while(count < numOfVertices) {

        closestUnmarkedNode = getClosestUnmarkedNode();

        mark[closestUnmarkedNode] = true;

        for(int i=0;i<numOfVertices;i++) {

            if((!mark[i]) && (adjMatrix[closestUnmarkedNode][i]>0) ) {

                if(distance[i] > distance[closestUnmarkedNode]+adjMatrix[closestUnmarkedNode][i]) {

                    distance[i] = distance[closestUnmarkedNode]+adjMatrix[closestUnmarkedNode][i];

                    predecessor[i] = closestUnmarkedNode;

                }

            }

        }

        count++;

    }

}


void Dijkstra::printPath(int node){

    if(node == source)

        cout<<(char)(node + 97)<<"..";

    else if(predecessor[node] == -1)

        cout<<"No path from “<<source<<”to "<<(char)(node + 97)<<endl;

    else {

        printPath(predecessor[node]);

        cout<<(char) (node + 97)<<"..";

    }

}


void Dijkstra::output(){

    for(int i=0;i<numOfVertices;i++) {

        if(i == source)

            cout<<(char)(source + 97)<<".."<<source;

        else

            printPath(i);

        cout<<"->"<<distance[i]<<endl;

    }

}


int main(){

    Dijkstra G;

    G.read();

    G.calculateDistance();

    G.output();

    return 0;

}

*/

