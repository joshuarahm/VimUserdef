#include "util/strbuf.h"
#include <string.h>

ovector_t* new_ovector( int size ) {
	if( size <= 0 ) 
		return NULL ;

	ovector_t* ret = calloc(sizeof( ovector_t ) + size * 3 * sizeof( int ), 1 ) ;

	ret->count = size ;
	return ret ;
}

strbuf_t* new_strbuf( size_t size ) {
	strbuf_t* ret = malloc( sizeof( strbuf_t ) + size ) ;
	
	ret->total_size = size; 
	ret->len = 0 ;

	return ret ;
}

size_t strbuf_read( strbuf_t* buf, FILE* file ) {
    return buf->len = fread( buf->buffer, 1, buf->total_size, file ) ;
}

size_t strbuf_cut_offset( strbuf_t* buf, FILE* file, int offset ) {
    int len = buf->len - offset ;
    strncpy( buf->buffer, buf->buffer + offset, len ) ;

    return buf->len = fread( buf->buffer + len, 1, buf->total_size - len, file ) + len ;
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

void strbuf_stream_regex7(
    strbuf_t* buffer,
    ovector_t* vec,
    FILE* file,
    const pcre* code,
    const pcre_extra* extra,
    int options,
    match_callback_t callback ) {
    
    /* return code */
    int rc = 1;
    int offset = 0 ;
    int i, off1, off2 ;
    strbuf_read( buffer, file );

    while ( 1 ) {
        /* execute the regular expression on
         * the buffer */
        rc = strbuf_pcre_exec( buffer, code, extra, offset, vec, options | PCRE_PARTIAL ) ;
        if( rc > 0 ) {
            for( i = 0 ; i < rc; ++ i ) {
                /* There was a match. Iterate through
                * the groups and call the callback */
                off1 = vec->ovector[2 * i] ;
                off2 = vec->ovector[2 * i + 1] ;
    
                callback( buffer->buffer + off1 , off2 - off1 , i ) ;
            }

            /* move the offset */
            offset = off2 ;
        } else {

            if( rc == PCRE_ERROR_PARTIAL ) {
                /* There is a partial match, so
                * we need to shift the buffer over */
                offset = 0 ;
                strbuf_cut_offset( buffer, file, vec->ovector[2 * i] ) ;
            } else if( rc == PCRE_ERROR_NOMATCH ) {
                /* there was not even a partial
                * match, so replace the buffer */
                offset = 0 ;
                if( strbuf_read( buffer, file ) <= 0 ) {
                    /* if there is no more to read
                    * from the stream, then exit */
                    break ;
                }
            }

        }
    }
}
