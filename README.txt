Highlights User Defined Types/Functions and Constants
for the C Programming language

To use run make/make install

There are 3 new highlights you may want to put in
your colors file:

hi userdefType     ctermfg=3
hi userdefFunction ctermfg=66
hi userdefConstant ctermfg=102
hi userdefClass    ctermfg=4
hi userdefPackage  ctermfg=2

This will add the function DetectTypes() and the command :HiTypes to vim, so
you may also want to add the line.

au BufWrite *.c call DetectTypes() to your vimrc

To manually highlight the file (as it may take some time to highlight) then run :HiTypes

If you need to add extra arguments to the compile flags such as extra include directories
you can specify the CFLAGS to send to the compiler via the file .userdef_(c|java)_config

so .userdef_c_config might look like:

    -I include -I /path/to/include/dir -D DEBUG

or .userdef_java_config loop like:
    
    --classpath /path/to/classpath.jar
