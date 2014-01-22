
#include "radiation.h"
#include "blocking_queue.h"
#include "util/strbuf.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>
#include "util/subprocess.h"

radiator_t cpp_radiator ;

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

/* the types of highlighting we can
 * have for each group mapping */
const char* cpp_typedef_groups[] = {
      NULL
    , NULL
    , "RadiationCppStruct"
    , "RadiationCppUnion"
    , "RadiationCppEnum"
    , NULL 
    , "RadiationCppTypedef"
    , "RadiationCppTypedef"
};

const char* cpp_complex_groups[] = {
      NULL
    , NULL
    , "RadiationCppStruct"
    , "RadiationCppUnion"
    , "RadiationCppEnum"
};

const char* cpp_function_groups[] = {
      NULL
    , NULL
    , NULL
    , NULL
    , NULL
    , NULL
    , NULL
    , "RadiationCppFunction"
};

/* This is a rough mapping of (regex,group) to
 * highlight type */
const char** cpp_highlights[] = {
      cpp_typedef_groups,
      cpp_complex_groups,
      cpp_function_groups,
} ;


const char* cpp_regexes[] = {
      /* typedef regex */
      TYPEDEF,
      COMPLEX,
      /* function regex */
      FUNCTION,
};


const pcre* cpp_pcre_arr[ LENGTH( cpp_regexes ) ] ;
const pcre_extra* cpp_pcre_extra_arr[ LENGTH( cpp_regexes ) ] ;

static void cpp_match_callback( const char* str, size_t len, int regex, int group ) {

    lprintf( "Callback match %.*s %d %d\n", (int)len, str, regex, group ) ;

    if( regex < 0 || regex > (int)LENGTH(cpp_highlights) ) 
        return ;
    
    if( len == 0 ) {
        return ;
    }

    const char* highlight = cpp_highlights[regex][group] ;

    if( highlight ) {
        /* Only add the highlight if the 
         * keyword exists */
        char* keyword = strndup( str, len ) ;

        /* queue the new syndef */
        lprintf("Queue: %s %s\n", highlight, keyword ) ;
        cpp_radiator.queue( &cpp_radiator, 
            new_syndef_command_destr( &keyword, highlight ) ) ;
    }
}

static void run_stream( radiator_t* ths, FILE* file ) {
    (void) ths ;

	strbuf_t* buffer = new_strbuf( 4096 ) ;
	ovector_t* vector = new_ovector( 10 ) ;

    strbuf_stream_regex8(
        buffer, vector, file,
        (const pcre**)cpp_pcre_arr,
        (const pcre_extra**)cpp_pcre_extra_arr, LENGTH(cpp_regexes),
        0, cpp_match_callback ) ;

    cpp_radiator.finished( &cpp_radiator, RADIATION_OK ) ;

	strbuffer_delete( buffer ) ;
	free( vector ) ;
}

void cpp_error_callback( radiator_t* radiator, const char* error ) {
    lprintf("Recieved error message: %s\n", error) ;
    /* Something came out of stderr. Give the error to Vim */
    radiator->error( radiator, error ) ;
}

static int cpp_radiate_file(
	radiator_t* ths,
	const char* filename,
	const char* filetype,
	const char* env ) {

	(void) env ;
	(void) filetype ;

    /* Get variables needed to compile */
	char* cpp_compiler = ths->query( ths, "radiation_cpp_compiler", "g++" ) ;
	char* cpp_flags    = ths->query( ths, "radiation_cpp_cflags", "" ) ;

	char buffer[ 4096 ] ;
	/* create the command and run the popen */
	snprintf( buffer, 4096, "%s %s -E %s", cpp_compiler, cpp_flags, filename ) ;

	FILE* proc ;
	proc = run_process( buffer, (error_callback_t)cpp_error_callback, &cpp_radiator ) ;

    if( ! proc ) {
        reprintf(&cpp_radiator, "Error running process: %s\n", strerror( errno ) ) ;
        cpp_radiator.finished( &cpp_radiator, RADIATION_ECANTRUN ) ;
    } else {
	    run_stream( ths, proc ) ;
        fclose( proc ) ;
    }

	free( cpp_compiler ) ;
	free( cpp_flags ) ;

	return RADIATION_OK ;
}

int cpp_init( void* arg ) {
	( void ) arg ;

	const char* error ;
	int   error_off ;
    size_t   i ;

    /* Initialize the C radiator struct. This is
     * our gateway to Vim */
	init_radiator( &cpp_radiator, cpp_radiate_file ) ;

    if( LENGTH(cpp_highlights) < LENGTH(cpp_regexes) ) {
        /* Check to make sure we do not have more regexes
         * than we do highlights */
        reprintf( &cpp_radiator,
            "The number of c regexes is greater than "
            "the number of highligts. This will cause "
            "a segmentation fault. Abort.") ;

        return -1 ;
    }

    for( i = 0 ; i < LENGTH(cpp_regexes); ++ i ) {
        /* Iterate through all the regular expressions and
         * compile/analyze them  */
	    cpp_pcre_arr[i] = pcre_compile(cpp_regexes[i], PCRE_DOTALL | PCRE_MULTILINE , &error, &error_off, NULL) ;;

	    if( ! cpp_pcre_arr[i] ) {
            reprintf(&cpp_radiator, "There was an error compiling regex '%s' offset %d\n", error, error_off) ;
		    return -1;
	    }
        
	    cpp_pcre_extra_arr[i] = pcre_study(cpp_pcre_arr[i], 0, &error) ;
	    if( ! cpp_pcre_extra_arr[i] ) {
		    reprintf( &cpp_radiator, "Error with pcre_study: %s\n", error) ;
		    return -1;
	    }
    }

	lprintf("Initialized C++ module.\n") ;
	return RADIATION_OK ;
}

