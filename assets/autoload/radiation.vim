let s:script_dir = expand('<sfile>:p:h')

" The following function bootstrap into python almost
" immediately, which bootstrap into C code almost immediately
" Function is called when radiation starts up.
" This initializes the shared library
function! radiation#Radiation_Init()
	let s:radiation_python_dir = s:script_dir . '/script/'
	" Call the python file first of all to load the function definitions
	exec 'pyfile ' . s:radiation_python_dir . 'radiation.py'
	
	" Initialize the library
	python radiation_init()
	
	if exists('l:radiation_errormesg')
		echoerr l:radiation_errormesg
	endif
	
	let g:radiation_initialized=1
endfunc
	
" Invokes the library to parse the current file and
" pull out other highlighting rules
function! radiation#Radiate()
	" Setup the command to run
	exec printf('python radiate( "%s", "%s", "%s" )', expand("%"), &filetype, "" )

    " This is where the errors are supposed to be
	" stored. This can be quieted with g:radiation_silent
	if ! exists("g:radiation_silent") || g:radiation_silent == 0  
	
		if exists('l:radiation_errormesg')
			echoerr l:radiation_errormesg
		endif

	endif

endfunc

