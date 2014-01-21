#ifndef SUBPROCESS_H_
#define SUBPROCESS_H_

/*
 * Author: jrahm
 * created: 2014/01/20
 * subprocess.h: <description>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef void(*error_callback_t)( void* arg, const char* error ) ;

FILE* run_process ( const char* command, error_callback_t callback, void* arg  ) ;

#endif /* SUBPROCESS_H_ */
