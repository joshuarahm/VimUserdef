from vim import *
from ctypes import * 
import sys

RADIATION_OK          = 0
RADIATION_ENORADIATOR = 1
RADIATION_ENOINIT     = 2
RADIATION_ETIMEOUT    = 3
RADIATION_ENOARGS     = 4

radiation_lib = None

def set_vim_error_mesg( errmesg ):
	vim.command( "let l:radiation_errormesg='%s'" % errmesg ) 

def init():
	global radiation_lib

	# load the library to be used
	radiation_lib = cdll.LoadLibrary("libvimradiation.so.1")

	radiation_lib.radiation_init.restype = c_int
	radiation_lib.radiate.restype        = c_int
	radiation_lib.radiation_next.restype = c_char_p
	radiation_lib.radiation_get_error_message.restype = c_char_p
	# radiation_lib.radiation_get_error_code.restype    = c_int

	radiation_lib.radiate.argtypes = [c_char_p, c_char_p, c_char_p]

	err = radiation_lib.radiation_init() ;

	if err > 0:
		errmesg = radiation_lib.radiation_get_error_message()
		set_vim_error_mesg( errmesg ) ;

	# radiation lib successfully initialized

def radiate( filename, filetype, env ):

	# make the library call to radiate. This will
	# set us up to iterate across all the commands
	# that the radiator will spit out
	res = radiation_lib.radiate( filename, filetype, env )

	if res > 0:
		
		# the radiation call failed, probably because there
		# is no radiator for that type
		errmesg = radiation_lib.radiation_get_error_message()
		set_vim_error_mesg( errmesg ) ;

		return ;

	while True :
		# The radiator is hard at work, it is time
		# for us to pick each command off the queue
		command = radiation_lib.radiation_next()

		if command == c_char_p( 0 ) :
			# if the call returned NULL, then we are done
			break

		else:
			vim.command( command )
