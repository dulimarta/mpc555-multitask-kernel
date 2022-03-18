include ../defs.mak

AS      = $(CROSS_TARGET)as
AR      = $(CROSS_TARGET)ar
RANLIB  = $(CROSS_TARGET)ranlib

CPPFLAGS += -I. -I-
ASFLAGS =-g -Wa,-ah

TARGET = libmodel.a

CC_SOURCES = ASignal.cc DSignal.cc Command.cc Robot.cc
CPP_SOURCES = Controller.cpp

LIBOBJS = $(CC_SOURCES:.cc=.o) $(CPP_SOURCES:.cpp=.o)

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
