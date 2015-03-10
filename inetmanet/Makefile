all: checkmakefiles
	cd src && $(MAKE)

clean: checkmakefiles
	cd src && $(MAKE) clean

cleanall: checkmakefiles
	cd src && $(MAKE) MODE=release clean
	cd src && $(MAKE) MODE=debug clean
	rm -f src/Makefile

makefiles:
	cd src && opp_makemake -f --deep -X underTest --make-so -o inet -O out $$NSC_VERSION_DEF
 
makefiles_exe:
	cd src && opp_makemake -f --deep -X underTest -o inet -O out $$NSC_VERSION_DEF
 
makefilesWimax:
	cd src && opp_makemake -f --deep -X underTest/applications/Ieee802154TestApp -X underTest/linklayer/ieee802154 -X underTest/wpan --make-so -o inet -O out $$NSC_VERSION_DEF
 
makefiles_exeWimax:
	cd src && opp_makemake -f --deep -X underTest/applications/Ieee802154TestApp -X underTest/linklayer/ieee802154 -X underTest/wpan -o inet -O out $$NSC_VERSION_DEF
 
makeall:
	cd src && opp_makemake -f --deep --make-so -o inet -O out $$NSC_VERSION_DEF
 
makeall_exe:
	cd src && opp_makemake -f --deep -o inet -O out $$NSC_VERSION_DEF

 
checkmakefiles:
	@if [ ! -f src/Makefile ]; then \
	echo; \
	echo '======================================================================='; \
	echo 'src/Makefile does not exist. Please use "make makefiles" to genrate it!'; \
	echo '======================================================================='; \
	echo; \
	exit 1; \
	fi

doxy:
	doxygen doxy.cfg

tcptut:
	cd doc/src/tcp && $(MAKE)
