#include "tree.h"
#include "radiation.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* First we need to forward declare all of the modules
 * units so we can reference them in this file */
#define INCLUDE_MODULE( module ) \
    int module##_init( void* arg ) ; \
    extern radiator_t module##_radiator ;
#include "modules/modules.inc"

/* Some constants */
#define RADIATION_ERROR_MESSAGE_LEN 1024
#define RADIATION_MAX_COMMAND_LEN 1024

#define err_printf( fmt, ... ) \
	snprintf( g_error_message, RADIATION_ERROR_MESSAGE_LEN, fmt, ##__VA_ARGS__) ;

#define vim_return( str, el ) \
	const char* _tmp3293__ = (str) ; \
	return _tmp3293__ == NULL ? el : _tmp3293__ ;

/* the file that some debug logging will print to
 * since stdout is consumed by vim */
FILE* logfile = NULL ;

/* value used in the array
 * of modules. Used to easily
 * describe the modules to include */
struct module_inc_node {

    /* The radiator for the module to use.
     * This is the central character of
     * each module. */
    radiator_t* radiator ;

    /* The filetype to use this radiator for */
    const char* filetype ;

    /* The initialization routine this
     * module uses to set itself up */
    int (*init)( void* ) ;

} ;


/* Now we need to redefine the macro to fill
 * the include array with modules */
#undef INCLUDE_MODULE
/* Definition of the macro to enable
 * and include modules in the build */
#define INCLUDE_MODULE( module ) \
	/* { &module_radiator, "module", module_init }, */ \
	{ &module##_radiator, #module, module##_init },

/* The array of modules to include
 * in this compilation */
struct module_inc_node g_enabled_modules[] = {

/* include the file that tells us the
 * modules to include */
#include    "modules/modules.inc"

};

struct treenode {
	RB_ENTRY(treenode) entry;
	const char *key ;
	radiator_t* value ;
} ;

struct thread_args {
	char* filename;
	char* filetype;
	char* env;
} ;

static int treenode_cmp( struct treenode* node1, struct treenode* node2 ) {
	return strcmp( node1->key, node2->key ) ;
}

/* used for iteration */
radiator_t* g_current_radiator = NULL ;
char* g_del_bucket = NULL ;

char g_error_message [ RADIATION_ERROR_MESSAGE_LEN ] ;
int  g_error_code = 0 ;
int  g_is_initialized = 0 ;

command_node_t* new_raw_command( const char* raw ) {
    command_node_t* ret = calloc( sizeof( command_node_t ), 1 ) ;
    ret->raw.raw = strdup(raw) ;
    ret->type = COMMAND_RAW ;
    return ret ;
}

command_node_t* new_raw_command_destr( char** raw ) {
    command_node_t* ret = calloc( sizeof( command_node_t ), 1 ) ;
    ret->raw.raw = *raw ;
    ret->type = COMMAND_RAW ;
    *raw = NULL ;
    return ret ;
}

command_node_t* new_syndef_command( const char* keyword, const char* hgroup ) {
    command_node_t* ret = malloc( sizeof( command_node_t ) ) ;
    ret->syndef.keyword = strdup(keyword) ;
    ret->syndef.hgroup = hgroup ;
    ret->type = COMMAND_SYNDEF ;

    return ret ;
}

command_node_t* new_syndef_command_destr( char** keyword, const char* hgroup ) {
    command_node_t* ret = malloc( sizeof( command_node_t ) ) ;
    ret->syndef.keyword = *keyword ;
    ret->syndef.hgroup = hgroup ;
    ret->type = COMMAND_SYNDEF ;
    *keyword = NULL ;

    return ret ;
}

static void* run_as_thread( void* args_ ) {
	struct thread_args* args = (struct thread_args*)args_ ;

	g_current_radiator->routine(
		g_current_radiator, args->filename,
		args->filetype, args->env ) ;

	free( args->filename ) ;
	free( args->filetype ) ;
	free( args->env ) ;

	free( args ) ;

	pthread_exit( NULL ) ;
}


RB_HEAD( REGISTRY, treenode ) ;
RB_GENERATE( REGISTRY, treenode, entry, treenode_cmp) ;

/* map<char*, radiator_t> */
struct REGISTRY g_registry ;

int radiation_init( ) {
	struct REGISTRY reg = RB_INITIALIZER( &reg ) ;
    int until = sizeof( g_enabled_modules ) / sizeof( struct module_inc_node ) ;  
    int i ;
    int ec ;

    logfile = fopen("radiation_log.txt", "a+") ;
    lprintf("Initializing Radiation\n") ;

	g_registry = reg ;
	g_is_initialized = 1 ;

    for( i = 0 ; i < until ; ++ i ) {
        
        /* initialize the module */
        ec = g_enabled_modules[i].init( NULL ) ;

        if( ec != RADIATION_OK ) {
            fprintf( stderr, "Unable to load module for filetype: %s\n",
                g_enabled_modules[i].filetype ) ;
        } else {
            /* register the file */
            radiation_register( g_enabled_modules[i].filetype,
                g_enabled_modules[i].radiator ) ;

            lprintf("loaded module for %s\n", g_enabled_modules[i].filetype) ;
        }

    }

    lprintf("radiation initialized\n") ;
	return g_error_code = RADIATION_OK;
}

int radiation_register( const char* filetype, radiator_t* rad ) {
	if( ! g_is_initialized ) {
		return g_error_code = RADIATION_ENOINIT ;
	}
	
	struct treenode* node = (struct treenode*)calloc( sizeof( struct treenode ), 1 ) ;
	node->key = filetype ;
	node->value = rad ;
	RB_INSERT( REGISTRY, &g_registry, node ) ;

	return g_error_code = RADIATION_OK ;
}

int radiation_get_error_code() {
	return g_error_code ;
}

int radiate( const char* filename, const char* filetype, const char* env ) {
    lprintf( "Radiating new file %s, type %s, env %s\n", filename, filetype, env ) ;

	if( ! g_is_initialized ) {
		err_printf( "Library not initialized. Call vim_init first!" ) ;
		return RADIATION_ENOINIT ;
	}

	struct treenode tmp ;
	struct treenode* node ;
	tmp.key = filetype ;

	node = RB_FIND(REGISTRY, &g_registry, &tmp) ;
	pthread_t thread ;

	if( node ) {
		/* Create the thread that is supposed to
		 * do the highlighting and pass it the arguments
		 * needed */
		g_current_radiator = node->value ;
		struct thread_args* args = (struct thread_args*)malloc( sizeof( struct thread_args ) );
		args->filename = strdup(filename) ;
		args->filetype = strdup(filetype) ;
		args->env = strdup(env) ;

		pthread_create( &thread, NULL, run_as_thread, args ) ;
	} else {
		err_printf("No radiator for filetype: %s", filetype) ;
		return g_error_code = RADIATION_ENORADIATOR ;
	}

	return g_error_code = RADIATION_OK ;
}

/* 
 * deletes a command both. After
 * this call the contents of command
 * AS WELL AS the pointer itself
 * will be free'd
 */
void delete_command( command_node_t* command ) {
    if( ! command ) 
        return ;

    switch ( command->type ) {
    case COMMAND_RAW:
        free( command->raw.raw ) ;
        break ;

    case COMMAND_SYNDEF:
        free( command->syndef.keyword ) ;
        break ;

    case COMMAND_QUERY:
        free( command->query.query ) ;
        break ; 
    }

    free( command ) ;
}

const char* radiation_next() {
    /* check to make sure the library
     * is initialized */
	if( ! g_is_initialized ) {
		err_printf( "Library not initialized. Call vim_init first!" ) ;
		g_error_code = RADIATION_ENOINIT ;
		return NULL ;
	}

	command_node_t* take = NULL ;
	char buf [ RADIATION_MAX_COMMAND_LEN ] ;

    /* free the last command that was on
     * the queue */
	free(g_del_bucket) ;
	g_del_bucket = NULL ;

    lprintf("Taking from the blocking queue\n") ;

    /* try to pull something off of the queue.
     * The way it extists now the plugin will
     * wait a second for more data before a
     * timout will occur */
	if( blocking_queue_take( g_current_radiator->data_queue, (void**)&take, 1000 ) == BQ_TIMEOUT ) {
		err_printf( "Waiting for next timed out" ) ;
		g_error_code = RADIATION_ETIMEOUT ;
	}

    /* There is no timeout and we can process 
     * the data that was returned */
    else{
        lprintf("Took data %p\n", take) ;

        if( take == NULL )  {
            /* NULL marks EOF so
             * lets do cleanup */
            free(g_del_bucket) ;
            g_del_bucket = NULL ;
        }

        else {

            switch ( take->type ) {
            
            /* act differently for the different command
             * types */
            case COMMAND_SYNDEF:
		        lprintf( "[syndef] - sy keyword %s %s\n", take->syndef.hgroup, take->syndef.keyword) ;
		        snprintf(buf, RADIATION_MAX_COMMAND_LEN, "sy keyword %s %s", take->syndef.hgroup, take->syndef.keyword) ;
		        g_del_bucket = strdup( buf ) ;
                break ;

            case COMMAND_RAW:
                /* For a raw command, a simple assignment
                 * will do */
		        lprintf( "[raw] - %s\n", take->raw.raw) ;
                g_del_bucket = take->raw.raw ;
                take->raw.raw = NULL ;
                break ;

            case COMMAND_QUERY:
                lprintf( "[query] - %s\n", take->query.query ) ;

                /* a litte hack to signal a query */
                snprintf( buf, RADIATION_MAX_COMMAND_LEN, "q:%s", take->query.query ) ;
                g_del_bucket = strdup( buf ) ; 
                break ;
            }

            delete_command( take ) ;
        }
	}

    lprintf("Returning %s\n", g_del_bucket) ;
	return g_del_bucket ;
}

const char* radiation_get_error_message( void ) {
	return g_error_message ;
}

int init_radiator( radiator_t* rad, radiate_file_routine_t routine ) {
    rad->data_queue    = new_blocking_queue() ;
    rad->message_queue = new_blocking_queue() ;
    rad->routine  = routine ;
    
    return 0 ;
}

void message_delete( message_t* mesg ) {
    if( mesg ) {
        free( mesg->strval ) ;
        free( mesg ) ;
    }
}

void radiator_queue_command( radiator_t* radiator, command_node_t* node ) {
    blocking_queue_add( radiator->data_queue, node ) ;
}

int radiator_read_message( radiator_t* radiator, uint64_t timeout, message_t** ret ) {
    return blocking_queue_take( radiator->message_queue, (void**)ret, timeout ) ;
}

/* signals to the python code that we need to know
 * the value of a variable before we can continue */
int radiator_query_variable( radiator_t* rad, const char* var, message_t** ret ) {
    command_node_t* command = malloc( sizeof( command_node_t* ) ) ;

    command->type = COMMAND_QUERY ;
    command->query.query = strdup(var) ;

    /* put the query command on the queue
     * for the python to get */
    radiator_queue_command( rad, command ) ;

    /* delete the old message */
    message_delete( * ret ) ;

    /* read the message with a 10 ms timeout */
    return radiator_read_message( rad, 10, ret ) ;
}

char* radiator_query_variable_default( radiator_t* rad, const char* var, const char* def ) {
    message_t* ret = NULL ;
    char* mesg ;

    if( radiator_query_variable( rad, var, &ret ) ) {
        message_delete( ret ) ;
        return strdup( def ) ; 
    }

    if( ret->type == MESSAGE_STRING_VALUE ) {
        mesg = ret->strval ;
        ret->strval = NULL ;
    } else {
        mesg = strdup( def ) ;
    }

    message_delete( ret ) ;

    return mesg ;
}

int radiation_put_string_message( const char* message ) {
    message_t* tmp = malloc( sizeof( message_t ) ) ;
    lprintf("putting message: %s\n", message) ;

    tmp->type = MESSAGE_STRING_VALUE ;

    if( message ) {
        tmp->stringval.value = strdup(message) ;
    } else {
        tmp->stringval.value = NULL ;
    }

    blocking_queue_add( g_current_radiator->message_queue, tmp ) ;

    return 0 ;
}

int radiator_wait_digest( radiator_t* rad, uint64_t timeout ) {
    return blocking_queue_wait_digest( rad->data_queue, timeout ) ;
}

int radiation_put_error_message( const char* message, int errorcode ) {
    message_t* tmp = malloc( sizeof( message_t ) ) ;

    tmp->type = MESSAGE_ERROR ;

    if( message ) {
        tmp->errorval.message = strdup(message) ;
    } else {
        tmp->errorval.message = NULL ;
    }

    tmp->errorval.code = errorcode ;;

    blocking_queue_add( g_current_radiator->message_queue, tmp ) ;

    return 0 ;
}
