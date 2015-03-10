#
# OMNeT++/OMNEST Makefile for libFiWimodified
#
# This file was generated with the command:
#  opp_makemake -f --deep --make-so -O out -I/home/intissar/simulator/inetmanet/src/util/headerserializers/headers -I/home/intissar/simulator/inetmanet/src/networklayer/arp -I/home/intissar/simulator/inetmanet/src/transport/sctp -I/home/intissar/simulator/inetmanet/src/transport/contract -I/home/intissar/simulator/inetmanet/src/world -I/home/intissar/simulator/inetmanet/src/linklayer/mfcore -I/home/intissar/simulator/inetmanet/src/linklayer/ethernet -I/home/intissar/simulator/inetmanet/src/networklayer/ted -I/home/intissar/simulator/inetmanet/src/util -I/home/intissar/simulator/inetmanet/src/linklayer/ieee80211/mac -I/home/intissar/simulator/inetmanet/src/networklayer/queue -I/home/intissar/simulator/inetmanet/src/networklayer/ipv6 -I/home/intissar/simulator/inetmanet/src/networklayer/common -I/home/intissar/simulator/inetmanet/src/applications/pingapp -I/home/intissar/simulator/inetmanet/src/networklayer/ldp -I/home/intissar/simulator/inetmanet/src/linklayer/radio/errormodel -I/home/intissar/simulator/inetmanet/src/transport/tcp -I/home/intissar/simulator/inetmanet/src/util/headerserializers -I/home/intissar/simulator/inetmanet/src/applications/ethernet -I/home/intissar/simulator/inetmanet/src -I/home/intissar/simulator/inetmanet/src/networklayer/rsvp_te -I/home/intissar/simulator/inetmanet/src/transport/udp -I/home/intissar/simulator/inetmanet/src/networklayer/ipv4 -I/home/intissar/simulator/inetmanet/src/networklayer/icmpv6 -I/home/intissar/simulator/inetmanet/src/base -I/home/intissar/FiWi/Vlans/src/linklayer -I/home/intissar/simulator/inetmanet/src/networklayer/contract -I/home/intissar/simulator/inetmanet/src/linklayer/ieee80211/mgmt -I/home/intissar/simulator/inetmanet/src/networklayer/manetrouting/base -I/home/intissar/simulator/inetmanet/src/networklayer/mpls -I/home/intissar/simulator/inetmanet/src/linklayer/contract -L/home/intissar/FiWi/Vlans/out/$(CONFIGNAME)/src -L/home/intissar/simulator/inetmanet/out/$(CONFIGNAME)/src -L../out/$(CONFIGNAME) -lVlans -linet -KVLANS_PROJ=/home/intissar/FiWi/Vlans -KINETMANET_PROJ=/home/intissar/simulator/inetmanet
#

# Name of target to be created (-o option)
TARGET = libFiWimodified$(SHARED_LIB_SUFFIX)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I$(INETMANET_PROJ)/src/util/headerserializers/headers \
    -I$(INETMANET_PROJ)/src/networklayer/arp \
    -I$(INETMANET_PROJ)/src/transport/sctp \
    -I$(INETMANET_PROJ)/src/transport/contract \
    -I$(INETMANET_PROJ)/src/world \
    -I$(INETMANET_PROJ)/src/linklayer/mfcore \
    -I$(INETMANET_PROJ)/src/linklayer/ethernet \
    -I$(INETMANET_PROJ)/src/networklayer/ted \
    -I$(INETMANET_PROJ)/src/util \
    -I$(INETMANET_PROJ)/src/linklayer/ieee80211/mac \
    -I$(INETMANET_PROJ)/src/networklayer/queue \
    -I$(INETMANET_PROJ)/src/networklayer/ipv6 \
    -I$(INETMANET_PROJ)/src/networklayer/common \
    -I$(INETMANET_PROJ)/src/applications/pingapp \
    -I$(INETMANET_PROJ)/src/networklayer/ldp \
    -I$(INETMANET_PROJ)/src/linklayer/radio/errormodel \
    -I$(INETMANET_PROJ)/src/transport/tcp \
    -I$(INETMANET_PROJ)/src/util/headerserializers \
    -I$(INETMANET_PROJ)/src/applications/ethernet \
    -I$(INETMANET_PROJ)/src \
    -I$(INETMANET_PROJ)/src/networklayer/rsvp_te \
    -I$(INETMANET_PROJ)/src/transport/udp \
    -I$(INETMANET_PROJ)/src/networklayer/ipv4 \
    -I$(INETMANET_PROJ)/src/networklayer/icmpv6 \
    -I$(INETMANET_PROJ)/src/base \
    -I$(VLANS_PROJ)/src/linklayer \
    -I$(INETMANET_PROJ)/src/networklayer/contract \
    -I$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt \
    -I$(INETMANET_PROJ)/src/networklayer/manetrouting/base \
    -I$(INETMANET_PROJ)/src/networklayer/mpls \
    -I$(INETMANET_PROJ)/src/linklayer/contract \
    -I. \
    -IPON \
    -IPON/OLT \
    -IPON/ONU \
    -IPON/common_pon \
    -IPowerSystem \
    -IWireless \
    -Icommon

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS = -L$(VLANS_PROJ)/out/$(CONFIGNAME)/src -L$(INETMANET_PROJ)/out/$(CONFIGNAME)/src -L../out/$(CONFIGNAME)  -lVlans -linet
LIBS += -Wl,-rpath,`abspath $(VLANS_PROJ)/out/$(CONFIGNAME)/src` -Wl,-rpath,`abspath $(INETMANET_PROJ)/out/$(CONFIGNAME)/src` -Wl,-rpath,`abspath ../out/$(CONFIGNAME)`

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc and .msg files
OBJS = \
    $O/PON/Lambda.o \
    $O/PON/AWG.o \
    $O/PON/PON_Splitter.o \
    $O/PON/OLT/OLT_QPL_RR.o \
    $O/PON/OLT/OLTQPerLLiDBase.o \
    $O/PON/OLT/ONUTableEntry.o \
    $O/PON/OLT/OLTMacCtl.o \
    $O/PON/OLT/ONUTable.o \
    $O/PON/OLT/EPON_OLT_relayDefault.o \
    $O/PON/OLT/ONUReservation.o \
    $O/PON/ONU/ONUMacCtl.o \
    $O/PON/ONU/ONU_QPL_RR.o \
    $O/PON/ONU/ONUQPerLLiDBase.o \
    $O/PON/ONU/EPON_ONU_relayDefault.o \
    $O/PON/common_pon/CopyableQueueCVectors.o \
    $O/PON/common_pon/EPON_mux_demux.o \
    $O/PON/common_pon/MPCPTools.o \
    $O/PON/common_pon/EPON_CtrlInfo.o \
    $O/PON/common_pon/PONUtil.o \
    $O/PON/common_pon/EPON_mac.o \
    $O/PowerSystem/PowerSysNode.o \
    $O/PowerSystem/DMS.o \
    $O/PowerSystem/OpenDSS.o \
    $O/Wireless/RangeArea.o \
    $O/Wireless/Basic80211Radio.o \
    $O/Wireless/MyInit80211Mac.o \
    $O/Wireless/BasicIeee80211MgmtAP.o \
    $O/Wireless/BasicIeee80211Mac.o \
    $O/Wireless/IBasic80211Mac.o \
    $O/common/FiWiFastAdmissionControl.o \
    $O/common/MyWeightedFairQueue.o \
    $O/common/StatsCollector.o \
    $O/common/FiWiAdmissionControl.o \
    $O/common/LinklessMAC.o \
    $O/common/SimpleQueue.o \
    $O/common/FiWiGeneralConfigs.o \
    $O/common/FiWiTrafficMatrix.o \
    $O/common/FiWiRoutingTable.o \
    $O/common/FiWiTrafGen.o \
    $O/common/NoDelaySwitch.o \
    $O/common/VeryBasicApp.o \
    $O/common/MyUtil.o \
    $O/PON/common_pon/EPON_messages_m.o \
    $O/PowerSystem/PEVMessages_m.o \
    $O/common/FiWiMessages_m.o

# Message files
MSGFILES = \
    PON/common_pon/EPON_messages.msg \
    PowerSystem/PEVMessages.msg \
    common/FiWiMessages.msg

# Other makefile variables (-K)
VLANS_PROJ=/home/intissar/FiWi/Vlans
INETMANET_PROJ=/home/intissar/simulator/inetmanet

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
	-rm -f FiWimodified FiWimodified.exe libFiWimodified.so libFiWimodified.a libFiWimodified.dll libFiWimodified.dylib
	-rm -f ./*_m.cc ./*_m.h
	-rm -f PON/*_m.cc PON/*_m.h
	-rm -f PON/OLT/*_m.cc PON/OLT/*_m.h
	-rm -f PON/ONU/*_m.cc PON/ONU/*_m.h
	-rm -f PON/common_pon/*_m.cc PON/common_pon/*_m.h
	-rm -f PowerSystem/*_m.cc PowerSystem/*_m.h
	-rm -f Wireless/*_m.cc Wireless/*_m.h
	-rm -f common/*_m.cc common/*_m.h

cleanall: clean
	-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc PON/*.cc PON/OLT/*.cc PON/ONU/*.cc PON/common_pon/*.cc PowerSystem/*.cc Wireless/*.cc common/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/PON/Lambda.o: PON/Lambda.cc \
	PON/Lambda.h \
	PON/common_pon/EPON_CtrlInfo.h
$O/PON/AWG.o: PON/AWG.cc \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	PON/OLT/ONUReservation.h \
	PON/common_pon/MPCP_codes.h \
	PON/common_pon/EPON_CtrlInfo.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	PON/common_pon/EPON_messages_m.h \
	PON/common_pon/MPCPTools.h \
	PON/common_pon/PONUtil.h \
	PON/Lambda.h \
	PON/OLT/ONUTable.h \
	PON/OLT/ONUTableEntry.h \
	common/FiWiGeneralConfigs.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h \
	PON/AWG.h
$O/PON/PON_Splitter.o: PON/PON_Splitter.cc \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	PON/Lambda.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	PON/common_pon/EPON_CtrlInfo.h \
	common/FiWiGeneralConfigs.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h \
	PON/common_pon/MPCPTools.h \
	PON/common_pon/EPON_messages_m.h \
	PON/common_pon/PONUtil.h \
	PON/PON_Splitter.h
$O/PON/OLT/ONUTable.o: PON/OLT/ONUTable.cc \
	PON/OLT/ONUReservation.h \
	PON/Lambda.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	PON/OLT/ONUTableEntry.h \
	PON/OLT/ONUTable.h \
	common/FiWiGeneralConfigs.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	PON/common_pon/PONUtil.h
$O/PON/OLT/OLT_QPL_RR.o: PON/OLT/OLT_QPL_RR.cc \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMACBase.h \
	PON/OLT/OLTQPerLLiDBase.h \
	PON/common_pon/EPON_mac.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	PON/common_pon/EPON_messages_m.h \
	$(INETMANET_PROJ)/src/base/IPowerControl.h \
	PON/OLT/OLTMacCtl.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	PON/common_pon/CopyableQueueCVectors.h \
	PON/OLT/IOLTMgmt.h \
	$(INETMANET_PROJ)/src/linklayer/contract/TxNotifDetails.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	PON/OLT/ONUTable.h \
	PON/OLT/ONUTableEntry.h \
	common/MyAbstractQueue.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	PON/OLT/ONUReservation.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(VLANS_PROJ)/src/linklayer/EtherFrameVlans_m.h \
	$(VLANS_PROJ)/src/linklayer/EtherVlanDefs.h \
	PON/common_pon/MPCP_codes.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	PON/common_pon/EPON_CtrlInfo.h \
	$(INETMANET_PROJ)/src/util/headerserializers/headers/ethernet.h \
	$(INETMANET_PROJ)/src/util/PowerControlManager.h \
	$(VLANS_PROJ)/src/linklayer/MACVlanRelayUnitBase.h \
	PON/common_pon/MPCPTools.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	PON/OLT/OLT_QPL_RR.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	$(INETMANET_PROJ)/src/util/PowerControlMessages_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h
$O/PON/OLT/OLTMacCtl.o: PON/OLT/OLTMacCtl.cc \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMACBase.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	PON/common_pon/EPON_messages_m.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.h \
	$(INETMANET_PROJ)/src/base/IPowerControl.h \
	$(INETMANET_PROJ)/src/util/RawStat.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/linklayer/contract/TxNotifDetails.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Consts.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Frame_m.h \
	PON/OLT/ONUTableEntry.h \
	common/MyAbstractQueue.h \
	common/FiWiGeneralConfigs.h \
	Wireless/BasicIeee80211Mac.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	common/FiWiTrafficMatrix.h \
	PON/OLT/ONUReservation.h \
	$(VLANS_PROJ)/src/linklayer/EtherVlanDefs.h \
	$(INETMANET_PROJ)/src/base/PassiveQueueBase.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h \
	PON/common_pon/MPCP_codes.h \
	common/SimpleQueue.h \
	$(INETMANET_PROJ)/src/linklayer/mfcore/WirelessMacBase.h \
	$(INETMANET_PROJ)/src/util/PowerControlManager.h \
	$(INETMANET_PROJ)/src/linklayer/radio/errormodel/Ieee80211DataRate.h \
	$(INETMANET_PROJ)/src/linklayer/contract/RadioState.h \
	common/FiWiFastAdmissionControl.h \
	$(INETMANET_PROJ)/src/util/PowerControlMessages_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h \
	$(INETMANET_PROJ)/src/networklayer/queue/IQoSClassifier.h \
	$(INETMANET_PROJ)/src/base/FSMA.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtFrames_m.h \
	PON/common_pon/EPON_mac.h \
	PON/common_pon/PONUtil.h \
	PON/OLT/OLTMacCtl.h \
	PowerSystem/PowerSysNode.h \
	PowerSystem/PEVMessages_m.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	PowerSystem/DMS.h \
	PON/OLT/ONUTable.h \
	PowerSystem/OpenDSS.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	PON/common_pon/EPON_CtrlInfo.h \
	$(INETMANET_PROJ)/src/util/headerserializers/headers/ethernet.h \
	common/FiWiTrafGen.h \
	PON/common_pon/MPCPTools.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	PON/Lambda.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	Wireless/IBasic80211Mac.h \
	common/MyUtil.h \
	common/MyWeightedFairQueue.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	common/FiWiRoutingTable.h \
	$(INETMANET_PROJ)/src/applications/ethernet/EtherApp_m.h \
	Wireless/BasicIeee80211MgmtAP.h
$O/PON/OLT/ONUReservation.o: PON/OLT/ONUReservation.cc \
	PON/OLT/ONUReservation.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	PON/OLT/ONUTableEntry.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h
$O/PON/OLT/ONUTableEntry.o: PON/OLT/ONUTableEntry.cc \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	PON/OLT/ONUTableEntry.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h
$O/PON/OLT/OLTQPerLLiDBase.o: PON/OLT/OLTQPerLLiDBase.cc \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMACBase.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	PON/common_pon/EPON_messages_m.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.h \
	$(INETMANET_PROJ)/src/base/IPowerControl.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/util/RawStat.h \
	$(INETMANET_PROJ)/src/linklayer/contract/TxNotifDetails.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Consts.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Frame_m.h \
	PON/OLT/ONUTableEntry.h \
	common/MyAbstractQueue.h \
	common/FiWiGeneralConfigs.h \
	Wireless/BasicIeee80211Mac.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	common/FiWiTrafficMatrix.h \
	PON/OLT/ONUReservation.h \
	$(VLANS_PROJ)/src/linklayer/EtherVlanDefs.h \
	$(VLANS_PROJ)/src/linklayer/EtherFrameVlans_m.h \
	$(INETMANET_PROJ)/src/base/PassiveQueueBase.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h \
	PON/common_pon/MPCP_codes.h \
	$(INETMANET_PROJ)/src/linklayer/mfcore/WirelessMacBase.h \
	$(INETMANET_PROJ)/src/util/PowerControlManager.h \
	$(INETMANET_PROJ)/src/linklayer/radio/errormodel/Ieee80211DataRate.h \
	$(INETMANET_PROJ)/src/linklayer/contract/RadioState.h \
	$(INETMANET_PROJ)/src/util/PowerControlMessages_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h \
	$(INETMANET_PROJ)/src/networklayer/queue/IQoSClassifier.h \
	PON/common_pon/IExtPassiveQueue.h \
	$(INETMANET_PROJ)/src/base/FSMA.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtFrames_m.h \
	PON/OLT/OLTQPerLLiDBase.h \
	PON/common_pon/EPON_mac.h \
	PON/common_pon/PONUtil.h \
	PON/OLT/OLTMacCtl.h \
	PON/common_pon/CopyableQueueCVectors.h \
	PON/OLT/IOLTMgmt.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	PON/OLT/ONUTable.h \
	PON/ONU/ONUMacCtl.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	PON/common_pon/EPON_CtrlInfo.h \
	$(INETMANET_PROJ)/src/util/headerserializers/headers/ethernet.h \
	$(VLANS_PROJ)/src/linklayer/MACVlanRelayUnitBase.h \
	PON/common_pon/MPCPTools.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	PON/Lambda.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	common/MyUtil.h \
	Wireless/IBasic80211Mac.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	common/FiWiRoutingTable.h \
	Wireless/BasicIeee80211MgmtAP.h
$O/PON/OLT/EPON_OLT_relayDefault.o: PON/OLT/EPON_OLT_relayDefault.cc \
	PON/OLT/ONUReservation.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(VLANS_PROJ)/src/linklayer/EtherFrameVlans_m.h \
	PON/common_pon/EPON_CtrlInfo.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(VLANS_PROJ)/src/linklayer/MACVlanRelayUnitBase.h \
	PON/common_pon/PONUtil.h \
	PON/OLT/EPON_OLT_relayDefault.h \
	PON/Lambda.h \
	common/StatsCollector.h \
	PON/OLT/ONUTableEntry.h \
	PON/OLT/ONUTable.h \
	common/FiWiGeneralConfigs.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h
$O/PON/ONU/ONUMacCtl.o: PON/ONU/ONUMacCtl.cc \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMACBase.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	PON/common_pon/EPON_messages_m.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.h \
	$(INETMANET_PROJ)/src/base/IPowerControl.h \
	$(INETMANET_PROJ)/src/util/RawStat.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/linklayer/contract/TxNotifDetails.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Consts.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	PON/OLT/ONUTableEntry.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Frame_m.h \
	common/MyAbstractQueue.h \
	common/FiWiGeneralConfigs.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	Wireless/BasicIeee80211Mac.h \
	common/FiWiTrafficMatrix.h \
	PON/OLT/ONUReservation.h \
	$(VLANS_PROJ)/src/linklayer/EtherVlanDefs.h \
	$(INETMANET_PROJ)/src/base/PassiveQueueBase.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h \
	PON/common_pon/MPCP_codes.h \
	$(INETMANET_PROJ)/src/linklayer/mfcore/WirelessMacBase.h \
	$(INETMANET_PROJ)/src/util/PowerControlManager.h \
	$(INETMANET_PROJ)/src/linklayer/radio/errormodel/Ieee80211DataRate.h \
	$(INETMANET_PROJ)/src/linklayer/contract/RadioState.h \
	$(INETMANET_PROJ)/src/util/PowerControlMessages_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h \
	$(INETMANET_PROJ)/src/networklayer/queue/IQoSClassifier.h \
	PON/common_pon/IExtPassiveQueue.h \
	$(INETMANET_PROJ)/src/base/FSMA.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtFrames_m.h \
	PON/common_pon/EPON_mac.h \
	PON/common_pon/PONUtil.h \
	PON/OLT/IOLTMgmt.h \
	common/StatsCollector.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	PON/OLT/ONUTable.h \
	PON/ONU/ONUMacCtl.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	PON/common_pon/EPON_CtrlInfo.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/util/headerserializers/headers/ethernet.h \
	PON/common_pon/MPCPTools.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	PON/Lambda.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	common/MyUtil.h \
	Wireless/IBasic80211Mac.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	PON/ONU/ONUQPerLLiDBase.h \
	common/FiWiRoutingTable.h \
	Wireless/BasicIeee80211MgmtAP.h
$O/PON/ONU/ONU_QPL_RR.o: PON/ONU/ONU_QPL_RR.cc \
	PON/common_pon/IExtPassiveQueue.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMACBase.h \
	PON/common_pon/EPON_mac.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	PON/common_pon/EPON_messages_m.h \
	PON/common_pon/PONUtil.h \
	$(INETMANET_PROJ)/src/base/IPowerControl.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	PON/OLT/IOLTMgmt.h \
	$(INETMANET_PROJ)/src/linklayer/contract/TxNotifDetails.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	PON/OLT/ONUTableEntry.h \
	common/FiWiGeneralConfigs.h \
	common/MyAbstractQueue.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	PON/ONU/ONUMacCtl.h \
	PON/OLT/ONUReservation.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(VLANS_PROJ)/src/linklayer/EtherVlanDefs.h \
	PON/common_pon/MPCP_codes.h \
	common/SimpleQueue.h \
	PON/common_pon/EPON_CtrlInfo.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/util/headerserializers/headers/ethernet.h \
	$(INETMANET_PROJ)/src/util/PowerControlManager.h \
	PON/common_pon/MPCPTools.h \
	PON/ONU/ONU_QPL_RR.h \
	PON/Lambda.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	$(INETMANET_PROJ)/src/util/PowerControlMessages_m.h \
	PON/ONU/ONUQPerLLiDBase.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h
$O/PON/ONU/EPON_ONU_relayDefault.o: PON/ONU/EPON_ONU_relayDefault.cc \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMACBase.h \
	common/LinklessMAC.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	PON/common_pon/EPON_messages_m.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.h \
	$(INETMANET_PROJ)/src/base/IPowerControl.h \
	$(INETMANET_PROJ)/src/util/RawStat.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/linklayer/contract/TxNotifDetails.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Consts.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceTableAccess.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Frame_m.h \
	common/FiWiGeneralConfigs.h \
	Wireless/BasicIeee80211Mac.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	common/FiWiTrafficMatrix.h \
	$(VLANS_PROJ)/src/linklayer/EtherVlanDefs.h \
	$(VLANS_PROJ)/src/linklayer/EtherFrameVlans_m.h \
	$(INETMANET_PROJ)/src/base/PassiveQueueBase.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h \
	$(INETMANET_PROJ)/src/linklayer/mfcore/WirelessMacBase.h \
	$(INETMANET_PROJ)/src/util/PowerControlManager.h \
	$(INETMANET_PROJ)/src/linklayer/radio/errormodel/Ieee80211DataRate.h \
	$(INETMANET_PROJ)/src/linklayer/contract/RadioState.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMAC.h \
	$(INETMANET_PROJ)/src/util/PowerControlMessages_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h \
	$(INETMANET_PROJ)/src/networklayer/queue/IQoSClassifier.h \
	$(INETMANET_PROJ)/src/base/FSMA.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtFrames_m.h \
	$(VLANS_PROJ)/src/linklayer/EtherMacVlan.h \
	PON/ONU/EPON_ONU_relayDefault.h \
	PowerSystem/PowerSysNode.h \
	PowerSystem/PEVMessages_m.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	PowerSystem/DMS.h \
	PowerSystem/OpenDSS.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	PON/common_pon/EPON_CtrlInfo.h \
	$(INETMANET_PROJ)/src/util/headerserializers/headers/ethernet.h \
	common/FiWiTrafGen.h \
	$(VLANS_PROJ)/src/linklayer/MACVlanRelayUnitBase.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	Wireless/IBasic80211Mac.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	common/MyUtil.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	common/FiWiRoutingTable.h \
	$(INETMANET_PROJ)/src/applications/ethernet/EtherApp_m.h \
	Wireless/BasicIeee80211MgmtAP.h
$O/PON/ONU/ONUQPerLLiDBase.o: PON/ONU/ONUQPerLLiDBase.cc \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMACBase.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	PON/common_pon/EPON_messages_m.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.h \
	$(INETMANET_PROJ)/src/base/IPowerControl.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/util/RawStat.h \
	$(INETMANET_PROJ)/src/linklayer/contract/TxNotifDetails.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Consts.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	PON/OLT/ONUTableEntry.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Frame_m.h \
	common/MyAbstractQueue.h \
	common/FiWiGeneralConfigs.h \
	Wireless/BasicIeee80211Mac.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	common/FiWiTrafficMatrix.h \
	PON/OLT/ONUReservation.h \
	$(VLANS_PROJ)/src/linklayer/EtherVlanDefs.h \
	$(INETMANET_PROJ)/src/base/PassiveQueueBase.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h \
	PON/common_pon/MPCP_codes.h \
	$(INETMANET_PROJ)/src/linklayer/mfcore/WirelessMacBase.h \
	common/SimpleQueue.h \
	$(INETMANET_PROJ)/src/util/PowerControlManager.h \
	$(INETMANET_PROJ)/src/linklayer/radio/errormodel/Ieee80211DataRate.h \
	$(INETMANET_PROJ)/src/linklayer/contract/RadioState.h \
	common/FiWiFastAdmissionControl.h \
	$(INETMANET_PROJ)/src/util/PowerControlMessages_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h \
	$(INETMANET_PROJ)/src/networklayer/queue/IQoSClassifier.h \
	PON/common_pon/IExtPassiveQueue.h \
	$(INETMANET_PROJ)/src/base/FSMA.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtFrames_m.h \
	PON/common_pon/EPON_mac.h \
	PON/common_pon/PONUtil.h \
	PowerSystem/PowerSysNode.h \
	PON/OLT/IOLTMgmt.h \
	common/StatsCollector.h \
	PowerSystem/PEVMessages_m.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	PON/OLT/ONUTable.h \
	PowerSystem/DMS.h \
	PON/ONU/ONUMacCtl.h \
	PowerSystem/OpenDSS.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	PON/common_pon/EPON_CtrlInfo.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/util/headerserializers/headers/ethernet.h \
	common/FiWiTrafGen.h \
	PON/common_pon/MPCPTools.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	PON/Lambda.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	Wireless/IBasic80211Mac.h \
	common/MyUtil.h \
	common/MyWeightedFairQueue.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	PON/ONU/ONUQPerLLiDBase.h \
	common/FiWiRoutingTable.h \
	$(INETMANET_PROJ)/src/applications/ethernet/EtherApp_m.h \
	Wireless/BasicIeee80211MgmtAP.h
$O/PON/common_pon/CopyableQueueCVectors.o: PON/common_pon/CopyableQueueCVectors.cc \
	PON/common_pon/CopyableQueueCVectors.h
$O/PON/common_pon/EPON_mux_demux.o: PON/common_pon/EPON_mux_demux.cc \
	$(INETMANET_PROJ)/src/networklayer/queue/IQoSClassifier.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/base/FSMA.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtFrames_m.h \
	PON/common_pon/EPON_mux_demux.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	PON/common_pon/EPON_messages_m.h \
	PON/common_pon/PONUtil.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/util/RawStat.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Consts.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Frame_m.h \
	PON/OLT/ONUTableEntry.h \
	PON/OLT/ONUTable.h \
	common/FiWiGeneralConfigs.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	Wireless/BasicIeee80211Mac.h \
	common/FiWiTrafficMatrix.h \
	PON/OLT/ONUReservation.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INETMANET_PROJ)/src/base/PassiveQueueBase.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h \
	$(INETMANET_PROJ)/src/linklayer/mfcore/WirelessMacBase.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	PON/common_pon/MPCPTools.h \
	$(INETMANET_PROJ)/src/linklayer/radio/errormodel/Ieee80211DataRate.h \
	$(INETMANET_PROJ)/src/linklayer/contract/RadioState.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	PON/Lambda.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	Wireless/IBasic80211Mac.h \
	common/MyUtil.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	common/FiWiRoutingTable.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	Wireless/BasicIeee80211MgmtAP.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h
$O/PON/common_pon/EPON_mac.o: PON/common_pon/EPON_mac.cc \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(VLANS_PROJ)/src/linklayer/EtherVlanDefs.h \
	PON/common_pon/EPON_CtrlInfo.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMACBase.h \
	$(INETMANET_PROJ)/src/util/headerserializers/headers/ethernet.h \
	PON/common_pon/EPON_mac.h \
	$(INETMANET_PROJ)/src/util/PowerControlManager.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	PON/common_pon/EPON_messages_m.h \
	$(INETMANET_PROJ)/src/base/IPowerControl.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/linklayer/contract/TxNotifDetails.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	$(INETMANET_PROJ)/src/util/PowerControlMessages_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h
$O/PON/common_pon/MPCPTools.o: PON/common_pon/MPCPTools.cc \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h \
	PON/common_pon/EPON_messages_m.h \
	PON/common_pon/MPCPTools.h
$O/PON/common_pon/EPON_CtrlInfo.o: PON/common_pon/EPON_CtrlInfo.cc \
	PON/common_pon/EPON_CtrlInfo.h
$O/PON/common_pon/EPON_messages_m.o: PON/common_pon/EPON_messages_m.cc \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h \
	PON/common_pon/EPON_messages_m.h
$O/PON/common_pon/PONUtil.o: PON/common_pon/PONUtil.cc \
	$(INETMANET_PROJ)/src/networklayer/queue/IQoSClassifier.h \
	$(INETMANET_PROJ)/src/base/FSMA.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtFrames_m.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.h \
	PON/common_pon/PONUtil.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/util/RawStat.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Consts.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Frame_m.h \
	common/FiWiGeneralConfigs.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	Wireless/BasicIeee80211Mac.h \
	common/FiWiTrafficMatrix.h \
	$(INETMANET_PROJ)/src/base/PassiveQueueBase.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h \
	$(INETMANET_PROJ)/src/linklayer/mfcore/WirelessMacBase.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/linklayer/radio/errormodel/Ieee80211DataRate.h \
	$(INETMANET_PROJ)/src/linklayer/contract/RadioState.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	PON/Lambda.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	common/MyUtil.h \
	Wireless/IBasic80211Mac.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	common/FiWiRoutingTable.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	Wireless/BasicIeee80211MgmtAP.h
$O/PowerSystem/OpenDSS.o: PowerSystem/OpenDSS.cc \
	$(INETMANET_PROJ)/src/networklayer/queue/IQoSClassifier.h \
	$(INETMANET_PROJ)/src/base/FSMA.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtFrames_m.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/util/RawStat.h \
	PowerSystem/PowerSysNode.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Consts.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Frame_m.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	Wireless/BasicIeee80211Mac.h \
	common/FiWiTrafficMatrix.h \
	PowerSystem/OpenDSS.h \
	$(INETMANET_PROJ)/src/base/PassiveQueueBase.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h \
	$(INETMANET_PROJ)/src/linklayer/mfcore/WirelessMacBase.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/linklayer/radio/errormodel/Ieee80211DataRate.h \
	$(INETMANET_PROJ)/src/linklayer/contract/RadioState.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	common/MyUtil.h \
	Wireless/IBasic80211Mac.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	common/FiWiRoutingTable.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	Wireless/BasicIeee80211MgmtAP.h
$O/PowerSystem/DMS.o: PowerSystem/DMS.cc \
	PowerSystem/OpenDSS.h \
	$(INETMANET_PROJ)/src/util/RawStat.h \
	PowerSystem/PowerSysNode.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	PowerSystem/DMS.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h
$O/PowerSystem/PowerSysNode.o: PowerSystem/PowerSysNode.cc \
	PowerSystem/PowerSysNode.h
$O/PowerSystem/PEVMessages_m.o: PowerSystem/PEVMessages_m.cc \
	PowerSystem/PEVMessages_m.h
$O/Wireless/BasicIeee80211MgmtAP.o: Wireless/BasicIeee80211MgmtAP.cc \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/networklayer/contract/IPProtocolId_m.h \
	$(INETMANET_PROJ)/src/networklayer/contract/IPControlInfo_m.h \
	$(INETMANET_PROJ)/src/base/QueueBase.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMACBase.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.h \
	$(INETMANET_PROJ)/src/base/IPowerControl.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/util/RawStat.h \
	$(INETMANET_PROJ)/src/linklayer/contract/TxNotifDetails.h \
	$(INETMANET_PROJ)/src/networklayer/contract/IPAddress.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Consts.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceTableAccess.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Frame_m.h \
	common/FiWiGeneralConfigs.h \
	$(INETMANET_PROJ)/src/networklayer/manetrouting/base/uint128.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	Wireless/BasicIeee80211Mac.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/IPRouteRule.h \
	common/FiWiTrafficMatrix.h \
	$(INETMANET_PROJ)/src/base/ReassemblyBuffer.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/ICMPMessage_m.h \
	$(INETMANET_PROJ)/src/base/AbstractQueue.h \
	$(INETMANET_PROJ)/src/base/PassiveQueueBase.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h \
	$(INETMANET_PROJ)/src/linklayer/mfcore/WirelessMacBase.h \
	$(INETMANET_PROJ)/src/util/PowerControlManager.h \
	$(INETMANET_PROJ)/src/linklayer/radio/errormodel/Ieee80211DataRate.h \
	$(INETMANET_PROJ)/src/linklayer/contract/RadioState.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMAC.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/IRoutingTable.h \
	$(INETMANET_PROJ)/src/util/PowerControlMessages_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h \
	$(INETMANET_PROJ)/src/networklayer/queue/IQoSClassifier.h \
	$(INETMANET_PROJ)/src/networklayer/contract/IPv6Address.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/ControlManetRouting_m.h \
	$(INETMANET_PROJ)/src/base/FSMA.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtFrames_m.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/IP.h \
	PowerSystem/PowerSysNode.h \
	$(INETMANET_PROJ)/src/networklayer/contract/IPControlInfo.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/IPRoute.h \
	common/StatsCollector.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/RoutingTableAccess.h \
	PowerSystem/PEVMessages_m.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/IPDatagram.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/IPFragBuf.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/ICMP.h \
	PowerSystem/DMS.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/ICMPMessage.h \
	PowerSystem/OpenDSS.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	common/FiWiTrafGen.h \
	$(INETMANET_PROJ)/src/base/ProtocolMap.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/IPDatagram_m.h \
	Wireless/IBasic80211Mac.h \
	common/MyUtil.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	$(INETMANET_PROJ)/src/networklayer/ipv4/ICMPAccess.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	common/FiWiRoutingTable.h \
	$(INETMANET_PROJ)/src/applications/ethernet/EtherApp_m.h \
	Wireless/BasicIeee80211MgmtAP.h
$O/Wireless/IBasic80211Mac.o: Wireless/IBasic80211Mac.cc \
	Wireless/IBasic80211Mac.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h
$O/Wireless/MyInit80211Mac.o: Wireless/MyInit80211Mac.cc \
	Wireless/MyInit80211Mac.h
$O/Wireless/RangeArea.o: Wireless/RangeArea.cc \
	$(INETMANET_PROJ)/src/networklayer/queue/IQoSClassifier.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/base/FSMA.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtFrames_m.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	PON/common_pon/EPON_messages_m.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/util/RawStat.h \
	Wireless/RangeArea.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Consts.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Frame_m.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	Wireless/BasicIeee80211Mac.h \
	common/FiWiTrafficMatrix.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INETMANET_PROJ)/src/base/PassiveQueueBase.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h \
	$(INETMANET_PROJ)/src/linklayer/mfcore/WirelessMacBase.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	PON/common_pon/MPCPTools.h \
	$(INETMANET_PROJ)/src/linklayer/radio/errormodel/Ieee80211DataRate.h \
	$(INETMANET_PROJ)/src/linklayer/contract/RadioState.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	common/MyUtil.h \
	Wireless/IBasic80211Mac.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	common/FiWiRoutingTable.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	Wireless/BasicIeee80211MgmtAP.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h
$O/Wireless/Basic80211Radio.o: Wireless/Basic80211Radio.cc \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMACBase.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	PON/common_pon/EPON_messages_m.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.h \
	$(INETMANET_PROJ)/src/base/IPowerControl.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	Wireless/Basic80211Radio.h \
	$(INETMANET_PROJ)/src/util/RawStat.h \
	Wireless/RangeArea.h \
	$(INETMANET_PROJ)/src/linklayer/contract/TxNotifDetails.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Consts.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceTableAccess.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Frame_m.h \
	common/FiWiGeneralConfigs.h \
	Wireless/BasicIeee80211Mac.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	common/FiWiTrafficMatrix.h \
	$(VLANS_PROJ)/src/linklayer/EtherVlanDefs.h \
	$(INETMANET_PROJ)/src/base/PassiveQueueBase.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h \
	$(INETMANET_PROJ)/src/linklayer/mfcore/WirelessMacBase.h \
	$(INETMANET_PROJ)/src/util/PowerControlManager.h \
	$(INETMANET_PROJ)/src/linklayer/radio/errormodel/Ieee80211DataRate.h \
	$(INETMANET_PROJ)/src/linklayer/contract/RadioState.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMAC.h \
	$(INETMANET_PROJ)/src/util/PowerControlMessages_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h \
	$(INETMANET_PROJ)/src/networklayer/queue/IQoSClassifier.h \
	$(INETMANET_PROJ)/src/base/FSMA.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtFrames_m.h \
	$(VLANS_PROJ)/src/linklayer/EtherMacVlan.h \
	PON/common_pon/PONUtil.h \
	PowerSystem/PowerSysNode.h \
	PowerSystem/PEVMessages_m.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	PowerSystem/DMS.h \
	PowerSystem/OpenDSS.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	PON/common_pon/EPON_CtrlInfo.h \
	$(INETMANET_PROJ)/src/util/headerserializers/headers/ethernet.h \
	common/FiWiTrafGen.h \
	PON/common_pon/MPCPTools.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	PON/Lambda.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	common/MyUtil.h \
	Wireless/IBasic80211Mac.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	common/FiWiRoutingTable.h \
	$(INETMANET_PROJ)/src/applications/ethernet/EtherApp_m.h \
	Wireless/BasicIeee80211MgmtAP.h
$O/Wireless/BasicIeee80211Mac.o: Wireless/BasicIeee80211Mac.cc \
	$(INETMANET_PROJ)/src/networklayer/queue/IQoSClassifier.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/base/FSMA.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtFrames_m.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMACBase.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.h \
	$(INETMANET_PROJ)/src/base/IPowerControl.h \
	PowerSystem/PowerSysNode.h \
	$(INETMANET_PROJ)/src/util/RawStat.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	PowerSystem/PEVMessages_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/TxNotifDetails.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Consts.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Frame_m.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceTableAccess.h \
	PowerSystem/DMS.h \
	common/FiWiGeneralConfigs.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	Wireless/BasicIeee80211Mac.h \
	common/FiWiTrafficMatrix.h \
	PowerSystem/OpenDSS.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INETMANET_PROJ)/src/base/PassiveQueueBase.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h \
	$(INETMANET_PROJ)/src/linklayer/mfcore/WirelessMacBase.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/util/PowerControlManager.h \
	common/FiWiTrafGen.h \
	$(INETMANET_PROJ)/src/linklayer/radio/errormodel/Ieee80211DataRate.h \
	$(INETMANET_PROJ)/src/linklayer/contract/RadioState.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	$(INETMANET_PROJ)/src/linklayer/contract/PhyControlInfo_m.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMAC.h \
	Wireless/IBasic80211Mac.h \
	common/MyUtil.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	$(INETMANET_PROJ)/src/util/PowerControlMessages_m.h \
	common/FiWiRoutingTable.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/applications/ethernet/EtherApp_m.h \
	Wireless/BasicIeee80211MgmtAP.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h
$O/common/LinklessMAC.o: common/LinklessMAC.cc \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMACBase.h \
	$(INETMANET_PROJ)/src/util/PowerControlManager.h \
	common/LinklessMAC.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	$(INETMANET_PROJ)/src/base/IPowerControl.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMAC.h \
	$(INETMANET_PROJ)/src/linklayer/contract/TxNotifDetails.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	$(INETMANET_PROJ)/src/util/PowerControlMessages_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h
$O/common/NoDelaySwitch.o: common/NoDelaySwitch.cc \
	common/NoDelaySwitch.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h
$O/common/SimpleQueue.o: common/SimpleQueue.cc \
	PON/common_pon/IExtPassiveQueue.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMACBase.h \
	PON/common_pon/EPON_mac.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	PON/common_pon/EPON_messages_m.h \
	PON/common_pon/PONUtil.h \
	$(INETMANET_PROJ)/src/base/IPowerControl.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	PON/OLT/IOLTMgmt.h \
	$(INETMANET_PROJ)/src/linklayer/contract/TxNotifDetails.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	PON/OLT/ONUTableEntry.h \
	common/FiWiGeneralConfigs.h \
	common/MyAbstractQueue.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	PON/ONU/ONUMacCtl.h \
	PON/OLT/ONUReservation.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(VLANS_PROJ)/src/linklayer/EtherVlanDefs.h \
	PON/common_pon/MPCP_codes.h \
	common/SimpleQueue.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	PON/common_pon/EPON_CtrlInfo.h \
	$(INETMANET_PROJ)/src/util/headerserializers/headers/ethernet.h \
	$(INETMANET_PROJ)/src/util/PowerControlManager.h \
	PON/common_pon/MPCPTools.h \
	PON/Lambda.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	$(INETMANET_PROJ)/src/util/PowerControlMessages_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h
$O/common/FiWiTrafGen.o: common/FiWiTrafGen.cc \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMACBase.h \
	common/LinklessMAC.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	PON/common_pon/EPON_messages_m.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.h \
	$(INETMANET_PROJ)/src/base/IPowerControl.h \
	$(INETMANET_PROJ)/src/util/RawStat.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/linklayer/contract/TxNotifDetails.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Consts.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceTableAccess.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Frame_m.h \
	common/FiWiGeneralConfigs.h \
	Wireless/BasicIeee80211Mac.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	common/FiWiTrafficMatrix.h \
	$(VLANS_PROJ)/src/linklayer/EtherVlanDefs.h \
	$(INETMANET_PROJ)/src/base/PassiveQueueBase.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h \
	$(INETMANET_PROJ)/src/linklayer/mfcore/WirelessMacBase.h \
	$(INETMANET_PROJ)/src/util/PowerControlManager.h \
	$(INETMANET_PROJ)/src/linklayer/radio/errormodel/Ieee80211DataRate.h \
	$(INETMANET_PROJ)/src/linklayer/contract/RadioState.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMAC.h \
	$(INETMANET_PROJ)/src/util/PowerControlMessages_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h \
	$(INETMANET_PROJ)/src/networklayer/queue/IQoSClassifier.h \
	$(INETMANET_PROJ)/src/base/FSMA.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtFrames_m.h \
	$(VLANS_PROJ)/src/linklayer/EtherMacVlan.h \
	PON/common_pon/EPON_mac.h \
	PowerSystem/PowerSysNode.h \
	PowerSystem/PEVMessages_m.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	PowerSystem/DMS.h \
	PowerSystem/OpenDSS.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	PON/common_pon/EPON_CtrlInfo.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/util/headerserializers/headers/ethernet.h \
	common/FiWiTrafGen.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	common/MyUtil.h \
	Wireless/IBasic80211Mac.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	common/FiWiRoutingTable.h \
	$(INETMANET_PROJ)/src/applications/ethernet/EtherApp_m.h \
	Wireless/BasicIeee80211MgmtAP.h
$O/common/MyWeightedFairQueue.o: common/MyWeightedFairQueue.cc \
	$(INETMANET_PROJ)/src/networklayer/queue/IQoSClassifier.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/base/FSMA.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtFrames_m.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/util/RawStat.h \
	PowerSystem/PowerSysNode.h \
	PowerSystem/PEVMessages_m.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Consts.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	PowerSystem/DMS.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Frame_m.h \
	common/MyAbstractQueue.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	Wireless/BasicIeee80211Mac.h \
	common/FiWiTrafficMatrix.h \
	PowerSystem/OpenDSS.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INETMANET_PROJ)/src/base/PassiveQueueBase.h \
	common/FiWiMessages_m.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h \
	$(INETMANET_PROJ)/src/linklayer/mfcore/WirelessMacBase.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	common/FiWiTrafGen.h \
	$(INETMANET_PROJ)/src/linklayer/radio/errormodel/Ieee80211DataRate.h \
	$(INETMANET_PROJ)/src/linklayer/contract/RadioState.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	Wireless/IBasic80211Mac.h \
	common/MyUtil.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	common/MyWeightedFairQueue.h \
	common/FiWiRoutingTable.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/applications/ethernet/EtherApp_m.h \
	Wireless/BasicIeee80211MgmtAP.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h
$O/common/FiWiAdmissionControl.o: common/FiWiAdmissionControl.cc \
	common/FiWiMessages_m.h \
	common/FiWiAdmissionControl.h
$O/common/FiWiFastAdmissionControl.o: common/FiWiFastAdmissionControl.cc \
	$(INETMANET_PROJ)/src/networklayer/queue/IQoSClassifier.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/base/FSMA.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtFrames_m.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/util/RawStat.h \
	PowerSystem/PowerSysNode.h \
	PowerSystem/PEVMessages_m.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Consts.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Frame_m.h \
	PowerSystem/DMS.h \
	common/MyAbstractQueue.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	Wireless/BasicIeee80211Mac.h \
	common/FiWiTrafficMatrix.h \
	PowerSystem/OpenDSS.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	$(INETMANET_PROJ)/src/base/PassiveQueueBase.h \
	common/FiWiMessages_m.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h \
	$(INETMANET_PROJ)/src/linklayer/mfcore/WirelessMacBase.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	common/FiWiTrafGen.h \
	$(INETMANET_PROJ)/src/linklayer/radio/errormodel/Ieee80211DataRate.h \
	$(INETMANET_PROJ)/src/linklayer/contract/RadioState.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	common/FiWiFastAdmissionControl.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	Wireless/IBasic80211Mac.h \
	common/MyUtil.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	common/FiWiRoutingTable.h \
	$(INETMANET_PROJ)/src/applications/ethernet/EtherApp_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	Wireless/BasicIeee80211MgmtAP.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h
$O/common/VeryBasicApp.o: common/VeryBasicApp.cc \
	common/FiWiMessages_m.h \
	common/StatsCollector.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	common/VeryBasicApp.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h
$O/common/FiWiRoutingTable.o: common/FiWiRoutingTable.cc \
	$(INETMANET_PROJ)/src/networklayer/queue/IQoSClassifier.h \
	$(INETMANET_PROJ)/src/base/FSMA.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtFrames_m.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	PON/common_pon/PONUtil.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/util/RawStat.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Consts.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Frame_m.h \
	common/FiWiGeneralConfigs.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	Wireless/BasicIeee80211Mac.h \
	common/FiWiTrafficMatrix.h \
	$(INETMANET_PROJ)/src/base/PassiveQueueBase.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h \
	$(INETMANET_PROJ)/src/linklayer/mfcore/WirelessMacBase.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/linklayer/radio/errormodel/Ieee80211DataRate.h \
	$(INETMANET_PROJ)/src/linklayer/contract/RadioState.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	PON/Lambda.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	common/MyUtil.h \
	Wireless/IBasic80211Mac.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	common/FiWiRoutingTable.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	Wireless/BasicIeee80211MgmtAP.h
$O/common/FiWiGeneralConfigs.o: common/FiWiGeneralConfigs.cc \
	$(INETMANET_PROJ)/src/networklayer/queue/IQoSClassifier.h \
	$(INETMANET_PROJ)/src/base/FSMA.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtFrames_m.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	PowerSystem/PowerSysNode.h \
	$(INETMANET_PROJ)/src/util/RawStat.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Consts.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	PowerSystem/DMS.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Frame_m.h \
	common/FiWiGeneralConfigs.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	Wireless/BasicIeee80211Mac.h \
	common/FiWiTrafficMatrix.h \
	PowerSystem/OpenDSS.h \
	$(INETMANET_PROJ)/src/base/PassiveQueueBase.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h \
	$(INETMANET_PROJ)/src/linklayer/mfcore/WirelessMacBase.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	PON/common_pon/EPON_CtrlInfo.h \
	$(INETMANET_PROJ)/src/linklayer/radio/errormodel/Ieee80211DataRate.h \
	$(INETMANET_PROJ)/src/linklayer/contract/RadioState.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	common/MyUtil.h \
	Wireless/IBasic80211Mac.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	common/FiWiRoutingTable.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	Wireless/BasicIeee80211MgmtAP.h
$O/common/StatsCollector.o: common/StatsCollector.cc \
	common/StatsCollector.h
$O/common/MyUtil.o: common/MyUtil.cc \
	$(INETMANET_PROJ)/src/networklayer/queue/IQoSClassifier.h \
	$(INETMANET_PROJ)/src/base/FSMA.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtFrames_m.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.h \
	PON/common_pon/PONUtil.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/util/RawStat.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Consts.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Frame_m.h \
	common/FiWiGeneralConfigs.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	Wireless/BasicIeee80211Mac.h \
	common/FiWiTrafficMatrix.h \
	$(INETMANET_PROJ)/src/base/PassiveQueueBase.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h \
	$(INETMANET_PROJ)/src/linklayer/mfcore/WirelessMacBase.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/linklayer/radio/errormodel/Ieee80211DataRate.h \
	$(INETMANET_PROJ)/src/linklayer/contract/RadioState.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	PON/Lambda.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	common/MyUtil.h \
	Wireless/IBasic80211Mac.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	common/FiWiRoutingTable.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	Wireless/BasicIeee80211MgmtAP.h
$O/common/FiWiMessages_m.o: common/FiWiMessages_m.cc \
	common/FiWiMessages_m.h
$O/common/FiWiTrafficMatrix.o: common/FiWiTrafficMatrix.cc \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherFrame_m.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMACBase.h \
	$(INETMANET_PROJ)/src/base/NotificationBoard.h \
	PON/common_pon/EPON_messages_m.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.h \
	$(INETMANET_PROJ)/src/base/IPowerControl.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceEntry.h \
	$(INETMANET_PROJ)/src/util/RawStat.h \
	$(INETMANET_PROJ)/src/linklayer/contract/TxNotifDetails.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Consts.h \
	$(INETMANET_PROJ)/src/base/ModuleAccess.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceTableAccess.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mac/Ieee80211Frame_m.h \
	common/FiWiGeneralConfigs.h \
	$(INETMANET_PROJ)/src/networklayer/common/InterfaceToken.h \
	Wireless/BasicIeee80211Mac.h \
	common/FiWiTrafficMatrix.h \
	$(VLANS_PROJ)/src/linklayer/EtherVlanDefs.h \
	$(INETMANET_PROJ)/src/base/PassiveQueueBase.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h \
	$(INETMANET_PROJ)/src/linklayer/mfcore/WirelessMacBase.h \
	$(INETMANET_PROJ)/src/util/PowerControlManager.h \
	$(INETMANET_PROJ)/src/linklayer/radio/errormodel/Ieee80211DataRate.h \
	$(INETMANET_PROJ)/src/linklayer/contract/RadioState.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/EtherMAC.h \
	$(INETMANET_PROJ)/src/util/PowerControlMessages_m.h \
	$(INETMANET_PROJ)/src/linklayer/contract/MACAddress.h \
	$(INETMANET_PROJ)/src/linklayer/ethernet/Ethernet.h \
	$(INETMANET_PROJ)/src/networklayer/queue/IQoSClassifier.h \
	$(INETMANET_PROJ)/src/base/FSMA.h \
	$(INETMANET_PROJ)/src/linklayer/ieee80211/mgmt/Ieee80211MgmtFrames_m.h \
	$(VLANS_PROJ)/src/linklayer/EtherMacVlan.h \
	PON/common_pon/EPON_mac.h \
	PowerSystem/PowerSysNode.h \
	PowerSystem/PEVMessages_m.h \
	$(INETMANET_PROJ)/src/base/INotifiable.h \
	PowerSystem/DMS.h \
	PowerSystem/OpenDSS.h \
	$(INETMANET_PROJ)/src/linklayer/contract/Ieee802Ctrl_m.h \
	PON/common_pon/EPON_CtrlInfo.h \
	$(INETMANET_PROJ)/src/base/INETDefs.h \
	$(INETMANET_PROJ)/src/util/headerserializers/headers/ethernet.h \
	common/FiWiTrafGen.h \
	$(INETMANET_PROJ)/src/base/IPassiveQueue.h \
	$(INETMANET_PROJ)/src/networklayer/common/IInterfaceTable.h \
	common/MyUtil.h \
	Wireless/IBasic80211Mac.h \
	$(INETMANET_PROJ)/src/base/NotifierConsts.h \
	common/FiWiRoutingTable.h \
	$(INETMANET_PROJ)/src/applications/ethernet/EtherApp_m.h \
	Wireless/BasicIeee80211MgmtAP.h

