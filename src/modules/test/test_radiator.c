#include "radiation.h"
#include "blocking_queue.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/* The radiator for the test program */
radiator_t test_radiator ;

/* The main function that is used
 * to radiate a file */
static int test_radiate_file(

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

    FILE * dict = fopen ( "/usr/share/dict/cracklib-small", "r" ) ;
    size_t len ;
    char* word ;

    if( ! dict ) {
        lprintf("Unable to open dictionary.\n") ;
        return 1 ;
    }

    /* We can make a completely custom
     * command */
    blocking_queue_add(
        ths->data_queue,

        /* This HAS to be a strdup, if it is not,
         * bad things will happen */
        new_raw_command( "hi RadiationTestWord ctermfg=45" )
    ) ;

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
        blocking_queue_add(
            ths->data_queue,
            new_syndef_command_destr( &word, "RadiationTestWord" )
        ) ;
    }

    // /* we have to finish off with a NULL pointer to
    //  * signal EOF */
    blocking_queue_add( ths->data_queue, NULL ) ;

    return 0 ;
}

/* The initialization routine */
int test_init( void* arg ) {
    (void) arg ;

    lprintf("Initialized test module.\n") ;

    /* create a new blocking queue */
    test_radiator.data_queue   = new_blocking_queue() ;
    test_radiator.radiate_file = test_radiate_file ;

    /* Signal that we were able to initialize
     * the module properly */
    return 0 ;
};
