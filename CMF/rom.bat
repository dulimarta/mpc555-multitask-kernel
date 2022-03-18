das -tPPC555EN:simple -l init.s
dcc -tPPC555EN:simple -Wsinit.o serial.c 555cmf.c 555mon.c parallel_trd_array.c -o 555mon.elf -g -XO -Wmrom.dld -m2 > 555mon.map
ddump -R -v -o 555mon.s19 555mon.elf
sortsrec 555mon.s19
copy ~new.s19 555mon.s19
del ~new.s19
