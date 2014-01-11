#ifdef linux
#include <bsd/sys/tree.h>
#else
#ifdef BSD
#include <sys/tree.h>
#else
#error Unable to compile for this system
#endif
#endif

#include "radiation.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define RADIATION_ERROR_MESSAGE_LEN 1024
#define RADIATION_MAX_COMMAND_LEN 1024

#define err_printf( fmt, ... ) \
	snprintf( g_error_message, RADIATION_ERROR_MESSAGE_LEN, fmt, ##__VA_ARGS__) ;

#define vim_return( str, el ) \
	const char* _tmp3293__ = (str) ; \
	return _tmp3293__ == NULL ? el : _tmp3293__ ;

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
	g_registry = reg ;

	g_is_initialized = 1 ;
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

	queue_value_t* take ;
	char buf [ RADIATION_MAX_COMMAND_LEN ] ;

	if( g_del_bucket ) {
		free(g_del_bucket) ;
		g_del_bucket = NULL ;
	}

	if( blocking_queue_take( g_current_radiator->data_queue, (void**)&take, 15 ) != BQ_TIMEOUT ) {
		err_printf( "Waiting for next timed out" ) ;
		g_error_code = RADIATION_ETIMEOUT ;
	} else{
		snprintf(buf, RADIATION_MAX_COMMAND_LEN, "sy keyword %s %s", take->hgroup, take->keyword) ;
		g_del_bucket = strdup( buf ) ;
	}

	free( take->keyword ) ;
	free( take->hgroup ) ;
	free( take ) ;

	return g_del_bucket ;
}

const char* radiation_get_error_message( void ) {
	return g_error_message ;
}
