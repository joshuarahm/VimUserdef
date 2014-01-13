#ifndef RADIATION_HPP_
#define RADIATION_HPP_

/*
 * Author: jrahm
 * created: 2014/01/09
 * radiation.hpp: <description>
 *
 * Functions which are compliant to vim's `libcall`
 * function are prefaced with `vim`
 */

#include "blocking_queue.h"
#include <stdio.h>

#define RADIATION_OK 0
#define RADIATION_ENORADIATOR 1
#define RADIATION_ENOINIT 2
#define RADIATION_ETIMEOUT 3
#define RADIATION_ENOARGS 4

extern FILE* logfile ;

#ifdef DEBUG
#define lprintf( fmt, ... ) \
	if( logfile ) { fprintf( logfile, fmt, ##__VA_ARGS__ ) ; fflush( logfile ) ; }
#else
#define lprintf( fmt, ... )
#endif

/* The types of elements to be
 * appended to the queue 
 *
 * This struct should be malloc'd
 * because it will be deleted by the
 * consumer */
typedef struct {
	/* the keyword to highlight */

	/* This keyword should be
	 * malloc'd on the stack to be
	 * freed by the consumer thread
	 */
	char* keyword ;

	/* the highlight group to use */

	/* This char* should NOT be malloc'd
	 * as it will not be free'd by the
	 * consumer thread */
	const char* hgroup ;

} queue_value_t ;

/* The general struct RADIATOR is the
 * structure that holds all the information
 * for radiating a file */
typedef struct RADIATOR {
	/* a function that is used to radiate the
	 * file provided */
	int(*radiate_file)(
		/* This highlighter */
		struct RADIATOR* ths,

		/* the filename to radiate */
		const char* file,

		/* filetype, as specified by
		 * Vim */
		const char* filetype,

		/* The environment to use */
		const char* env ) ;

	/* The queue that the radiate file function will eventually
	 * start posting queue_value_t structs to */
	blocking_queue_t* data_queue ;
} radiator_t ;

#ifdef __cplusplus
extern "C" {
#endif
	/* initialsizes the library and adds all the modules */
	int radiation_init( void ) ;

	/* Invokes this program to start parsing the filename with
	 * the environment given to it 
	 *
	 * Returns: an error message if the task could not be completed,
	 * null otherwise.
	 */
	int radiate( const char* filename, const char* filetype, const char* env ) ;
	
	/*
	 * Adds a radiator to the registry based no the filetype
	 */
	int radiation_register( const char* filetype, radiator_t* radiator) ;
	
	/*
	 * returns the next value
	 */
	const char* radiation_next( void ) ;
	
	/*
	 * Return an description of the
	 * error that occured
	 */
	const char* radiation_get_error_message( void ) ;
	
	/* returns the error code */
	int radiation_get_error_code( void ) ;
#ifdef __cplusplus
}
#endif

#endif /* RADIATION_HPP_ */
