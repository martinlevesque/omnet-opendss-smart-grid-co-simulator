#
# OMNeT++/OMNEST Makefile for libVlans
#
# This file was generated with the command:
#  opp_makemake -f --deep --make-so -I/home/martin/travaux/inrs/simulator/inetmanet/src/util/headerserializers/headers -I/home/martin/travaux/inrs/simulator/inetmanet/src/networklayer/arp -I/home/martin/travaux/inrs/simulator/inetmanet/src/transport/sctp -I/home/martin/travaux/inrs/simulator/inetmanet/src/world -I/home/martin/travaux/inrs/simulator/inetmanet/src/transport/contract -I/home/martin/travaux/inrs/simulator/inetmanet/src/linklayer/mfcore -I/home/martin/travaux/inrs/simulator/inetmanet/src/linklayer/ethernet -I/home/martin/travaux/inrs/simulator/inetmanet/src/util -I/home/martin/travaux/inrs/simulator/inetmanet/src/networklayer/ted -I/home/martin/travaux/inrs/simulator/inetmanet/src/linklayer/ieee80211/mac -I/home/martin/travaux/inrs/simulator/inetmanet/src/networklayer/queue -I/home/martin/travaux/inrs/simulator/inetmanet/src/networklayer/common -I/home/martin/travaux/inrs/simulator/inetmanet/src/networklayer/ipv6 -I/home/martin/travaux/inrs/simulator/inetmanet/src/applications/pingapp -I/home/martin/travaux/inrs/simulator/inetmanet/src/networklayer/ldp -I/home/martin/travaux/inrs/simulator/inetmanet/src/linklayer/radio/errormodel -I/home/martin/travaux/inrs/simulator/inetmanet/src/transport/tcp -I/home/martin/travaux/inrs/simulator/inetmanet/src/util/headerserializers -I/home/martin/travaux/inrs/simulator/inetmanet/src -I/home/martin/travaux/inrs/simulator/inetmanet/src/networklayer/rsvp_te -I/home/martin/travaux/inrs/simulator/inetmanet/src/transport/udp -I/home/martin/travaux/inrs/simulator/inetmanet/src/networklayer/ipv4 -I/home/martin/travaux/inrs/simulator/inetmanet/src/networklayer/icmpv6 -I/home/martin/travaux/inrs/simulator/inetmanet/src/base -I/home/martin/travaux/inrs/simulator/inetmanet/src/networklayer/contract -I/home/martin/travaux/inrs/simulator/inetmanet/src/linklayer/ieee80211/mgmt -I/home/martin/travaux/inrs/simulator/inetmanet/src/networklayer/manetrouting/base -I/home/martin/travaux/inrs/simulator/inetmanet/src/networklayer/mpls -I/home/martin/travaux/inrs/simulator/inetmanet/src/linklayer/contract -I/home/martin/travaux/inrs/simulator/inetmanet/src/networklayer/autorouting -L/home/martin/travaux/inrs/simulator/inetmanet/out/$(CONFIGNAME)/src -linet -KINETMANET_PROJ=/home/martin/travaux/inrs/simulator/inetmanet
#

# Name of target to be created (-o option)
TARGET = libVlans$(SHARED_LIB_SUFFIX)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I$(INETMANET_PROJ)/src/util/headerserializers/headers \
    -I$(INETMANET_PROJ)/src/networklayer/arp \
    -I$(INETMANET_PROJ)/src/transport/sctp \
    -I$(INETMANET_PROJ)/src/world \
    -I$(INETMANET_PROJ)/src/transport/contract \
    -I$(INETMANET_PROJ)/src/linklayer/mfcore \
    -I$(INETMANET_PROJ)/src/linklayer/ethernet \
    -I$(INETMANET_PROJ)/src/util \
    -I$(INETMANET_PROJ)/src/networklayer/ted \
    -I$(INETMANET_PROJ)/src/linklayer/ieee80211/mac \
    -I$(INETMANET_PROJ)/src/networklayer/queue \
    -I$(INETMANET_PROJ)/src/networklayer/common \
    -I$(INETMANET_PROJ)/src/networklayer/ipv6 \
    -I$(INETMANET_PROJ)/src/applications/pingapp \
    -I$(INETMANET_PROJ)/src/networklayer/ldp \
    -I$(INETMANET_PROJ)/src/linklayer/radio/errormodel \
    -I$(INETMANET_PROJ)/src/transport/tcp \
    -I$(INETMANET_PROJ)/src/util/headerserializers \
    -I$(INETMANET_PROJ)/src \
    -I$(INETMANET_PROJ)/src/networklayer/rsvp_te \
    -I$(INETMANET_PROJ)/src/transport/udp \
    -I$(INETMANET_PROJ)/src/networklayer/ipv4 \
    -I$(INETMANET_PROJ)/src/networklayer/icmpv6 \
    -I$(INETMANET_PROJ)/src/base \
    -I$(INETMANET_PROJ)/src/networklayer/contract \
    -I$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt \
    -I$(INETMANET_PROJ)/src/networklayer/manetrouting/base \
    -I$(INETMANET_PROJ)/src/networklayer/mpls \
    -I$(INETMANET_PROJ)/src/linklayer/contract \
    -I$(INETMANET_PROJ)/src/networklayer/autorouting \
    -I. \
    -Ibase \
    -Ilinklayer \
    -Inetworklayer \
    -Inetworklayer/autoconfig \
    -Inodes

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS = -L$(INETMANET_PROJ)/out/$(CONFIGNAME)/src  -linet
LIBS += -Wl,-rpath,`abspath $(INETMANET_PROJ)/out/$(CONFIGNAME)/src`

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc and .msg files
OBJS = \
    $O/linklayer/MACVlanRelayUnitBase.o \
    $O/linklayer/Q_mgmt_PerVlan.o \
    $O/linklayer/EtherMacVlan.o \
    $O/linklayer/EtherEncapVlans.o \
    $O/networklayer/ARP_Vlans.o \
    $O/networklayer/autoconfig/VlanNetConfig.o \
    $O/linklayer/EtherFrameVlans_m.o \
    $O/linklayer/Ieee802VlanCtrl_m.o

# Message files
MSGFILES = \
    linklayer/EtherFrameVlans.msg \
    linklayer/Ieee802VlanCtrl.msg

# Other makefile variables (-K)
INETMANET_PROJ=/home/martin/travaux/inrs/simulator/inetmanet

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppenvir$D $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $(TARGET)

$(TARGET) : $O/$(TARGET)
	$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	$(SHLIB_LD) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(LIBS) $(OMNETPP_LIBS) $(LDFLAGS)
	$(SHLIB_POSTPROCESS) $O/$(TARGET)

.PHONY:

.SUFFIXES: .cc

$O/%.o: %.cc
	@$(MKPATH) $(dir $@)
	$(CXX) -c $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(MSGC) -s _m.cc $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

clean:
	-rm -rf $O
	-rm -f Vlans Vlans.exe libVlans.so libVlans.a libVlans.dll libVlans.dylib
	-rm -f ./*_m.cc ./*_m.h
	-rm -f base/*_m.cc base/*_m.h
	-rm -f linklayer/*_m.cc linklayer/*_m.h
	-rm -f networklayer/*_m.cc networklayer/*_m.h
	-rm -f networklayer/autoconfig/*_m.cc networklayer/autoconfig/*_m.h
	-rm -f nodes/*_m.cc nodes/*_m.h

cleanall: clean
	-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc base/*.cc linklayer/*.cc networklayer/*.cc networklayer/autoconfig/*.cc nodes/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/linklayer/EtherEncapVlans.o: linklayer/EtherEncapVlans.cc \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	linklayer/EtherVlanDefs.h \
	linklayer/EtherFrameVlans_m.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/util/headerserializers/headers/ethernet.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherEncap.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	linklayer/EtherEncapVlans.h \
	linklayer/Ieee802VlanCtrl_m.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceTableAccess.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h
$O/linklayer/EtherFrameVlans_m.o: linklayer/EtherFrameVlans_m.cc \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	linklayer/EtherFrameVlans_m.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h
$O/linklayer/Q_mgmt_PerVlan.o: linklayer/Q_mgmt_PerVlan.cc \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	linklayer/Q_mgmt_PerVlan.h
$O/linklayer/EtherMacVlan.o: linklayer/EtherMacVlan.cc \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	linklayer/EtherVlanDefs.h \
	linklayer/EtherMacVlan.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMACBase.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/util/headerserializers/headers/ethernet.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	$(INETMANET_PROJ)/src/util/PowerControlManager.h \
	$(INETMANET_PROJ)/src/base/IPowerControl.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMAC.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	$(INETMANET_PROJ)/src/linklayer/contract/TxNotifDetails.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	$(INETMANET_PROJ)/src/util/PowerControlMessages_m.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceTableAccess.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h
$O/linklayer/Ieee802VlanCtrl_m.o: linklayer/Ieee802VlanCtrl_m.cc \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	linklayer/Ieee802VlanCtrl_m.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h
$O/linklayer/MACVlanRelayUnitBase.o: linklayer/MACVlanRelayUnitBase.cc \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	linklayer/EtherFrameVlans_m.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	linklayer/MACVlanRelayUnitBase.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h
$O/networklayer/ARP_Vlans.o: networklayer/ARP_Vlans.cc \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INETMANET_PROJ)/src/networklayer/contract/IPProtocolId_m.h \
	$(INETMANET_PROJ)/src/networklayer/contract/IPControlInfo_m.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/IPv4InterfaceData.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/networklayer/arp/ARP.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/IPDatagram_m.h \
	$(INETMANET_PROJ)/src/networklayer/contract/IPControlInfo.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/IPRoute.h \
	linklayer/Ieee802VlanCtrl_m.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/RoutingTableAccess.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/IPDatagram.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/IRoutingTable.h \
	networklayer/ARP_Vlans.h \
	$(INETMANET_PROJ)/src/networklayer/contract/IPAddress.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	$(INETMANET_PROJ)/src/networklayer/arp/ARPPacket_m.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceTableAccess.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/IPRouteRule.h
$O/networklayer/autoconfig/VlanNetConfig.o: networklayer/autoconfig/VlanNetConfig.cc \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/IPv4InterfaceData.h \
	$(INETMANET_PROJ)/src/networklayer/autorouting/FlatNetworkConfigurator.h \
	networklayer/autoconfig/VlanNetConfig.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceTable.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	$(INETMANET_PROJ)/src/networklayer/contract/IPAddress.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h

