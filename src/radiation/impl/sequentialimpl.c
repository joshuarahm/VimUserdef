#include "sequentialimpl.h"
#include <string.h>
#include <stdlib.h>

int   _vim_sequential_post_error_( radiator_t* ths, const char* error ) {
    char* tmp = strdup( error ) ;
	return ths->error_destr( ths, &tmp ) ;
}

int   _vim_sequential_error_destr_( radiator_t* ths, char** error ) {
    command_node_t* command_node = calloc( sizeof(command_node_t), 1 ) ;

    command_node->type = COMMAND_ERROR ;
    command_node->error.error = *error ; 
    *error = NULL ;

	ths->queue( ths, command_node ) ;

    return 0 ;
}

int _p_vim_sequential_radiator_query_variable_( radiator_t* rad, const char* var, message_t** ret ) {
    command_node_t* command = malloc( sizeof( command_node_t ) ) ;
    command->type = COMMAND_QUERY ;
    command->query.query = strdup(var) ;
    /* put the query command on the queue
     * for the python to get */
    rad->queue( rad, command ) ;
    /* delete the old message */
    message_delete( * ret ) ;
    /* read the message with a 10 ms timeout */
    return rad->read( rad, 10, ret ) ;
}

char* _vim_sequential_query_( radiator_t* ths, const char* variable, const char* def ) {
	message_t* ret = NULL ;
	char* mesg ;

	if( _p_vim_sequential_radiator_query_variable_( ths, variable, &ret ) ) {
		message_delete( ret ) ;
		mesg = def == NULL ? NULL : strdup( def ) ;
	} else if ( ret->type == MESSAGE_STRING_VALUE )  {
		mesg = ret->strval ;
		ret->strval = NULL ;
	} else {
		mesg = def == NULL ? NULL : strdup( def ) ;
	}

	message_delete( ret ) ;
	return mesg ;
}

void  _vim_sequential_finish_( radiator_t* ths, int success ) {
	(void) success ;
	ths->queue( ths, NULL ) ;
}

void  _vim_sequential_queue_command_( radiator_t* ths, command_node_t* node ) {
	blocking_queue_add( ths->data_queue, node ) ;
}

int   _vim_sequential_read_message_( radiator_t* ths, uint64_t timeout, message_t** ret ) {
	return blocking_queue_take( ths->message_queue, (void**)ret, timeout ) ;
}

struct SINTF sequential_interface = {
    .error       = _vim_sequential_post_error_    ,
    .error_destr = _vim_sequential_error_destr_   ,
    .query       = _vim_sequential_query_         ,
    .finished    = _vim_sequential_finish_        ,
    .queue       = _vim_sequential_queue_command_ ,
    .read        = _vim_sequential_read_message_  ,
} ;

