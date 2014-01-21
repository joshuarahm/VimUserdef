let s:script_dir = expand('<sfile>:p:h')

" The following function bootstrap into python almost
" immediately, which bootstrap into C code almost immediately
" Function is called when radiation starts up.
" This initializes the shared library
function! radiation#Radiation_Init()
	let l:radiation_errormesg = ""
	let s:radiation_python_dir = s:script_dir . '/script/'
	" Call the python file first of all to load the function definitions
	exec 'pyfile ' . s:radiation_python_dir . 'radiation.py'
	
	" Initialize the library
	python radiation_init()
	
	if exists('l:radiation_errormesg') && g:radiation_noisy == 1
		echoerr l:radiation_errormesg
	endif
	
	let g:radiation_initialized=1
endfunc

	
" Invokes the library to parse the current file and
" pull out other highlighting rules
function! radiation#Radiate()
	let l:radiation_errormesg = ""
	" Setup the command to run
	exec printf('python radiate( "%s", "%s", "%s" )', expand("%"), &filetype, "" )

    " This is where the errors are supposed to be
	" stored. This can be quieted with g:radiation_silent
	if l:radiation_errormesg!="" && g:radiation_noisy == 1
		echoerr l:radiation_errormesg
	endif

endfunc

function! radiation#Digest()
	let l:radiation_errormesg = ""
	" Have python digest everything
	exec 'python digest_all()'

    " This is where the errors are supposed to be
	" stored. This can be quieted with g:radiation_silent
	if l:radiation_errormesg!="" && g:radiation_noisy == 1
		echoerr l:radiation_errormesg
	endif
endfunc
