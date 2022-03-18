include ../defs.mak

AS      = $(CROSS_TARGET)as
AR      = $(CROSS_TARGET)ar
RANLIB  = $(CROSS_TARGET)ranlib

CPPFLAGS += -I. -I../libmpc -I-
ASFLAGS =-g

TARGET = libk.a time.ps
C_SOURCES = 
CC_SOURCES = Clock.cc Task.cc Scheduler.cc
AS_SOURCES = sched.S sync.S

LIBOBJS = $(CC_SOURCES:.cc=.o) $(C_SOURCES:.c=.o) $(AS_SOURCES:.S=.o) 

all: $(TARGET)

libk.a : $(LIBOBJS)
	$(AR) -r $@ $(LIBOBJS)
	$(RANLIB) $@

clean:
	echo $(MAKEFILES) $(MAKE)
	rm a.out *.o *.O *~ $(TARGET)

depend: $(C_SOURCES) $(CC_SOURCES)
	$(CC) -MM $(CPPFLAGS) $(C_SOURCES) $(CC_SOURCES) > .$@

%.dvi: %.tex
	latex $<

%.ps: %.dvi
	dvips $<

include .depend
