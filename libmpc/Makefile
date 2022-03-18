include ../defs.mak

AS      = $(CROSS_TARGET)as
AR      = $(CROSS_TARGET)ar
RANLIB  = $(CROSS_TARGET)ranlib

CPPFLAGS += -I. -I../models -I../kernel -I-
ASFLAGS =-g -Wa,-ah

TARGET = libmpc.a

C_SOURCES = iodev.c tpu.c usiu.c mpc555.c uimb.c qadc.c \
   intr.c ctors.c memory.c io.c qsm.c mios.c stdlib.c cmf.c
CC_SOURCES = stream.cc MIOS_IO.cc MIOS_PWM.cc QADC_Sig.cc QADC_IO.cc \
   MPC.cc mem.cc sleep.cc
AS_SOURCES = crt0.S except.S arch.S

LIBOBJS = $(AS_SOURCES:.S=.o) $(CC_SOURCES:.cc=.o) $(C_SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(LIBOBJS)
	$(AR) -r $@ $(LIBOBJS)
	$(RANLIB) $@

clean:
	echo $(MAKEFILES) $(MAKE)
	rm *.o *.O *~ $(TARGET)

depend: $(C_SOURCES) $(CC_SOURCES)
	$(CC) -MM $(CPPFLAGS) $(C_SOURCES) $(CC_SOURCES) > .$@

include .depend
