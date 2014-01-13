#include "radiation.h"
#include "blocking_queue.h"

#include <string.h>
#include <stdlib.h>

/* The radiator for the test program */
radiator_t test_radiator ;

/* The main function that is used
 * to radiate a file */
static int test_radiate_file(
    radiator_t* ths,
    const char* filename,
    const char* filetype,
    const char* env ) {

    (void) filename;
    (void) filetype;
    (void) env;

    queue_value_t* value ;

    value = malloc( sizeof( queue_value_t ) ) ;

    /* Add keyword test te highlight group 'RadiationType' */
    value->keyword = strdup("test") ;
    value->hgroup  = "RadiationType" ;
    
    /* add this to the blocking queue */
    lprintf("Adding %p to the blocking queue\n", value ) ;
    blocking_queue_add( ths->data_queue, value ) ;

    /* we have to finish off with a NULL pointer to
     * signal EOF */
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
