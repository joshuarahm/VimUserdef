#include <algorithm>

#include "cpp/StreamMatcher.hpp"
#include "cpp/PcreRegex.hpp"
#include "radiation.h"

using namespace std ;

StreamMatcher::StreamMatcher( size_t buffer_size ) {
	this->m_buffer = new char[buffer_size] ;
	this->m_stream = NULL ;
	this->m_callback = NULL ;
	this->m_buffer_size = buffer_size ;
	this->m_buffer_len = 0 ;
} ;

StreamMatcher::~StreamMatcher() {
	delete[] m_buffer ;
}

void StreamMatcher::shift( size_t offset ) {
	if ( m_stream && * m_stream ) {

		for( size_t i = offset ; i < m_buffer_len ; ++ i ) {
			/* Shift the characters over */
			m_buffer[i-offset] = m_buffer[i] ;
		}

		if( offset > m_buffer_len ) {
			offset = m_buffer_len ;
		}

		size_t len = m_buffer_len - offset ;
		int read = m_stream->readsome( m_buffer + len, m_buffer_size - len ) ;
		if( read < 0 ) {
			m_buffer_len = 0 ;
		} else {
			m_buffer_len = len + read ;
		}
	}
}

/*
 * Set the stream that this
 * class matches against
 */
void StreamMatcher::setStream( std::istream& stream ) {
	this->m_stream = & stream ;

	/* read initial chunk */
	m_buffer_len = m_stream->readsome( m_buffer, m_buffer_size ) ; 
}

/*
 * Set the callback to be called 
 * when there is a regular expression
 * match
 */
void StreamMatcher::setRegexMatchCallback( RegexMatchCallback* callback ) {
	this->m_callback = callback ;
}

/*
 * Execute the regex `code` on the stream
 */
void StreamMatcher::executeRegex( PcreRegex* regex, size_t ovec_size ) {
	ovec_size *= 3 ;
	int matches ;

	PcreOutVector ovector( ovec_size ) ;

	while ( this->m_buffer_len > 0 ) { 
		/* execut the regex.*/
		matches = regex->execute( this->m_buffer, this->m_buffer_len, ovector, PCRE_PARTIAL );
	
		if ( matches == PCRE_ERROR_PARTIAL ) {
			/* If there is a partial match, we shift
		 	* the buffer to thy to get a full match */
			if( ovector[0] == 0 ) {
				/* the match does not fit in the buffer */
				this->shift( this->m_buffer_size ) ;
			} else {
				/* shift to the beginning */
				this->shift( ovector[0] ) ;
			}
		} else if( matches > 0 ) {
			dispatch_ovector( ovector, matches ) ;
			/* Shift to pass the match */
			this->shift( ovector[1] ) ;
		} else {
			lprintf("Unhandled error code: %d\n", matches) ;
			this->shift( this->m_buffer_size ) ;
		}

	}
}

void StreamMatcher::dispatch_ovector( int* ovec, size_t match ) {
	for( int i = 0 ; i < (int)match ; ++ i ) {
		size_t off = ovec[2*i] ;
		size_t len = ovec[2*i+1] - off ;
		this->m_callback->regexMatched( string(m_buffer + off, len ), i ) ;
	}
}
