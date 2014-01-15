
#include "radiation.h"
#include "blocking_queue.h"
#include "util/strbuf.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>

radiator_t c_radiator ;

/* we will initialize these guys in
 * the init routine so we do not have
 * to keep doing it */
pcre* typedef_re ;
pcre_extra* typedef_re_extra ;

#define TYPEDEF_REGEX "typedef\\s+(.*)?\\s+(\\w+)\\s+;"

static void run_stream( radiator_t* ths, FILE* file ) {
	char* capture ;

	strbuf_t* buffer = new_strbuf( 4000, 100 ) ;
	ovector_t* vector = new_ovector( 5 ) ;

	size_t offset ;
	int rc = 0 ;

	/* while we can read more of the file
	 * into the stream buffer */
	while ( strbuf_read( buffer, file ) ) {
		offset = 0 ;

		/* match all the possible matches
		 * that are currently in the buffer */
		while( rc != PCRE_ERROR_NOMATCH ) {
			/* execute the regular expression on
			 * the buffer */
			rc = strbuf_pcre_exec( buffer, typedef_re, typedef_re_extra,
				offset, vector, PCRE_PARTIAL ) ;

			if( rc == PCRE_ERROR_PARTIAL ) {
				/* If there is a partial match but nothing else, then
				 * we need to try to match on that, so shift the buffer
				 * so the partial match is first */
				strbuf_cut_offset( buffer, file, ovector_first_match( vector ) ) ;
			} else if ( rc > 0 ) {

				/* the second capture is what the typename
				 * is to highlight */
				capture = strbuf_get_capture( buffer, vector, 2 ) ;
				lprintf("Found capture %s\n", capture) ;

				/* Communicate the new command */
				radiator_queue_command( ths,
					new_syndef_command_destr( &capture, "RadiationCTypedef") ) ;

				/* TODO Un hack this */
				/* Set the offset so we can get the next
				 * one */
				offset = vector->ovector[1];
			};
		}
	}

	free( buffer ) ;
	free( vector ) ;
}

static int c_radiate_file(
	radiator_t* ths,
	const char* filename,
	const char* filetype,
	const char* env ) {
	(void) env ;
	(void) filetype ;

	char* c_compiler = radiator_query_variable_default( ths, "radiation_c_compiler", "gcc" ) ;
	char* c_flags    = radiator_query_variable_default( ths, "radiation_c_cflags", "" ) ;
	FILE* proc ;

	char buffer[ 4096 ] ;
	/* create the command and run the popen */
	snprintf( buffer, 4096, "%s %s -E %s", c_compiler, c_flags, filename ) ;
	lprintf("Running command: %s\n", buffer) ;
	proc = popen( buffer, "r" ) ;

	run_stream( ths, proc ) ;

	free( c_compiler ) ;
	free( c_flags ) ;

	return RADIATION_OK ;
}

int c_init( void* arg ) {
	( void ) arg ;

	const char* error ;
	int   error_off ;
	init_radiator( &c_radiator, c_radiate_file ) ;

	typedef_re = pcre_compile(TYPEDEF_REGEX, PCRE_DOTALL, &error, &error_off, NULL) ;

	if( ! typedef_re ) {
		lprintf("There was an error compiling regex '%s' offset %d\n", error, error_off) ;
		return -1;
	}
	
	typedef_re_extra = pcre_study(typedef_re, 0, &error) ;
	if( ! typedef_re_extra ) {
		lprintf("Error with pcre_study: %s\n", error) ;
		return -1;
	}

	lprintf("Initialized c module.\n") ;
	return RADIATION_OK ;
}
