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

/* Stream buffer struct. Used for
 * -realaively- easy and easy eiting
 * of streams.
 */
typedef struct strbuf {
	/* total number of bytes allocated
	 * to the buffer */
	size_t total_size ;

	/* the total length of the string
	 * held in buffer */
	size_t len ;

	/* the number of bytes to roll
	 * over from the last read to the
	 * beginning */
	size_t keep ;

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

int ovector_first_match( ovector_t* ovector ) ;

/* creates a new strbuf */
strbuf_t* new_strbuf( size_t size, size_t keep ) ;

/* reads as many bytes into strbuf as possible
 * returns the number of bytes read from the file */
size_t strbuf_read( strbuf_t* buf, FILE* file ) ;

size_t strbuf_cut_offset( strbuf_t* buf, FILE* file, int offset ) ;

/* returns the next string that the
 * ovector points to as a strdup'd
 * version of the string that must
 * be free'd */
char* strbuf_next_capture( strbuf_t* buf, ovector_t* itr ) ;

char* strbuf_get_capture( strbuf_t* buf, ovector_t* itr, size_t off ) ;

int strbuf_pcre_exec(
	strbuf_t* buf,
	const pcre* code,
	const pcre_extra* extra,
	size_t offset , 
	ovector_t* ovec ,
	int options ) ;


#endif /* STRBUF_H_ */
