bin_PROGRAMS = robnav robnav.txt
robnav_SOURCES = main.cpp PIThand.cc PIDControl.cc CommandInt.cpp \
   WallClimber.cpp Suction.cpp Joint.cpp Controller.cpp \
   SJoint.cpp DSignal.cc ASignal.cc param.S

robnav_HEAD=ASignal.h DSignal.h CommandInt.h SJoint.h Joint.h Suction.h utility.h PIDControl.h WallClimber.h Controller.h Robot.h

CROSS_TARGET=powerpc-eabi-

CC=$(CROSS_TARGET)gcc
CXX=$(CROSS_TARGET)g++
#robnav_LINK=$(CROSS_TARGET)ld
robnav_LINK=/usr/local/powerpc/lib/gcc-lib/powerpc-eabi/2.95.2/collect2

CFLAGS= -g -Wall -Wsign-compare -fsigned-char \
   -Wpointer-arith -Wcast-qual -Wshadow -Werror \
   -memb -mno-eabi -fomit-frame-pointer \
	-mhard-float -fstack-check -Dram    # -Dflash
CPPFLAGS = -I. -I../libmpc -I../libpic -I-
ASFLAGS=-c -Dram
robnav_LDFLAGS= --warn-common --warn-section-align --noinhibit-exec \
-t --no-whole-archive -T link-ram.ld -o $@
robnav_LDADD = ../libmpc/libmpc.a -lm
CXXFLAGS=$(CFLAGS) -fno-exceptions -fno-rtti -Woverloaded-virtual

robnav.txt: robnav
	$(CROSS_TARGET)objcopy -S -O srec $< $@.tmp
	tr -d '\r' < $@.tmp > $@
	rm $@.tmp

depend: $(robnav_SOURCES) $(robnav_HEAD)
	cp Makefile Makefile.bak
	sed '/^# DO NOT DELETE/,$$d' < Makefile.bak > Makefile
	echo '# DO NOT DELETE' >> Makefile
	$(CC) -MM $(CPPFLAGS) $(robnav_SOURCES) >> Makefile
