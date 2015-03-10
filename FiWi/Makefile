#
# OMNeT++/OMNEST Makefile for libFiWimodified
#
# This file was generated with the command:
#  opp_makemake -f --deep --make-so -O out -d src -X. -L/home/intissar/FiWi/Vlans/out/$(CONFIGNAME)/src -L/home/intissar/simulator/inetmanet/out/$(CONFIGNAME)/src -L./out/$(CONFIGNAME)/src -lVlans -linet -KVLANS_PROJ=/home/intissar/FiWi/Vlans -KINETMANET_PROJ=/home/intissar/simulator/inetmanet
#

# Name of target to be created (-o option)
TARGET = libFiWimodified$(SHARED_LIB_SUFFIX)

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS = -L$(VLANS_PROJ)/out/$(CONFIGNAME)/src -L$(INETMANET_PROJ)/out/$(CONFIGNAME)/src -Lout/$(CONFIGNAME)/src  -lVlans -linet
LIBS += -Wl,-rpath,`abspath $(VLANS_PROJ)/out/$(CONFIGNAME)/src` -Wl,-rpath,`abspath $(INETMANET_PROJ)/out/$(CONFIGNAME)/src` -Wl,-rpath,`abspath out/$(CONFIGNAME)/src`

# Output directory
PROJECT_OUTPUT_DIR = out
PROJECTRELATIVE_PATH =
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

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

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $(TARGET)

$(TARGET) : $O/$(TARGET)
	$(LN) $O/$(TARGET) .

$O/$(TARGET):  submakedirs $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	$(SHLIB_LD) -o $O/$(TARGET)   $(EXTRA_OBJS) $(LIBS) $(OMNETPP_LIBS) $(LDFLAGS)
	$(SHLIB_POSTPROCESS) $O/$(TARGET)

submakedirs:  src_dir

.PHONY: src
src: src_dir

src_dir:
	cd src && $(MAKE)

msgheaders:
	cd src && $(MAKE) msgheaders

clean:
	-rm -rf $O
	-rm -f FiWimodified FiWimodified.exe libFiWimodified.so libFiWimodified.a libFiWimodified.dll libFiWimodified.dylib

	-cd src && $(MAKE) clean

cleanall: clean
	-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	-cd src && if [ -f Makefile ]; then $(MAKE) depend; fi

