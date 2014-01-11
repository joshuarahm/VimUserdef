CPPC?=g++
AR?=ar
OPTFLAGS?=-g3 -ggdb
CFLAGS= -Wall -Wextra -Isrc/include/ $(OPTFLAGS) -fPIC
OBJECTS=obs/radiation.o obs/blocking_queue.o

SHARED_OBJECT=libvimradiation.so.1

all: setup $(OBJECTS)
	$(CPPC) -shared -Wl,-soname,$(SHARED_OBJECT) -o $(SHARED_OBJECT) $(OBJECTS)

clean:
	rm -rf obs/ $(SHARED_OBJECT)

genmake:
	find . -name genmake.sh -exec {} \;

setup:
	mkdir -p obs/

obs/radiation.o: src/radiation.c
	$(CPPC) $(CFLAGS) -o $@ -c $<

obs/blocking_queue.o: src/blocking_queue.c
	$(CPPC) $(CFLAGS) -o $@ -c $<

