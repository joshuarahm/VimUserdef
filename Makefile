#!/bin/bash

SHELL=/bin/bash

# The C compiler to use. Allows the user specify
# the compiler on as an environment variable
CC?=gcc
CPPC?=g++

# The archiver. This is used by the modules to
# archive their object code
AR?=ar

# Optimization flags. May be specified in the
# environment but defaults to full debugging
#
# This flag may also be used to pass additional
# CFLAGS to the compiler
OPTFLAGS?=-g3 -ggdb

# The flags to compile C files with. 
CFLAGS=-Wall -Wextra -I $(shell pwd)/src/include/ -I $(shell pwd)/src/ $(OPTFLAGS) -fPIC

# the objects to compile on the top
# level
OBJECTS=obs/radiation.o obs/blocking_queue.o\
	obs/strbuf.o obs/subprocess.o\
	obs/serverimpl.o obs/sequentialimpl.o\
	obs/sset.o obs/PcreRegex.o obs/StreamMatcher.o \
	obs/Mutex.o obs/Condition.o obs/Thread.o obs/Time.o

# The name of the library to produce
SHARED_OBJECT=libvimradiation.so.1
STATIC_LIB=libvimradiationstat.a

# The modules that are included in this build so
# the makefile knows what to build

# export some flags to tell the modules
# what some of the configuration is
export CC
export CPPC
export CFLAGS

all: setup $(OBJECTS) modules 
	$(CPPC) -shared -Wl,--whole-archive $(shell find modules -name lib*.a ) \
	  -Wl,--no-whole-archive,-soname,$(SHARED_OBJECT) -o $(SHARED_OBJECT) $(OBJECTS)
	$(AR) -r $(STATIC_LIB) $(OBJECTS)

# use a clever perl script to pick the modules
# that we need to compile
modules: INCLUDED_MODULES=$(shell gcc -x c -E src/modules/modules.inc | perl -lne \
	'if( $$_ =~ /INCLUDE_MODULE\(\s*(\w+)\s*\)/ ) { \
		print "src/modules/$$1"\
	}')

# Iterate and compile all of the modules
modules:
	@for i in $(INCLUDED_MODULES) ; do \
		if [[ -d $$i && -f $$i/Makefile ]] ; then \
			cd $$i ;   \
			make all || exit 1; \
			cd ../../.. ; \
		fi \
	done

clean:
	@for i in src/modules/* ; do \
		if [[ -d $$i && -f $$i/Makefile ]] ; then \
			cd $$i ;   \
			make clean ; \
			cd ../../.. ; \
		fi \
	done
	rm -rf $(OBJECTS) $(SHARED_OBJECT)

setup:
	mkdir -p obs/

# compile the object files needed
obs/radiation.o: src/radiation/radiation.c
	$(CC) $(CFLAGS) -o $@ -c $<

obs/blocking_queue.o: src/util/blocking_queue.c
	$(CC) $(CFLAGS) -o $@ -c $<

obs/sset.o: src/util/sset.c
	$(CC) $(CFLAGS) -o $@ -c $<

obs/strbuf.o: src/util/strbuf.c
	$(CC) $(CFLAGS) -o $@ -c $<

obs/subprocess.o: src/util/subprocess.c
	$(CC) $(CFLAGS) -o $@ -c $<

obs/serverimpl.o: src/radiation/impl/serverimpl.c
	$(CC) $(CFLAGS) -o $@ -c $<

obs/sequentialimpl.o: src/radiation/impl/sequentialimpl.c
	$(CC) $(CFLAGS) -o $@ -c $<

obs/Mutex.o: src/cpp/Mutex.cpp
	$(CPPC) $(CFLAGS) -o $@ -c $<

obs/PcreRegex.o: src/cpp/PcreRegex.cpp
	$(CPPC) $(CFLAGS) -o $@ -c $<

obs/Condition.o: src/cpp/Condition.cpp
	$(CPPC) $(CFLAGS) -o $@ -c $<

obs/StreamMatcher.o: src/cpp/StreamMatcher.cpp
	$(CPPC) $(CFLAGS) -o $@ -c $<

obs/Thread.o: src/cpp/Thread.cpp
	$(CPPC) $(CFLAGS) -o $@ -c $<

obs/Time.o: src/cpp/Time.cpp
	$(CPPC) $(CFLAGS) -o $@ -c $<

install: all
	./install.sh

