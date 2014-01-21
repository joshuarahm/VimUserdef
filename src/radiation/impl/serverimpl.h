#ifndef SERVERIMPL_H_
#define SERVERIMPL_H_

/*
 * Author: jrahm
 * created: 2014/01/21
 * serverimpl.h: This contains the interface for the commands when Vim is
 * operating in server mode
 */

#include "radiation.h"

extern char* g_servername ;
extern int   (*_vim_server_post_error)( radiator_t* ths, const char* error ) ;
extern int   (*_vim_server_error_destr)( radiator_t* ths, char** error ) ;
extern char* (*_vim_server_query)( radiator_t* ths, const char* variable, const char* def ) ;
extern void  (*_vim_server_finish)( radiator_t* ths, int success ) ;
extern void  (*_vim_server_queue_command)( radiator_t* ths, command_node_t* node ) ;
extern int   (*_vim_server_read_message)( radiator_t* ths, uint64_t timeout, message_t** ret ) ;

#endif /* SERVERIMPL_H_ */
