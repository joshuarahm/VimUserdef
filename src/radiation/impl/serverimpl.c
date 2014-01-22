#include "serverimpl.h"
#include "sequentialimpl.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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
    size_t i ;
	char* real = malloc( len ) ;

	len = snprintf( real, len, "\033:call radiation#Error(\"%s\")<CR>", *error) ;
    for( i = 0 ; i < len ; ++ i ) {
        if( isspace(real[i]) ) {
            real[i] = ' ' ;
        }
    }
    
	char* argv[] = {
		"--remote-send",
		real
	} ;
 
	int ret ;
	radiation_server_call(argv, 2, &ret );

	free( real ) ;
	free( *error ) ;

	return ret ;
}

struct SINTF server_interface = {
    .error       = _vim_sequential_post_error_    ,
    .error_destr = _vim_server_error_destr_       ,
    .query       = _vim_server_query_             ,
    .finished    = _vim_server_finish_            ,
    .queue       = _vim_sequential_queue_command_ ,
    .read        = _vim_sequential_read_message_  ,
} ;

