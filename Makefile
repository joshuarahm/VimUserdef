#!/bin/bash

SHELL=/bin/bash

# The C compiler to use. Allows the user specify
# the compiler on as an environment variable
CC?=gcc

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
CFLAGS=-Wall -Wextra -I $(shell pwd)/src/include/ $(OPTFLAGS) -fPIC

# the objects to compile on the top
# level
OBJECTS=obs/radiation.o obs/blocking_queue.o obs/strbuf.o

# The name of the library to produce
SHARED_OBJECT=libvimradiation.so.1

# The modules that are included in this build so
# the makefile knows what to build

# export some flags to tell the modules
# what some of the configuration is
export CC
export CFLAGS

all: setup modules $(OBJECTS)
	$(CC) -shared -Wl,--whole-archive $(shell find . -name lib*.a ) \
	  -Wl,--no-whole-archive,-soname,$(SHARED_OBJECT) -o $(SHARED_OBJECT) $(OBJECTS)

# use a clever perl script to pick the modules
# that we need to compile
modules: INCLUDED_MODULES=$(shell perl -lne \
	'if( $$_ =~ /INCLUDE_MODULE\(\s*(\w+)\s*\)/ ) { \
		print "src/modules/$$1"\
	}'\
	src/modules/modules.inc)

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
obs/radiation.o: src/radiation.c
	$(CC) $(CFLAGS) -o $@ -c $<

obs/blocking_queue.o: src/blocking_queue.c
	$(CC) $(CFLAGS) -o $@ -c $<

obs/strbuf.o: src/strbuf.c
	$(CC) $(CFLAGS) -o $@ -c $<

install: all
	./install.sh

