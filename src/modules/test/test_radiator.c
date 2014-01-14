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

    /* The message struct is what is passed from the
     * top layers */
    message_t* mesg = NULL ;

    /* query for the variable. The radiator struct
     * is the means by which to communicate. */
    ec = radiator_query_variable(ths, "radiation_test_dictfile", &mesg ) ;

    if( mesg->type == MESSAGE_ERROR ) {
        /* there was an error trying to open the file,
         * so use the default */
        lprintf("Using default dictionary.\n") ;
        dict = fopen ( "testdic.txt", "r" ) ;
    } else {
        /* we could correctly get the variable so
         * we can use the value */
        lprintf("Using specified dictionary: %s\n", mesg->stringval.value) ;
        dict = fopen ( mesg->stringval.value, "r" ) ;
    }


    if( ! dict ) {
        lprintf("Unable to open dictionary.\n") ;
        return 1 ;
    }

    /* We can make a completely custom
     * command */
    radiator_queue_command( ths,
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
        radiator_queue_command( ths,
            new_syndef_command_destr( &word, "RadiationTestWord" )
        ) ;
    }

    /* we have to finish off with a NULL pointer to
     * signal EOF */
    radiator_queue_command( ths, NULL ) ;

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

