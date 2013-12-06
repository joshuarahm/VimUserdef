" Function that uses a Ruby script to
" parse all C and H files in a directory
" and sub directories to detect what new
" types there are

function! SetupCEnvironment()
    let l:cc="/usr/bin/gcc"
    let l:cflags=""

    if exists('g:userdef_c_compiler')
        let l:cc=g:userdef_c_compiler
    endif

    if exists('g:userdef_cflags')
        " there is a defined global variable
        let l:cflags.=g:userdef_cflags
    endif

    if filereadable('.userdef_c_config')
        " There is a file I can raed
        let l:cflags.=join( readfile('.userdef_c_config') )
    endif

    return printf( "CC='%s' CFLAGS='%s'", l:cc, l:cflags )
endfunc

function! SetupJavaEnvironment()
    let l:javap = '/usr/bin/javap'
    let l:javap_flags = ''

    if exists('g:userdef_javap')
        let l:javap = g:userdef_javap
    endif

    if exists('g:userdef_javap_flags')
        let l:javap_flags .= g:userdef_javap_flags
    endif

    if filereadable('.userdef_java_config')
        let l:javap_flags .= join( readfile('.userdef_java_config') )
    endif

    return printf( "JAVAP='%s' JAVAP_FLAGS='%s'", l:javap, l:javap_flags )
endfunc

function! SetupEnvironment()
    " Get the file type of the buffer
    let l:filename = expand('%:t')

    if l:filename =~ '\.c$' || l:filename =~ '\.h$'
        return SetupCEnvironment()
    elseif l:filename =~ '\.java$'
        return SetupJavaEnvironment()
    endif
endfunc

function! DetectTypes( ... )
    let l:tmpfile = "/tmp/.vimuserdefgen.vim"
    if a:0 > 0
        " Overridden default value
        let l:file=a:1
    else
        " The default value is the current
        " file
        let l:file=bufname("%")
    endif

    let l:env = SetupEnvironment()
    let l:cmd = printf( "%s ~/.bin/vimuserdef %s > %s", l:env, l:file, l:tmpfile )
    " echo "Command: ".l:cmd
    let l:output=system(l:cmd)

    exe 'source '.l:tmpfile
    call delete(l:tmpfile)
endfunction

exe 'command! HiTypes call DetectTypes()'
