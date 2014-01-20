#ifndef CBLACKLIST_H_
#define CBLACKLIST_H_

/*
 * Author: jrahm
 * created: 2014/01/19
 * cblacklist.h: <description>
 */

#include "tree.h"

int blacklist_contains( const char* val ) ;

void blacklist_insert( const char * val ) ;

void blacklist_init( const char** vals ) ;

#endif /* CBLACKLIST_H_ */
