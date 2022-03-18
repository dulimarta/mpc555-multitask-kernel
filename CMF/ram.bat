dcc -tPPC555EN:simple serial.c 555cmf.c 555mon.c parallel_trd_array.c -o 555mon.elf -g -XO -Wmram.dld -m2 > 555mon.map
ddump -R -v -o 555mon.s19 555mon.elf
