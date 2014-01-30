#ifndef RADIATION_HPP_
#define RADIATION_HPP_

/*
 * Author: jrahm
 * created: 2014/01/09
 * radiation.hpp: <description>
 *
 * Functions which are compliant to vim's `libcall`
 * function are prefaced with `vim`
 */

#include <stdio.h>

#include "blocking_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RADIATION_OK 0
#define RADIATION_ENORADIATOR 1
#define RADIATION_ENOINIT 2
#define RADIATION_ETIMEOUT 3
#define RADIATION_ENOARGS 4
#define RADIATION_ECANTRUN 5

extern FILE* logfile ;

#ifdef DEBUG
#define lprintf( fmt, ... ) \
	if( logfile ) { fprintf( logfile, fmt, ##__VA_ARGS__ ) ; fflush( logfile ) ; }
#else
#define lprintf( fmt, ... )
#endif

/* The types of elements to be
 * appended to the queue 
 *
 * This struct should be malloc'd
 * because it will be deleted by the
 * consumer */

enum command_type {
	  COMMAND_SYNDEF
	, COMMAND_RAW
    , COMMAND_QUERY
    , COMMAND_ERROR
    , COMMAND_FATAL
};

enum message_type {
      MESSAGE_STRING_VALUE
    , MESSAGE_ERROR
} ;

typedef struct {
	/* the keyword to highlight */

	enum command_type type ;

	union {
		struct {
			/* This keyword should be
	 		 * malloc'd on the stack to be
	 		 * freed by the consumer thread
	 		 */
			char* keyword ;
		
			/* the highlight group to use */
		
			/* This char* should NOT be malloc'd
	 		 * as it will not be free'd by the
	 		 * consumer thread */
			const char* hgroup ;
		} syndef ;

		struct {
			/* The raw command */
			char* raw ;
		} raw ;

        struct {
            /* The variable to query */
            char* query ;
        } query ;

        struct {
            /* pointer to the error message */
            char* error ;
        } error ;
	} ;

} command_node_t ;

/* This struct is a part of the communicaiton
 * that allows the library to obtain the value
 * of a variable in vim */
typedef struct {
    
    /* The type of response. */
    enum message_type type ; 

    union {
        char* strval ;

        struct {
            /* returned if the value is a
             * string value */
            char* value ;
        } stringval ;

        struct {
            char* message ;
            int   code ;
        } errorval ;
    } ;
} message_t ;

/* Creates a new syntax defining command node.
 * 
 * The keyword will be duplicated using strdup, so
 * it may be allocated wherever. The hgroup parameter
 * will not be deleted and should be a constant
 *
 * The hgroup, or highlight group should be constant, because
 * it will not be deleted by the command node.
 */
command_node_t* new_syndef_command( const char* keyword, const char* hgroup ) ;

/* destructive syndef command. This does not implicitly
 * call strdup on the keyword. Instead, it uses double pointer
 * notation to take ownership of the char* and set the original
 * to NULL
 */
command_node_t* new_syndef_command_destr( char** keyword, const char* hgroup ) ;

/*
 * Creates a new raw command node. This is a command that in
 * arbitrary. This is more powerful than the sydef type, and certianly
 * everything that syndef command can do, raw command can also do, but
 * it does not lend itself to better optimizations by the system, so it
 * is recommended that you use syndef for syntax definitions
 *
 * The raw parameter will be deleted by the command node. So it should be
 * a heap allocated value.
 */
command_node_t* new_raw_command( const char* raw ) ;

/* 
 * destructive raw command. like the previous destructive command,
 * it does no implicitly strdup. Use this only when effeciency matters.
 */
command_node_t* new_raw_command_destr( char** raw ) ;

/* forward declare for typedef */
struct RADIATOR ;

typedef int (*radiate_file_routine_t)(struct RADIATOR*,const char*,const char*,const char*);


#define SINTF_BODY \
    /* post an error message to Vim */                         \
    int   (*error)( struct RADIATOR* ths, const char* error ) ;\
    /*                                                         \
     * Just like the above, except that the string is not strdup'd   \
     * so this function will take control of the error pointer using \
     * double pointer notation.                                      \
     */                                                              \
    int   (*error_destr)( struct RADIATOR* ths, char** error );      \
    /*                                                               \
    * Queries Vim for the value of a variable, waits for             \
    * the result and stores that result in `ret`.                    \
    *                                                                \
    * When passed into this function, if *ret != NULL, it is free'd  \
    * to make chaining operations more consise, so be careful        \
    * when using the values repeatedly and be sure to ALWAYS         \
    * initialize your message_t*'s to NULL                           \
    */                                                               \
    char* (*query)( struct RADIATOR* ths, const char* variable, const char* def ) ; \
    /* signal the end of the radiation */                                           \
    void  (*finished)( struct RADIATOR* ths, int success ) ;                        \
    /* Adds a command to the radiator's data queue to be processed by               \
     * the client side */                                                           \
    void (*queue)( struct RADIATOR* ths, command_node_t* node ) ;                   \
    /* reads a message from the Vim server. This may be */                          \
    int  (*read)( struct RADIATOR* ths, uint64_t timeout, message_t** ret ) ;

struct SINTF {
    SINTF_BODY
} ;

#define EXTENDS_RADIATOR(type) \
    union { \
        radiator_t super ; \
        struct { \
            radiate_file_routine_t routine ;\
	        blocking_queue_t* data_queue ;\
            blocking_queue_t* message_queue ;\
 	        int   (*error)( type* ths, const char* error ) ;\
 	        int   (*error_destr)( type* ths, char** error );\
 	        char* (*query)( type* ths, const char* variable, const char* def ) ;\
 	        void  (*finished)( type* ths, int success ) ;\
 	        void  (*queue)( type* ths, command_node_t* node ) ;\
 	        int   (*read)( type* ths, uint64_t timeout, message_t** ret ) ;\
        } ; \
    };

/* The general struct RADIATOR is the
 * structure that holds all the information
 * for radiating a file */
typedef struct RADIATOR {
	/* a function that is used to radiate the
	 * file provided */
    radiate_file_routine_t routine ;

	/* The queue that the radiate file function will eventually
	 * start posting command_node_t structs to */
	blocking_queue_t* data_queue ;
    blocking_queue_t* message_queue ;


    union {
        struct SINTF system_interface;

        struct {
             int   (*error)( struct RADIATOR* ths, const char* error ) ;
             int   (*error_destr)( struct RADIATOR* ths, char** error );
             char* (*query)( struct RADIATOR* ths, const char* variable, const char* def ) ;
             void  (*finished)( struct RADIATOR* ths, int success ) ;
             void  (*queue)( struct RADIATOR* ths, command_node_t* node ) ;
             int   (*read)( struct RADIATOR* ths, uint64_t timeout, message_t** ret ) ;
        } ;
    } ;
} radiator_t ;

#undef SINTF

/* initializes a radiator. */
int init_radiator( radiator_t* rad, radiate_file_routine_t routine ) ;

/*
 * A printf-like frontend to radiator_post_error_message
 */
void reprintf( radiator_t* rad, const char* fmt, ... ) ;

/*
 * If Vim is running in a server environment, it is possible to
 * invoke it through the remote interface and tell it digest the
 * priority queue.
 */
int radiation_call_digest( ) ;

/* 
 * Makes a call to the server and returns
 * the stdout output and places the exit code
 * in `ret`
 */
char* radiation_server_call( char** argv, size_t len, int* ret ) ;

/*
 * frees the memory for a message
 */
void message_delete( message_t* mesg ) ;

/* initialsizes the library and adds all the modules */
int radiation_init( void ) ;

/* Invokes this program to start parsing the filename with
 * the environment given to it 
 *
 * Returns: an error message if the task could not be completed,
 * null otherwise.
 */
int radiate( const char* filename, const char* filetype, const char* env ) ;

/*
 * Adds a radiator to the registry based no the filetype
 */
int radiation_register( const char* filetype, radiator_t* radiator) ;

/*
 * returns the next value
 */
const char* radiation_next( void ) ;

/*
 * Return an description of the
 * error that occured
 */
const char* radiation_get_error_message( void ) ;

/* returns the error code */
int radiation_get_error_code( void ) ;

/* python-end communication to the library */
int radiation_put_string_message( const char* message ) ;

/* Puts an error message and error code to communicate
 * excepive behavior to the lower layers */
int radiation_put_error_message( const char* message, int errorcode ) ;

/* Tells radiation what the servername of the current running Vim
 * session is */
int radiation_set_servername( const char* servername ) ;

#ifdef __cplusplus
}
#endif

#endif /* RADIATION_HPP_ */
