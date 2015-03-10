//
// Copyright (C) 2008 Alfonso Ariza
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//


#include <omnetpp.h>
#include <stdlib.h>
#include <string.h>
#include "InterfaceEntry.h"
#include "IPRoute.h"
#include "ManetManager.h"
#include "RoutingTableAccess.h"
#include "InterfaceTableAccess.h"
#include "IRoutingTable.h"
#include "IPv4InterfaceData.h"
#include "ControlManetRouting_m.h"
#include "IPDatagram.h"



Define_Module(ManetManager);
Define_Module(ManetManagerStatic);


void ManetManager::initialize(int stage)
{
    bool manetPurgeRoutingTables=false;
    if (stage==4)
    {
        manetActive = (bool) par("manetActive");
        routingProtocol = par("routingProtocol").stringValue ();
        if (manetActive)
        {
            manetPurgeRoutingTables = (bool) par("manetPurgeRoutingTables");
            if (manetPurgeRoutingTables)
            {
                IRoutingTable *rt = RoutingTableAccess ().get ();
                const IPRoute *entry;
                // clean the route table wlan interface entry
                for (int i=rt->getNumRoutes()-1; i>=0; i--)
                {
                    entry= rt->getRoute(i);
                    const InterfaceEntry *ie = entry->getInterface();
                    if (strstr (ie->getName(),"wlan")!=NULL)
                    {
                        rt->deleteRoute(entry);
                    }
                }
            }
            if (par("AUTOASSIGN_ADDRESS"))
            {
                IPAddress AUTOASSIGN_ADDRESS_BASE(par("AUTOASSIGN_ADDRESS_BASE").stringValue());
                if (AUTOASSIGN_ADDRESS_BASE.getInt() == 0)
                    opp_error("ManetManager needs AUTOASSIGN_ADDRESS_BASE to be set to 0.0.0.0");
                IInterfaceTable *ift = InterfaceTableAccess ().get();
                IPAddress myAddr (AUTOASSIGN_ADDRESS_BASE.getInt() + uint32(getParentModule()->getId()));
                for (int k=0; k<ift->getNumInterfaces(); k++)
                {
                    InterfaceEntry *ie = ift->getInterface(k);
                    if (strstr (ie->getName(),"wlan")!=NULL)
                    {
                        ie->ipv4Data()->setIPAddress(myAddr);
                        ie->ipv4Data()->setNetmask(IPAddress::ALLONES_ADDRESS); // full address must match for local delivery
                    }
                }
            }
            /* for use dinamic modules in the future */

            if (strcmp("AODV", routingProtocol)==0)
            {
                if (!gate("to_aodv")->isConnected())
                {
                    dynamicLoad = true;
                    cModuleType *moduleType = cModuleType::find("inet.networklayer.manetrouting.AODVUU");
                    routingModule = moduleType->create("manetroutingprotocol", this);
                    // set up parameters and gate sizes before we set up its submodules
                    //  routingModule->par("unidir_hack") = par("unidir_hack");
                    //  routingModule->par("rreq_gratuitous") = par("rreq_gratuitous");
                    //  routingModule->par("expanding_ring_search") = par("expanding_ring_search");
                    //  routingModule->par("local_repair")= par("local_repair");
                    //  routingModule->par("receive_n_hellos") = par("receive_n_hellos");
                    //  routingModule->par("hello_jittering") = par("hello_jittering");
                    //  routingModule->par ("wait_on_reboot") =par ("wait_on_reboot");
                    //  routingModule->par("debug")=par("debug");
                    //  routingModule->par("rt_log_interval")=par("rt_log_interval");   // Note: in milliseconds!
                    //  routingModule->par("log_to_file")=par("log_to_file");
                    //  routingModule->par("optimized_hellos")=par("optimized_hellos");
                    //  routingModule->par("ratelimit")=par("ratelimit");
                    //  routingModule->par("llfeedback")=par("llfeedback");
                    //  routingModule->par("internet_gw_mode")=par("internet_gw_mode");
                    //  routingModule->par("internet_gw_address")=par("internet_gw_address");
                    //  routingModule->par("active_timeout")=par("active_timeout");

                    routingModule->finalizeParameters();
                    // Connet to ip
                    routingModule->gate("to_ip")->connectTo(gate("from_aodv"));
                    //           gate("to_aodv")->connectTo(routingModule->gate("ipIn"));
                }
            }
            else if (strcmp("DYMO", routingProtocol)==0)
            {
                if (!gate("to_dymo")->isConnected())
                {
                    dynamicLoad = true;
                    cModuleType *moduleType = cModuleType::find("inet.networklayer.manetrouting.DYMOUM");
                    routingModule = moduleType->create("manetroutingprotocol", this);

                    //  routingModule->par("no_path_acc_")=par("no_path_acc_");
                    //  routingModule->par("reissue_rreq_")= par("reissue_rreq_");
                    //  routingModule->par("s_bit_")= par("s_bit_");
                    //  routingModule->par("hello_ival_")=par("hello_ival_");
                    //  routingModule->par("promiscuous") = par("promiscuous");

                    //  routingModule->par("MaxPktSec") = par("MaxPktSec");
                    //  routingModule->par("NetDiameter") = par("NetDiameter");
                    //  routingModule->par("RouteTimeOut") = par("RouteTimeOut");
                    //  routingModule->par("RouteDeleteTimeOut") = par("RouteDeleteTimeOut");
                    //  routingModule->par("RREQWaitTime") = par("RREQWaitTime");
                    //  routingModule->par("RREQTries") = par("RREQTries");
                    //  routingModule->par("noRouteBehaviour") = par("noRouteBehaviour");
                    // set up parameters and gate sizes before we set up its submodules
                    routingModule->finalizeParameters();

                    // Connet to ip
                    routingModule->gate("to_ip")->connectTo(gate("from_dymo"));
                    //           gate("to_aodv")->connectTo(routingModule->gate("ipIn"));
                }
            }
            else if (strcmp("DSR",routingProtocol)==0)
            {
                if (!gate("to_dsr")->isConnected())
                {
                    dynamicLoad = true;
                    cModuleType *moduleType = cModuleType::find("inet.networklayer.manetrouting.DSRUU");
                    routingModule = moduleType->create("manetroutingprotocol", this);
                    // set up parameters and gate sizes before we set up its submodules
                    //  routingModule->par("PrintDebug") = par("PrintDebug");
                    //  routingModule->par("FlushLinkCache") = par("FlushLinkCache");
                    //  routingModule->par("PromiscOperation") = par("PromiscOperation");
                    //  routingModule->par("UseNetworkLayerAck")= par("UseNetworkLayerAck");
                    //  routingModule->par("BroadCastJitter") = par("BroadCastJitter");
                    //  routingModule->par("RouteCacheTimeout") = par("RouteCacheTimeout");
                    //  routingModule->par ("SendBufferTimeout") =par ("SendBufferTimeout");
                    //  routingModule->par("SendBufferSize")=par("SendBufferSize");
                    //  routingModule->par("RequestTableSize")=par("RequestTableSize"); // Note: in milliseconds!
                    //  routingModule->par("RequestTableIds")=par("RequestTableIds");
                    //  routingModule->par("MaxRequestRexmt")=par("MaxRequestRexmt");
                    //  routingModule->par("MaxRequestPeriod")=par("MaxRequestPeriod");
                    //  routingModule->par("RequestPeriod")=par("RequestPeriod");
                    //  routingModule->par("NonpropRequestTimeout")=par("NonpropRequestTimeout");
                    //  routingModule->par("RexmtBufferSize")=par("RexmtBufferSize");
                    //  routingModule->par("MaxMaintRexmt")=par("MaxMaintRexmt");
                    //  routingModule->par("MaintHoldoffTime")=par("MaintHoldoffTime");
                    //  routingModule->par("TryPassiveAcks")=par("TryPassiveAcks");
                    //  routingModule->par("PassiveAckTimeout")=par("PassiveAckTimeout");
                    //  routingModule->par("GratReplyHoldOff")=par("GratReplyHoldOff");
                    //  routingModule->par("MAX_SALVAGE_COUNT")=par("MAX_SALVAGE_COUNT");
                    //  routingModule->par("LifoSize")=par("LifoSize");
                    //  routingModule->par("PathCache")=par("PathCache");
                    //  routingModule->par("ETX_Active")=par("ETX_Active");
                    //  routingModule->par("ETXHelloInterval")=par("ETXHelloInterval");
                    //  routingModule->par("ETXWindowNumHello")=par("ETXWindowNumHello");
                    //  routingModule->par("ETXRetryBeforeFail")=par("ETXRetryBeforeFail");
                    //  routingModule->par("RREQMaxVisit")=par("RREQMaxVisit");
                    //  routingModule->par("RREPDestinationOnly")=par("RREPDestinationOnly");

                    routingModule->finalizeParameters();
                    // Connet to ip
                    routingModule->gate("toIp")->connectTo(gate("from_dsr"));
                    //          gate("to_dsr")->connectTo(routingModule->gate("fromIp"));
                }
            }

            else if (strncmp("OLSR",routingProtocol,4)==0)
            {
                if (!gate("to_olsr")->isConnected())
                {
                    dynamicLoad = true;
                    bool isEtx = (strcmp("OLSR_ETX",routingProtocol)==0);
                    cModuleType *moduleType;
                    if (isEtx)
                        moduleType = cModuleType::find("inet.networklayer.manetrouting.OLSR_ETX");
                    else
                        moduleType = cModuleType::find("inet.networklayer.manetrouting.OLSR");

                    routingModule = moduleType->create("manetroutingprotocol", this);
                    // set up parameters and gate sizes before we set up its submodules
                    //  routingModule->par("Hello_ival")=par("Hello_ival");
                    //  routingModule->par("Tc_ival")=par("Tc_ival");
                    //  routingModule->par("Mid_ival")=par("Mid_ival");
                    //  routingModule->par("use_mac")=par("use_mac");
                    //  routingModule->par("Willingness")=par("Willingness");
                    //  if (isEtx)
                    //  {
                    //      routingModule->par("Mpr_algorithm") = par("Mpr_algorithm");
                    //      routingModule->par("routing_algorithm") = par("routing_algorithm");
                    //      routingModule->par("Link_quality")= par("Link_quality");
                    //      routingModule->par("Fish_eye") = par("Fish_eye");
                    //      routingModule->par("Tc_redundancy") = par("Tc_redundancy");
                    //      routingModule->par("Link_delay") = par("Link_delay");
                    //      routingModule->par("C_alpha") = par("C_alpha");

                    //  }
                    routingModule->finalizeParameters();
                    // Connet to ip
                    routingModule->gate("to_ip")->connectTo(gate("from_olsr"));
                }
            }
            else if (strcmp("DSDV",routingProtocol)==0)
            {
                if (!gate("to_dsdv")->isConnected())
                {
                    dynamicLoad = true;
                    cModuleType *moduleType;
                    moduleType = cModuleType::find("inet.networklayer.manetrouting.DSDV_2");

                    routingModule = moduleType->create("manetroutingprotocol", this);

                    // set up parameters and gate sizes before we set up its submodules
                    //  routingModule->par("hellomsgperiod_DSDV")=par("hellomsgperiod_DSDV");
                    //  routingModule->par("netmask")=par("netmask");
                    //  routingModule->par("RNGseed_DSDV")=par("RNGseed_DSDV");
                    //  routingModule->par("MaxVariance_DSDV")=par("MaxVariance_DSDV");
                    //  routingModule->par("timetolive_routing_entry")=par("timetolive_routing_entry");

                    routingModule->finalizeParameters();
                    // Connet to ip
                    routingModule->gate("DSDV_toip")->connectTo(gate("from_dsdv"));
                }
            }
            else if (strcmp("DYMOFAU", routingProtocol)==0)
            {
                if (!gate("to_dymo")->isConnected())
                {
                    dynamicLoad = true;
                    cModuleType *moduleType = cModuleType::find("inet.networklayer.manetrouting.DYMO");
                    routingModule = moduleType->create("manetroutingprotocol", this);
                    // Connet to ip
                    routingModule->gate("to_ip")->connectTo(gate("from_dymo"));
                }
            }
            else if (strcmp("BATMAN", routingProtocol)==0)
            {
                if (!gate("to_batman")->isConnected())
                {
                    dynamicLoad = true;
                    cModuleType *moduleType = cModuleType::find("inet.networklayer.manetrouting.Batman");
                    routingModule = moduleType->create("manetroutingprotocol", this);
                    routingModule->finalizeParameters();

                    // Connet to ip
                    routingModule->gate("to_ip")->connectTo(gate("from_batman"));
                    //           gate("to_aodv")->connectTo(routingModule->gate("ipIn"));
                }
            }
            if (dynamicLoad)
            {
                // create internals, and schedule it
                routingModule->buildInside();
                routingModule->scheduleStart(simTime());
            }
        }


        if (strcmp("AODV", routingProtocol)==0)
            routing_protocol = AODV;
        else if (strcmp("DSR", routingProtocol)==0)
            routing_protocol = DSR;
        else if (strcmp("DYMO", routingProtocol)==0)
            routing_protocol = DYMO;
        else if (strcmp("DYMOFAU", routingProtocol)==0)
            routing_protocol = DYMO;
        else if (strncmp("OLSR", routingProtocol,4)==0)
            routing_protocol = OLSR;
        else if (strcmp("DSDV", routingProtocol)==0)
            routing_protocol = DSDV;
        else if (strcmp("BATMAN", routingProtocol)==0)
            routing_protocol = BATMAN;
        else
            manetActive=false;
        ev << "active Ad-hoc routing protocol : " << routingProtocol << "  dynamic : " << dynamicLoad << " \n";
    }
}




void ManetManager::handleMessage(cMessage *msg)
{
    /* for use dinamic modules in the future */
    /*
        sendDirect(msg,0, routingModule, "ipIn");
    */
    ControlManetRouting * controlRouting;
    if (!manetActive)
    {
        delete msg;
        return;
    }
    if (msg->arrivedOn("from_ip"))
    {
        switch (routing_protocol)
        {
        case AODV:
            if (dynamicLoad)
                sendDirect(msg,routingModule, "from_ip");
            else
                send( msg, "to_aodv");
            break;
        case DSR:
            if (dynamicLoad)
                sendDirect(msg,routingModule, "fromIp");
            else
                send( msg, "to_dsr");
            break;
        case DYMO:
            if (dynamicLoad)
                sendDirect(msg,routingModule, "from_ip");
            else
                send( msg, "to_dymo");
            break;
        case OLSR:
            controlRouting =  dynamic_cast<ControlManetRouting *> (msg);
            if (controlRouting)
            {
                if (controlRouting->getOptionCode() == MANET_ROUTE_NOROUTE)
                    icmpAccess.get()->sendErrorMessage((IPDatagram *)controlRouting->decapsulate(), ICMP_DESTINATION_UNREACHABLE, 0);
                delete controlRouting;
            }
            else
            {
                if (dynamicLoad)
                    sendDirect(msg,routingModule, "from_ip");
                else
                    send( msg, "to_olsr");
            }
            break;
        case DSDV:
            controlRouting =  dynamic_cast<ControlManetRouting *> (msg);
            if (controlRouting)
            {
                if (controlRouting->getOptionCode() == MANET_ROUTE_NOROUTE)
                    icmpAccess.get()->sendErrorMessage((IPDatagram *)controlRouting->decapsulate(), ICMP_DESTINATION_UNREACHABLE, 0);
                delete controlRouting;
            }
            else
            {
                if (dynamicLoad)
                    sendDirect(msg,routingModule, "ip_toDSDV");
                else
                    send( msg, "to_dsdv");
            }
            break;
        case BATMAN:
            controlRouting =  dynamic_cast<ControlManetRouting *> (msg);
            if (controlRouting)
            {
                if (controlRouting->getOptionCode() == MANET_ROUTE_NOROUTE)
                    icmpAccess.get()->sendErrorMessage((IPDatagram *)controlRouting->decapsulate(), ICMP_DESTINATION_UNREACHABLE, 0);
                delete controlRouting;
            }
            else
            {
                if (dynamicLoad)
                    sendDirect(msg,routingModule, "from_ip");
                else
                    send( msg, "to_batman");
            }
            break;
        }

    }
    else
    {
        switch (routing_protocol)
        {
        case AODV:
            if (!msg->arrivedOn("from_aodv"))
            {
                delete msg;
                return;
            }
            break;
        case DSR:
            if (!msg->arrivedOn("from_dsr"))
            {
                delete msg;
                return;
            }
            break;
        case DYMO:
            if (!msg->arrivedOn("from_dymo"))
            {
                delete msg;
                return;
            }
            break;
        case OLSR:
            if (!msg->arrivedOn("from_olsr"))
            {
                delete msg;
                return;
            }
            break;
        case DSDV:
            if (!msg->arrivedOn("from_dsdv"))
            {
                delete msg;
                return;
            }
            break;
        case BATMAN:
            if (!msg->arrivedOn("from_batman"))
            {
                delete msg;
                return;
            }
            break;
        }
        send(msg,"to_ip");
    }
}

