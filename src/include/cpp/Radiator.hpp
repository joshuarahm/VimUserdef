#ifndef RADIATION_CPP_HPP_
#define RADIATION_CPP_HPP_

/*
 * Author: jrahm
 * created: 2014/01/24
 * radiation_cpp.hpp: <description>
 */

#include "radiation.h"

/* 
 * This class is a thin wrapper to the C code's
 * RADIATOR raidator_t structure
 */
class Radiator {
public:
    /*
     * Create a new Radiator from a new radiator
     * structure. This class does not take ownership
     * of the structure, so the memory must still be
     * deleted
     */
    inline Radiator( struct RADIATOR* radiator ) {
        this->radiator = radiator ;
    }

    /*
     * Query Vim for the evaluation of a variable.
     * If the variable does not exist then `def` is
     * returned instead.
     */
    inline char* query( const char* var, const char* def = NULL ) {
        return this->radiator->query( this->radiator, var, def ) ;
    }

    /*
     * Sinals that the radiator is finished. This will put
     * an eof signal on the queue and alert the server
     * if the server exists.
     */
    inline void finished( int success = RADIATION_OK )  {
        this->radiator->finished( this->radiator, success ) ;
    }

    /*
     * Queue a command for processing in Vim.
     * This will either be processed while the radiator
     * is running or after the radiator has signaled
     * eof.
     */
    inline void queue( command_node_t* command ) {
        this->radiator->queue( this->radiator, command ) ;
    }

    /*
     * Reads a message from the input queue. This will have
     * information about the state of Vim.
     */
    inline int read( uint64_t timeout, message_t** ret ) {
        return this->radiator->read( this->radiator, timeout, ret ) ;
    }

    /*
     * Post an error to the Vim server. This will communicate
     * unexpected exceptions.
     */
    inline void error( const char* error ) {
        this->radiator->error( this->radiator, error ) ;
    }

    /*
     * Does the same as above, but instead of copying the string,
     * it will use the pointer *error while setting the pointer to
     * NULL afterword to signal ownership
     */
    inline int error_destr( char** error ) {
        return this->radiator->error_destr( this->radiator, error ) ;
    }

private:
    /* 
     * The wraped radiator
     */
    struct RADIATOR* radiator ; 

} ;

#endif /* RADIATION_CPP_HPP_ */
