#include "radiation.h"
#include "blocking_queue.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/* The radiator for the test program */
radiator_t test_radiator ;

/* The main function that is used
 * to radiate a file */
int test_radiate_file(

    /* All the test radiator does is use
     * the unix dictionary file to highlight
     * a bunch of words */
    radiator_t* ths,
    const char* filename,
    const char* filetype,
    const char* env ) {

    (void) filename;
    (void) filetype;
    (void) env;

    size_t len ;
    char* word ;
    int ec ;
    ( void ) ec ;
    FILE * dict ;

    /* to show how the communication to Vim can work,
     * this is how we can specify our own dictionary
     * file in Vim and access it in this module */

    /* query for the variable. The radiator struct
     * is the means by which to communicate. */
    dict = fopen(ths->query(ths, "radiation_test_dictfile", "testdic.txt" ), "r");

    if( ! dict ) {
        lprintf("Unable to open dictionary.\n") ;
        return 1 ;
    }

    /* We can make a completely custom
     * command */
	ths->queue( ths,
        new_raw_command("hi RadiationTestWord ctermfg=45")  ) ;

    while ( 1 ) {
        len = 10 ;
        word = (char*) malloc( len ) ;

        if( getline( &word, &len, dict ) < 0 ) {
            free( word ) ;
            break ;
        }

        /* chop newline */
        word[strlen(word)-1] = '\0' ;

        /* We can create a syndef command */
        ths->queue( ths,
            new_syndef_command_destr( &word, "RadiationTestWord" )
        ) ;
    }

    /* we have to finish off with a NULL pointer to
     * signal EOF */
    ths->finished( ths, RADIATION_OK );

    return 0 ;
}

/* The initialization routine */
int test_init( void* arg ) {
    (void) arg ;

    init_radiator( &test_radiator, test_radiate_file );

    lprintf("Initialized test module.\n") ;
    /* Signal that we were able to initialize
     * the module properly */
    return RADIATION_OK ;
};

