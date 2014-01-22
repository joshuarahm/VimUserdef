# it is important for every test moudule to have a make file called Makefile.
# this allows the module to be built independently of all the other
# modules.
#
# Each module must create a static library (.a) file. The name of the
# library is not important; it just needs to follow the C standard
# lib*.a standard.
#
# At the end, the make process will recursively find all the .a files
# to compile into a large shared library.
#
# If the make process for a module fails, it will not halt the parent
# compilation, so be aware that the module may not build, but the
# compilation will still finish.
#
# The language the module is written in is not important as long as
# it is able to be called from the C code itself. I would recommend
# using C, C++ or Objective C, but modules can also be written in
# say, Haskell or maybe even Python.
#
# Environment variables that are passed in are CC, CFLAGS, and AR (archiver)


# Every module should have an all make rule. This will
# be called by name from the top-level make file
all: cpp_radiator.o
	$(AR) -r libcppradiator.a $^

# include all of the object files here
cpp_radiator.o: cpp_radiator.c
	$(CC) $(CFLAGS) -o $@ -c $<

# All Makefiles should contain a clean rule.
# This rule should remove all of the object
# files and the library produced
clean:
	rm -rf cpp_radiator.o libcppradiator.a 
