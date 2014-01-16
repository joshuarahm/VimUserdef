#ifndef STRBUF_H_
#define STRBUF_H_

/*
 * Author: jrahm
 * created: 2014/01/14
 * strbuf.h: <description>
 */

#include <stdlib.h>
#include <stdio.h>

#include <pcre.h>

typedef struct {
	size_t count ;
	int itr ;
	int ovector[] ;
} ovector_t ;

typedef void (*match_callback_t)(const char* str, size_t strlen, int group);

/* Stream buffer struct. Used for
 * -realaively- easy and easy eiting
 * of streams.
 */
typedef struct strbuf {
	/* total number of bytes allocated
	 * to the buffer */
	size_t total_size ;

    /* the length of the string held in
     * the buffer */
	size_t len ;

	/* the array of characters */
	char buffer[] ;
} strbuf_t ;


/* creates a new ovector which may
 * be used to capture groups from the
 * regex */
ovector_t* new_ovector( int size ) ;

/* returns the length of the total
 * match */
size_t ovector_matchlen( ovector_t* ovector ) ;

/* returns the offset of the first match of
 * the ovector */
int ovector_first_match( ovector_t* ovector ) ;

/* creates a new strbuf */
strbuf_t* new_strbuf( size_t size ) ;

/* reads as much as possible into the strbuf
 * from the file `file`. This completely replaces
 * everything in the buffer */
size_t strbuf_read( strbuf_t* buf, FILE* file ) ;

/* mark the position in the buffer to save. This
 * part will be retained in the buffer and
 * the remaining will be filled by the file
 *
 * Returns the length of the string in the buffer after
 * the operation.
 */
size_t strbuf_cut_offset( strbuf_t* buf, FILE* file, int offset ) ;

void strbuf_stream_regex7(
    strbuf_t* buffer,
    ovector_t* vec,
    FILE* tostream,
    const pcre* code,
    const pcre_extra* extra,
    int options,
    match_callback_t callback ) ;

int strbuf_pcre_exec(
	strbuf_t* buf,
	const pcre* code,
	const pcre_extra* extra,
	size_t offset , 
	ovector_t* ovec ,
	int options ) ;


#endif /* STRBUF_H_ */
