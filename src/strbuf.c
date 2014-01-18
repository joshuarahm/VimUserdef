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
	strbuf_t* ret = malloc( sizeof( strbuf_t ) ) ;
	
	ret->total_size = size; 
	ret->len = 0 ;

    ret->buffer = malloc( size ) ;

	return ret ;
}

size_t strbuf_read( strbuf_t* buf, FILE* file ) {
    return buf->len = fread( buf->buffer, 1, buf->total_size, file ) ;
}

size_t strbuf_cut_offset( strbuf_t* buf, FILE* file, int offset ) {
    int len = buf->len - offset ;
    if ( len > 0 ) {
        memmove( buf->buffer, buf->buffer + offset, len ) ;
    } else {
        len = 0 ;
    }

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


struct strbuf_pcre_info {
    const pcre* regex ;
    const pcre_extra* extra ;
    int rel_pointer ;
} ;

/* run all the matches of one regex.
 * returns a positive offset of the
 * next parital match, or buf->total_size for
 * no match */
static int strbuf_exhaust( strbuf_t* buffer, ovector_t* vec,
    struct strbuf_pcre_info* info, int options,
    match_callback_t callback, int idx ) {
    
    /* return code */
    int rc = 1;
    int offset = 0 ;
    int i, off1, off2 ;

    while ( 1 ) {
        /* execute the regular expression on
         * the buffer */
        rc = strbuf_pcre_exec( buffer, info->regex, info->extra, offset, vec, options | PCRE_PARTIAL ) ;
        if( rc > 0 ) {
            for( i = 0 ; i < rc; ++ i ) {
                /* There was a match. Iterate through
                * the groups and call the callback */
                off1 = vec->ovector[2 * i] ;
                off2 = vec->ovector[2 * i + 1] ;
    
                callback( buffer->buffer + off1 , off2 - off1 , idx, i ) ;
            }

            /* move the offset */
            offset = off2 + 1 ;
        } else {

            if( rc == PCRE_ERROR_PARTIAL ) {
                /* this is a partial match and we should
                 * communicate that */
                info->rel_pointer = vec->ovector[0] ;
                return 0 ;
            } else if( rc == PCRE_ERROR_NOMATCH ) {
                /* we do not have a match at all,
                 * so return -1 */
                info->rel_pointer = buffer->total_size ;
                return -1 ;
            }

        }
    }
}

void strbuf_stream_regex8(
    strbuf_t* buffer,
    ovector_t* vec,
    FILE* file,
    const pcre** code,
    const pcre_extra** extra,
    size_t nres,
    int options,
    match_callback_t callback ) {
    
    strbuf_read( buffer, file );

    struct strbuf_pcre_info* info_arr = malloc( sizeof( struct strbuf_pcre_info ) * nres ) ;
    size_t i ;
    for( i = 0 ; i < nres ; ++ i ) {
        info_arr[i].regex = code[i] ;
        info_arr[i].extra = extra[i] ;
        info_arr[i].rel_pointer = 0 ;
    };

    size_t smallest_idx = 0 ;

    while ( 1 ) {
        struct strbuf_pcre_info* smallest = &info_arr[smallest_idx] ;
        int shift = smallest->rel_pointer; 

        /* cut off the buffer at the start of the closest
         * partial match */
        if( shift != 0 && strbuf_cut_offset( buffer, file, shift ) <= 0 ) {
            /* there is nothing more to read */
            break ;
        }

        /* Make all the matches possible with the current
         * regex and the current buffer */
        strbuf_exhaust( buffer, vec, smallest, options, callback, smallest_idx ) ;

        /* update the relative pointers to account for the
         * shifted buffer and find the new smallest */
        size_t new_smallest_idx = smallest_idx ;
        for( i = 0 ; i < nres ; ++ i ) {
            if( i != smallest_idx ) {
                /* update everything except
                 * the one which was just run */
                info_arr[i].rel_pointer -= shift ;
                if( info_arr[i].rel_pointer <= info_arr[new_smallest_idx].rel_pointer ) {
                    /* set the new smallest to the current
                     * value of i. Give the benefit of the
                     * doubt to the new guy */
                    new_smallest_idx = i ;
                }
            }
        }

        smallest_idx = new_smallest_idx ;
    }

    free( info_arr );
}

int strbuffer_delete( strbuf_t* buf ) {
    free( buf->buffer ) ;
    free( buf ) ;

    return 0 ;
}
