# $Id: Makefile.am,v 1.1 2000/04/27 10:12:55 dulimart Exp $
# This is an input for the 'automake' program

noinst_LIBRARIES = libmpc.a
libmpc_a_SOURCES = ctors.c intr.c io.c lcd.c mios.c mpc555.c \
      qadc.c qsm.c stdlib.c tpu.c uimb.c usiu.c memory.c \
      crt0.S except.S stream.cc strstream.cc \
      MIOS_IO.cc QADC_Sig.cc MPC.cc mem.cc

my_HEADER=intr.h io.h lcd.h mios.h mpc555.h qadc.h \
		qsm.h tpu.h uimb.h usiu.h memory.h stream.h strstream.h \
      MIOS_IO.h QADC_Sig.h MPC.h stdlib.h

CC=powerpc-eabi-gcc
CXX=powerpc-eabi-gcc
AS=powerpc-eabi-as
AR=powerpc-eabi-ar
RANLIB=powerpc-eabi-ranlib
LD=powerpc-eabi-ld
CFLAGS= -Wall -Wsign-compare -Werror -Wshadow \
   -Wpointer-arith -Wcast-qual -Wid-clash-16 \
   -g -memb -mno-eabi -fomit-frame-pointer \
	-mhard-float -fstack-check -Dram
CPPFLAGS = -I. -I../src -I-
ASFLAGS=-c -g -Dram
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -Woverloaded-virtual

depend: $(libmpc_a_SOURCES) $(my_HEADER) 
	cp Makefile Makefile.bak
	sed '/^# DO NOT DELETE/,$$d' < Makefile.bak > Makefile
	echo '# DO NOT DELETE' >> Makefile
	$(CC) -MM $(CPPFLAGS) $(libmpc_a_SOURCES) >> Makefile
