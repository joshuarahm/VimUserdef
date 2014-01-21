#ifndef SEQUENTIALIMPL_H_
#define SEQUENTIALIMPL_H_

/*
 * Author: jrahm
 * created: 2014/01/21
 * sequentialimpl.h:
 *
 * This file is the interface for a Radiation that
 * runs on top of a sequential Vim
 */

#include "radiation.h"

extern int   (*_vim_sequential_post_error)( radiator_t* ths, const char* error ) ;
extern int   (*_vim_sequential_error_destr)( radiator_t* ths, char** error ) ;
extern char* (*_vim_sequential_query)( radiator_t* ths, const char* variable, const char* def ) ;
extern void  (*_vim_sequential_finish)( radiator_t* ths, int success ) ;
extern void  (*_vim_sequential_queue_command)( radiator_t* ths, command_node_t* node ) ;
extern int   (*_vim_sequential_read_message)( radiator_t* ths, uint64_t timeout, message_t** ret ) ;

#endif /* SEQUENTIALIMPL_H_ */
