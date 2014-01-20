#include "c_blacklist.h"
#include <string.h>
#include <stdlib.h>

struct blacklist_node {
	RB_ENTRY(blacklist_node) entry;
	const char* value ;
} ;

static int blacklist_node_cmp( struct blacklist_node* node1, struct blacklist_node* node2 ) {
	return strcmp( node1->value, node2->value ) ;
}

RB_HEAD( blacklist, blacklist_node ) ;
RB_GENERATE( blacklist, blacklist_node, entry, blacklist_node_cmp  ) ;


typedef struct blacklist blacklist_t ;
blacklist_t* c_blacklist ;


int blacklist_contains( const char* val ) {
	struct blacklist_node tmp ;
	tmp.value = val ;
	return RB_FIND( blacklist, c_blacklist, &tmp ) != NULL  ;;
}

void blacklist_insert( const char * val ) {
	struct blacklist_node* insert = calloc( sizeof( struct blacklist_node ), 1 ) ;
	insert->value = val ;
	RB_INSERT( blacklist, c_blacklist, insert ) ;
}

void blacklist_init( const char** vals ) {
	c_blacklist = calloc ( sizeof( blacklist_t ), 1 ) ;
	struct blacklist tmp = RB_INITIALIZER( &tmp ) ;
	*c_blacklist = tmp ;

	while ( *vals != NULL ) {
		blacklist_insert( *vals ) ;
		vals ++ ;
	}
}

