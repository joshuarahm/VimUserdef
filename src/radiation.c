#ifdef linux
#include <bsd/sys/tree.h>
#else

#ifdef BSD || __APPLE___
#include <sys/tree.h>
#else

#ifdef TREE_H_PATH
#include TREE_H_PATH
#else
/* can't compile for dozer yet :-( */
#error Cannot find the tree.h file. If you know where it is, add a #define called TREE_H_PATH with the path of the file surrounded by quotes.

#endif
#endif
#endif

#define TEST radiation.h
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

static void* run_as_thread( void* args_ ) {
	struct thread_args* args = (struct thread_args*)args_ ;

	g_current_radiator->radiate_file(
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

        if( ec != 0 ) {
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

const char* radiation_next() {
	if( ! g_is_initialized ) {
		err_printf( "Library not initialized. Call vim_init first!" ) ;
		g_error_code = RADIATION_ENOINIT ;
		return NULL ;
	}

	queue_value_t* take = NULL ;
	char buf [ RADIATION_MAX_COMMAND_LEN ] ;

	if( g_del_bucket ) {
		free(g_del_bucket) ;
		g_del_bucket = NULL ;
	}

    lprintf("Taking from the blocking queue\n") ;
	if( blocking_queue_take( g_current_radiator->data_queue, (void**)&take, 1000 ) == BQ_TIMEOUT ) {
		err_printf( "Waiting for next timed out" ) ;
		g_error_code = RADIATION_ETIMEOUT ;
	} else{
        lprintf("Took data %p\n", take) ;
        if( take != NULL ) {
            lprintf("sy keyword %s %s\n", take->hgroup, take->keyword) ;
		    snprintf(buf, RADIATION_MAX_COMMAND_LEN, "sy keyword %s %s", take->hgroup, take->keyword) ;
		    g_del_bucket = strdup( buf ) ;
        }
	}

    if( take != NULL ) {
	    free( take->keyword ) ;
	    free( take ) ;
    }

    lprintf("Returning %s\n", g_del_bucket) ;
	return g_del_bucket ;
}

const char* radiation_get_error_message( void ) {
	return g_error_message ;
}