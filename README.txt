Highlights User Defined Types/Functions and Constants
for the C Programming language

To use run make/make install

There are 3 new highlights you may want to put in
your colors file:

hi userdefType  ctermfg=3
hi userdefFunction ctermfg=66
hi userdefConstant ctermfg=102

This will add the function DetectTypes() to vim, so
you may also want to add the line

au BufWrite *.c call DetectTypes() to your vimrc
