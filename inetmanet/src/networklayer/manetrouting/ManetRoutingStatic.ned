package inet.networklayer.manetrouting;
import inet.networklayer.manetrouting.OLSR;
import inet.networklayer.manetrouting.OLSR_ETX;
import inet.networklayer.manetrouting.DSDV_2;
import inet.networklayer.manetrouting.DSRUU;
import inet.networklayer.manetrouting.AODVUU;
import inet.networklayer.manetrouting.DYMOUM;

//
// MANET router (implementation without dynamic module creation)
// DO NOT use this module.
// 
// see: ManetRouter
// 
module ManetRoutingStatic
{
    parameters:
        @display("bgb=160,88");
    gates:
        input from_ip;
        output to_ip;
    submodules:
        // the name of manet routing manager module must be "manetmanager", the routing protocols is ManetRouting.ned
        manetmanager: ManetManagerStatic {
            parameters:
                // Manager parameters
                // eliminate in the routing table all the initial reference to wlan interfaces
                manetActive = true;
                routingProtocol = "DSDV";
                //		manetPurgeRoutingTables=true,
                manetPurgeRoutingTables = true;
        }
        dymoprotocol: DYMOUM {
            parameters:
                no_path_acc_ = false;
                reissue_rreq_ = false;
                s_bit_ = false;
                hello_ival_ = 0;
                MaxPktSec = 20; // 10
                promiscuous = false;
                NetDiameter = 10;
                RouteTimeOut = 3000;
                RouteDeleteTimeOut = 3000*5; //5*RouteTimeOut,
                RREQWaitTime = 1000;
                RREQTries = 3;
                noRouteBehaviour = 1;
        }
        aodvprotocol: AODVUU {
            parameters:
                // Aodv parameters
                log_to_file = false;
                hello_jittering = true;
                optimized_hellos = true;
                expanding_ring_search = true;
                local_repair = true;
                rreq_gratuitous = true;
                debug = false;
                rt_log_interval = 0;
                unidir_hack = 0;
                internet_gw_mode = 0;
                receive_n_hellos = 1;
                ratelimit = 1000;
                llfeedback = false; //1000, not suport inter layer communication
                wait_on_reboot = 0;
                active_timeout = 6000; // time in ms
                internet_gw_address = "0.0.0.0";
        }
        dsrprotocol: DSRUU {
            parameters:
                PrintDebug = true;
                FlushLinkCache = true;
                PromiscOperation = true;
                UseNetworkLayerAck = false;
                BroadCastJitter = -1; // 20  ms
                RouteCacheTimeout = -1; //300 seconds
                SendBufferTimeout = -1; //30 s
                SendBufferSize = -1;
                RequestTableSize = -1;
                RequestTableIds = -1;
                MaxRequestRexmt = -1; // 16,
                MaxRequestPeriod = -1; //10 SECONDS
                RequestPeriod = -1; //500  MILLISECONDS
                NonpropRequestTimeout = -1; //30  MILLISECONDS
                RexmtBufferSize = -1; //MAINT_BUF_MAX_LEN
                MaintHoldoffTime = -1; //250 MILLISECONDS
                MaxMaintRexmt = -1; //2
                TryPassiveAcks = true; //1
                PassiveAckTimeout = -1; //100 MILLISECONDS
                GratReplyHoldOff = -1; //1 SECONDS
                MAX_SALVAGE_COUNT = -1; //15
                LifoSize = 20;
                PathCache = true;
		RetryPacket = false;
                ETX_Active = false;
                ETXHelloInterval = 1; // Second
                ETXWindowNumHello = 10;
                ETXRetryBeforeFail = -1;
                RREPDestinationOnly = false;
                RREQMaxVisit = 5; // Max Number that a RREQ can be processes by a node
        }
        olsrprotocol: OLSR_ETX {
            parameters:
                Willingness = 3;
                Hello_ival = 2;
                Tc_ival = 5;
                Mid_ival = 5;
                use_mac = 0;
                Mpr_algorithm = 1;
                routing_algorithm = 1;
                Link_quality = 2;
                Fish_eye = false;
                Tc_redundancy = 3;
                Link_delay = true; //default false
                C_alpha = 0.2;
        }
        dsdvprotocol: DSDV_2 {
            parameters:
                hellomsgperiod_DSDV = 1s; //Period of DSDV hello message generation [seconds] 
                timetolive_routing_entry = 5s; // ;[seconds]
                netmask = "255.255.0.0"; //
                MaxVariance_DSDV = 1;
                RNGseed_DSDV = 0;

                @display("p=90,48;b=40,24");
        }
    connections allowunconnected:
        manetmanager.from_ip <-- from_ip;
        manetmanager.to_ip --> to_ip;
        manetmanager.from_aodv <-- aodvprotocol.to_ip;
        manetmanager.to_aodv --> aodvprotocol.from_ip;
        manetmanager.from_dymo <-- dymoprotocol.to_ip;
        manetmanager.to_dymo --> dymoprotocol.from_ip;

        manetmanager.from_dsr <-- dsrprotocol.toIp;
        manetmanager.to_dsr --> dsrprotocol.fromIp;

        manetmanager.from_olsr <-- olsrprotocol.to_ip;
        manetmanager.to_olsr --> olsrprotocol.from_ip;

        manetmanager.from_dsdv <-- dsdvprotocol.DSDV_toip;
        manetmanager.to_dsdv --> dsdvprotocol.ip_toDSDV;
}

