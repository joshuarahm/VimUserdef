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

#define SPAWN_QUIET 1

typedef void(*error_callback_t)( void* arg, const char* error ) ;

FILE* run_process ( const char* command, error_callback_t callback, void* arg  ) ;

/* Runs a process and returns a single pipe for both stderr and stdout */
FILE* spawn_oevp ( const char* file, char *const argv[]  ) ;

/* runs a command, waits for it to finish and finally
 * returns the exit code. Do not do this for interactive
 * commands! */
int spawn_waitvp( const char* file, char *const argv[], int options ) ;

#endif /* SUBPROCESS_H_ */
