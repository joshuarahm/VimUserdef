
#include "radiation.h"
#include "blocking_queue.h"
#include "envparse.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>

radiator_t c_radiator ;

static char* readall( FILE* file ) {
	char* buf = malloc( 1024 ) ;
	size_t buflen = 1024 ;
	size_t totalread = 0 ;
	size_t bytesread = 0 ;

	size_t next_read = buflen - totalread ;
	while ( (bytesread = fread( buf + totalread, next_read, 1, file )) == next_read ) {
		buf = realloc( buf, buflen *= 2 ) ;
		totalread += bytesread;
		next_read = buflen - totalread ;
	}

	buf = realloc( buf, totalread ) ;
	return buf ;
}

static int c_radiate_file(
	radiator_t* ths,
	const char* filename,
	const char* filetype,
	const char* env ) {
	(void) env ;
	(void) filetype ;

	char* output ;
	char* c_compiler = radiator_query_variable_default( ths, "radiation_c_compiler", "gcc" ) ;
	char* c_flags    = radiator_query_variable_default( ths, "radiation_c_cflags", "" ) ;
	FILE* proc ;

	char buffer[ 4096 ] ;
	/* create the command and run the popen */
	snprintf( buffer, 4096, "%s %s -E %s", c_compiler, c_flags, filename ) ;
	proc = popen( buffer, "r" ) ;

	free( c_compiler ) ;
	free( c_flags ) ;

	return RADIATION_OK ;
}

int c_init( void* arg ) {
	( void ) arg ;

	init_radiator( &c_radiator, c_radiate_file ) ;

	lprintf("Initialized c module.\n") ;
	return RADIATION_OK ;
}
