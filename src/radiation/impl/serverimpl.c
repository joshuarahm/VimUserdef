#include "serverimpl.h"
#include "sequentialimpl.h"

#include <string.h>
#include <stdlib.h>

void  _vim_sequential_finish_(radiator_t* ths, int success ) ;
int   _vim_sequential_post_error_(radiator_t*, const char*);
int   _vim_sequential_read_message_( radiator_t* ths, uint64_t timeout, message_t** ret ) ;
void  _vim_sequential_queue_command_( radiator_t* ths, command_node_t* node ) ;

void  _vim_server_finish_(radiator_t* ths, int success ) {
	_vim_sequential_finish_( ths, success ) ;
	radiation_call_digest() ;
}

char* _vim_server_query_(radiator_t* ths,const char* val,const char* def) {
	(void)ths ;
	int ret ;
	char* output ;

	char* argv[] = {
		"--remote-expr",
		strdup(val)
	};

	output = radiation_server_call(argv, 2, &ret ) ;

	if( output == NULL )
		return strdup(def) ;

	free( argv[1] ) ;
	return output ;
}


int  _vim_server_error_destr_(radiator_t* ths, char** error) {
	(void) ths ;
	if( ! error || ! *error ) return -1 ;

	size_t len = strlen( *error ) + 128;
	char* real = malloc( len ) ;

	snprintf( real, len, "call radiation#error(\"%s\")", *error) ;
	char* argv[] = {
		"--remote-send",
		real
	} ;

	int ret ;
	radiation_server_call(argv, 2, &ret );

	free( real ) ;
	free( error ) ;

	return ret ;
}

int   (*_vim_server_error_destr)(radiator_t*, char**) = _vim_server_error_destr_;
char* (*_vim_server_query)(radiator_t*,const char*,const char*) = _vim_server_query_;
void  (*_vim_server_finish)(radiator_t*,int) = _vim_server_finish_;


void  (*_vim_server_queue_command)(radiator_t*,command_node_t* ) = _vim_sequential_queue_command_;
int   (*_vim_server_read_message)(radiator_t*,uint64_t,message_t**) = _vim_sequential_read_message_;
int   (*_vim_server_post_error)(radiator_t*,const char*) = _vim_sequential_post_error_;

