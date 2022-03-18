include ../defs.mak
AS       = $(CROSS_TARGET)as
OBJCOPY  = $(CROSS_TARGET)objcopy
LD       = $(CROSS_TARGET)ld

#ASFLAGS=-Wa,-ahl -D$(TARGET_MEM)
CPPFLAGS += -I. -I../libmpc -I-
LDFLAGS=-v --discard-all --discard-locals \
--library-path=.  --nmagic --no-whole-archive \
-Map $@.map -nostartfiles -nostdlib \
--script=link-$(TARGET_MEM).ld --sort-common --trace \
--warn-common --warn-constructors --warn-section-align 
#LDLIBS = /usr/local/powerpc/lib/gcc-lib/powerpc-eabi/2.95.2/libgcc.a ../libmpc/libmpc.a -lm -lc
LDLIBS = ../libmpc/libmpc.a -lm -lc

TARGET = main.elf main.txt

C_SOURCES = 555cmf.c serial.c parallel_trd_array.c

OBJS = $(C_SOURCES:.c=.o)

all: $(TARGET)

%.elf: $(OBJS) ../libmpc/libmpc.a
	$(LD) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@

../libmpc/libmpc.a:
	make -C ../libmpc libmpc.a

%.txt: %.elf
	$(OBJCOPY) --strip-all --strip-debug --strip-unneeded -O srec $< $@.tmp
	tr -d '\r' < $@.tmp > $@
	rm $@.tmp

clean:
	rm *.o *.O *~ $(TARGET)

depend: $(CPP_SOURCES) $(CC_SOURCES)
	$(CC) -MM $(CPPFLAGS) $(CC_SOURCES) $(CPP_SOURCES) > .$@

include .depend
