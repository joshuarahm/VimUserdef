" Function that uses a Ruby script to
" parse all C and H files in a directory
" and sub directories to detect what new
" types there are
function! DetectTypes( ... )
    if &filetype != 'c'
        return
    endif

    if a:0 > 0
        " Overridden default value
        let l:file=a:1
    else
        " The default value is the current
        " file
        let l:dir=bufname("%")
    endif

    " Get the correct C compiler
    if exists('g:c_compiler')
        " The C comiler has been defined
        " by the user
        let l:cc = g:c_compiler
    else
        " Otherwise, the C compiler is gcc
        let l:cc = 'gcc'
    endif

    " set the cflags varaible
    if exists('g:cflags')
        " there is a defined global variable
        let l:cflags=g:cflags
    elseif filereadable('.userdef_c_config')
        " There is a file I can raed
        let l:cflags=join( readfile('.userdef_c_config') )
    else
        " There is no extra cflags
        let l:cflags=''
    endif

    let l:cmd="CC=\"".l:cc."\" CFLAGS=\"".l:cflags."\" ~/.bin/vimuserdef ".l:dir

    let l:output=system(l:cmd)

    " Split the output by newline
    " characters
    let l:lines = split( l:output, "\n" )
    let l:cmd = ""

    for i in l:lines
        exe i
    endfor

endfunction
