include defs.mak

SUBDIRS = libmpc kernel models src

.PHONY: all clean depend doc

all depend:
	for s in $(SUBDIRS); do \
		(cd $$s; $(MAKE) $@) \
	done

clean:
	-rm *~
	for s in $(SUBDIRS); do \
		(cd $$s; $(MAKE) $@) \
	done

doc: doc/html/index.html

doc/html/index.html : libmpc/*.h libmpc/*.c src/*.cc src/*.cpp src/*.h Doxyfile
	doxygen 2>doxygen.log
