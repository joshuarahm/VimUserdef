
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

#define IDENTIFIER "(?:\\w|_)(?:\\w|_|\\d)+"
#define TYPEDEF_REGEX "typedef[^;]+\\s(" IDENTIFIER  ")\\s*;"

static void c_match_callback( const char* str, size_t len, int group ) {
    if( group == 1 ) {
        lprintf("got typedef %.*s\n", (int)len, str) ;
    } else {
        lprintf("got group #%d %.*s\n", group, (int)len, str) ;
    }
}

static void run_stream( radiator_t* ths, FILE* file ) {
    (void) ths ;

	strbuf_t* buffer = new_strbuf( 4096 ) ;
	ovector_t* vector = new_ovector( 3 ) ;

    strbuf_stream_regex7(
        buffer, vector, file,
        typedef_re, typedef_re_extra,
        0, c_match_callback ) ;

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

	typedef_re = pcre_compile(TYPEDEF_REGEX, PCRE_DOTALL | PCRE_MULTILINE , &error, &error_off, NULL) ;

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
