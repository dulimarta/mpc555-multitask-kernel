include ../defs.mak
AS       = $(CROSS_TARGET)as
OBJCOPY  = $(CROSS_TARGET)objcopy
LD       = $(CROSS_TARGET)ld

#ASFLAGS=-Wa,-ahl -D$(TARGET_MEM)
CPPFLAGS += -I. -I../libmpc -I../kernel -I../models -I-

ifneq ($(TARGET_MEM),flash)
LDSCRIPT=link-ram.ld
else
LDSCRIPT=link-flash.ld
endif

LDFLAGS = -v --discard-locals \
-L../libmpc -L../kernel -L../models \
--library-path=. --nmagic --no-whole-archive \
-Map $(@:.elf=).map --cref -nostartfiles -nostdlib \
--script=$(LDSCRIPT) --sort-common -t \
--warn-common --warn-constructors --warn-section-align 
#LDFLAGS += --strip-all --strip-debug

COPYFLAGS = -v --strip-all --strip-debug --strip-unneeded --discard-locals

LIBS = ../kernel/libk.a ../libmpc/libmpc.a ../models/libmodel.a
LDLIBS = -lk -lmpc -lmodel -lm -lc

TARGET = task.txt main.txt rx_test.txt

CC_SOURCES = PIDControl.cc PIThand.cc config.cc
CPP_SOURCES = main.cpp WallClimber.cpp Joint.cpp Suction.cpp SJoint.cpp \
	CommandInt.cpp
AS_SOURCES = param.S

TASK_CC_SOURCES = task.cc
TASK_C_SOURCES =
CXXFLAGS += -Wno-unused

OBJS = $(CC_SOURCES:.cc=.o) $(CPP_SOURCES:.cpp=.o) $(AS_SOURCES:.S=.o)
TASK_OBJS = $(TASK_CC_SOURCES:.cc=.o) $(TASK_C_SOURCES:.c=.o) $(AS_SOURCES:.S=.o)

.PRECIOUS: %.elf
.SUFFIXES: .elf

.PHONY: all
all: $(TARGET)

task.elf: $(LIBS) $(TASK_OBJS)
	$(LD) $(LDFLAGS) $(TASK_OBJS) $(LDLIBS) -o $@

main.elf: $(LIBS) $(OBJS)
	$(COMPILE.cc) main.cpp
	$(LD) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@

rx_test.elf: rx_test.o
	$(LD) $(LDFLAGS) rx_test.o $(LDLIBS) -o $@


../libmpc/libmpc.a:
	make -C ../libmpc libmpc.a

../kernel/libk.a:
	make -C ../kernel libk.a

../models/libmodel.a:
	make -C ../models libmodel.a

%.txt : %.elf
	$(OBJCOPY) $(COPYFLAGS) -O srec $< $@.tmp
	tr -d '\r' < $@.tmp | ./s19.pl -f > $@
	rm $@.tmp

%.elf : %.o ../libmpc/libmpc.a
	$(LD) $(LDFLAGS) $< $(LDLIBS) -o $@

.c.o:
	$(COMPILE.c) $(OUTPUT_OPTION) $<

.cc.o:
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

clean:
	rm *.o *.O *~ *.elf $(TARGET)

.PHONY: depend
depend: $(CPP_SOURCES) $(CC_SOURCES) rx_test.c
	$(CC) -MM $(CPPFLAGS) $(CC_SOURCES) $(CPP_SOURCES) > .$@
	$(CC) -MM $(CPPFLAGS) $(TASK_CC_SOURCES) $(TASK_C_SOURCES) >> .$@

include .depend
