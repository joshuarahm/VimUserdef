
#include "radiation.h"
#include "blocking_queue.h"
#include "util/strbuf.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>
#include "c_blacklist.h"
#include "util/subprocess.h"

radiator_t c_radiator ;

/* the following are some useful regexes
 * to determine certian features */
#define IDENTIFIER    "(?:[a-zA-Z]|_)(?:\\w|_)*"
#define BODY(group)   "(?:\\{(?:[^{}]|(?"group"))*\\})"

#define INT_CHAR      "(?:(?:signed|unsigned|long|short)\\s+)*(?:signed|unsigned|long|short|int|char)"
#define DOUBLE_FLOAT  "(?:(?:long\\s+)?double|float)"
#define PRIMATIVE     "(?:" DOUBLE_FLOAT "|" INT_CHAR "|void)"
#define COMPLEX       "(?:(struct(?:\\s+("IDENTIFIER"))?|union(?:\\s+("IDENTIFIER"))?|enum(?:\\s+("IDENTIFIER"))?)(?:\\s*("BODY("5")"))?)"
#define NAKED         "(?:" COMPLEX "|" PRIMATIVE "|"IDENTIFIER")"
#define POINTER       "(?:(?:" NAKED ")(?:\\s*\\*\\s*)*)"
#define FULL_TYPE     "(?:(?:const\\s+|volatile\\s+|static\\s+|extern\\s+)*" POINTER ")"
#define TYPEDEF       "(?:typedef\\s+" FULL_TYPE "(?:\\s*(?:(" IDENTIFIER ")|\\(\\s*\\*\\s*("IDENTIFIER")\\))))"
#define FUNCTION      "(?:static\\s+)?" FULL_TYPE "((?<=\\W)\\s*|\\s+)(" IDENTIFIER ")\\s*\\((?!\\s*\\*)[^{;]*?"

typedef long unsigned test_t;
/* get the length of a static array */
#define LENGTH(x) (sizeof(x) / sizeof(x[0]))

#include "c_reserved_header.h"

/* the types of highlighting we can
 * have for each group mapping */
const char* c_typedef_groups[] = {
      NULL
    , NULL
    , "RadiationCStruct"
    , "RadiationCUnion"
    , "RadiationCEnum"
    , NULL 
    , "RadiationCTypedef"
    , "RadiationCTypedef"
};

const char* c_complex_groups[] = {
      NULL
    , NULL
    , "RadiationCStruct"
    , "RadiationCUnion"
    , "RadiationCEnum"
};

const char* c_function_groups[] = {
      NULL
    , NULL
    , NULL
    , NULL
    , NULL
    , NULL
    , NULL
    , "RadiationCFunction"
};

/* This is a rough mapping of (regex,group) to
 * highlight type */
const char** c_highlights[] = {
      c_typedef_groups,
      c_complex_groups,
      c_function_groups,
} ;


const char* c_regexes[] = {
      /* typedef regex */
      TYPEDEF,
      COMPLEX,
      /* function regex */
      FUNCTION,
};


const pcre* c_pcre_arr[ LENGTH( c_regexes ) ] ;
const pcre_extra* c_pcre_extra_arr[ LENGTH( c_regexes ) ] ;

static void c_match_callback( const char* str, size_t len, int regex, int group ) {

    lprintf( "Callback match %.*s %d %d\n", (int)len, str, regex, group ) ;

    if( regex < 0 || regex > (int)LENGTH(c_highlights) ) 
        return ;
    
    if( len == 0 ) {
        return ;
    }

    const char* highlight = c_highlights[regex][group] ;

    if( highlight ) {
        /* Only add the highlight if the 
         * keyword exists */
        char* keyword = strndup( str, len ) ;
    
        if( ! blacklist_contains( keyword ) ) {
            /* queue the new syndef */
            lprintf("Queue: %s %s\n", highlight, keyword ) ;
            radiator_queue_command( &c_radiator,
                new_syndef_command_destr( &keyword, highlight ) ) ;
        } else {
            free( keyword ) ;
        }
    }
}

static void run_stream( radiator_t* ths, FILE* file ) {
    (void) ths ;

	strbuf_t* buffer = new_strbuf( 4096 ) ;
	ovector_t* vector = new_ovector( 10 ) ;

    strbuf_stream_regex8(
        buffer, vector, file,
        (const pcre**)c_pcre_arr,
        (const pcre_extra**)c_pcre_extra_arr, LENGTH(c_regexes),
        0, c_match_callback ) ;

    radiator_queue_command( &c_radiator, NULL ) ;

	strbuffer_delete( buffer ) ;
	free( vector ) ;
}

void c_error_callback( radiator_t* radiator, const char* error ) {
    lprintf("Recieved error message: %s\n", error) ;
    /* Something came out of stderr. Give the error to Vim */
    radiator_post_error_message( radiator, error ) ;
}

static int c_radiate_file(
	radiator_t* ths,
	const char* filename,
	const char* filetype,
	const char* env ) {

	(void) env ;
	(void) filetype ;

    /* Get variables needed to compile */
	char* c_compiler = radiator_query_variable_default( ths, "radiation_c_compiler", "gcc" ) ;
	char* c_flags    = radiator_query_variable_default( ths, "radiation_c_cflags", "" ) ;
    
	FILE* proc ;

	char buffer[ 4096 ] ;
	/* create the command and run the popen */
	snprintf( buffer, 4096, "%s %s -E %s", c_compiler, c_flags, filename ) ;

	proc = run_process( buffer, (error_callback_t)c_error_callback, &c_radiator ) ;

    if( ! proc ) {
        reprintf(&c_radiator, "Error running process: %s\n", strerror( errno ) ) ;
        radiator_queue_command( &c_radiator, NULL ) ;
    } else {
	    run_stream( ths, proc ) ;
        fclose( proc ) ;
    }

	free( c_compiler ) ;
	free( c_flags ) ;

	return RADIATION_OK ;
}

int c_init( void* arg ) {
	( void ) arg ;

	const char* error ;
	int   error_off ;
    size_t   i ;

    /* Initialize the C radiator struct. This is
     * our gateway to Vim */
	init_radiator( &c_radiator, c_radiate_file ) ;

    /* initialize a blacklist of reserved words so the radiator
     * does not add on to then */
    blacklist_init( c_reserved_words ) ;

    if( LENGTH(c_highlights) < LENGTH(c_regexes) ) {
        /* Check to make sure we do not have more regexes
         * than we do highlights */
        reprintf( &c_radiator,
            "The number of c regexes is greater than "
            "the number of highligts. This will cause "
            "a segmentation fault. Abort.") ;

        return -1 ;
    }

    for( i = 0 ; i < LENGTH(c_regexes); ++ i ) {
        /* Iterate through all the regular expressions and
         * compile/analyze them  */
	    c_pcre_arr[i] = pcre_compile(c_regexes[i], PCRE_DOTALL | PCRE_MULTILINE , &error, &error_off, NULL) ;;

	    if( ! c_pcre_arr[i] ) {
            reprintf(&c_radiator, "There was an error compiling regex '%s' offset %d\n", error, error_off) ;
		    return -1;
	    }
        
	    c_pcre_extra_arr[i] = pcre_study(c_pcre_arr[i], 0, &error) ;
	    if( ! c_pcre_extra_arr[i] ) {
		    reprintf( &c_radiator, "Error with pcre_study: %s\n", error) ;
		    return -1;
	    }
    }

	lprintf("Initialized c module.\n") ;
	return RADIATION_OK ;
}

