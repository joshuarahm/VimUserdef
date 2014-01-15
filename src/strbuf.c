#include "util/strbuf.h"
#include <string.h>

ovector_t* new_ovector( int size ) {
	if( size <= 0 ) 
		return NULL ;

	ovector_t* ret = calloc(sizeof( ovector_t ) + size * 3 * sizeof( int ), 1 ) ;

	ret->count = size ;
	return ret ;
}

strbuf_t* new_strbuf( size_t size, size_t keep ) {
	strbuf_t* ret = malloc( sizeof( ovector_t ) + size + keep ) ;
	
	ret->total_size = size + keep; 
	ret->len = 0 ;
	ret->keep = keep ;

	return ret ;
}

size_t strbuf_read( strbuf_t* buf, FILE* file ) {
	int to_read = buf->len - buf->keep ;
	return strbuf_cut_offset( buf, file, to_read ) ;
}

size_t strbuf_cut_offset( strbuf_t* buf, FILE* file, int to_read ) {
	size_t ret ;
	char* read = buf->buffer ;
	int keep = buf->len - to_read ;

	if( keep > 0 ) {
		read += keep;
		strncpy( buf->buffer, buf->buffer + to_read, keep) ;
	} else {
		keep = 0 ;
	}

	ret = fread( read, to_read, 1, file ) ;
	buf->len = ret + keep ;
	return ret ;
}

char* strbuf_next_capture( strbuf_t* buf, ovector_t* itr ) {
	return strbuf_get_capture( buf, itr, itr->itr ++ ) ;
}

char* strbuf_get_capture( strbuf_t* buf, ovector_t* itr, size_t off ) {
	if( off >= itr->count ) {
		return NULL ;
	}

	int off1 = itr->ovector[ off * 2 ] ;
	int off2 = itr->ovector[ off * 2 + 1 ] ;

	int len = off2 - off1 ;

	if( ! len ) {
		return NULL ;
	}

	return strndup( buf->buffer + off1, len ) ;
}

int strbuf_pcre_exec( strbuf_t* buf, const pcre* code,
	const pcre_extra* extra, size_t offset,
	ovector_t* ovec, int options ) {
	return pcre_exec( code, extra, buf->buffer, buf->len, offset, options, ovec->ovector, ovec->count * 3 ) ;
}

size_t ovector_matchlen( ovector_t* ovector ) {
	return ovector->ovector[1] - ovector->ovector[0] ;
}

int ovector_first_match( ovector_t* ovector ) {
	return ovector->ovector[0] ;
}
