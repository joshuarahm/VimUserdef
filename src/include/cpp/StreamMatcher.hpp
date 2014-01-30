#ifndef STREAMBUFFER_HPP_
#define STREAMBUFFER_HPP_

/*
 * Author: jrahm
 * created: 2014/01/26
 * StreamBuffer.hpp: <description>
 */

#include <iostream>
#include "cpp/PcreRegex.hpp"

/*
 * An interface which is called when a regex
 * is matched.
 */
class RegexMatchCallback {
public:

	/*
	 * Called when a regular expression matches
	 * a Regular expression
	 */
	virtual void regexMatched(
		
		/*
		 * The string that was matched.
		 */
		std::string matched,

		/*
		 * The number of the group that was
		 * matched
		 */
		size_t group ) = 0 ;
};

/*
 * An object ment to make matching regexes
 * easy along streams.
 */
class StreamMatcher {
public:
	/*
	 * construct a new stream buffer.
	 */
	StreamMatcher( size_t buffer_size=4096 ) ;

	/*
	 * Deconstructs the stream matcher
	 */
	~StreamMatcher() ;
	/*
	 * Set the callback to be called 
	 * when there is a regular expression
	 * match
	 */
	void setRegexMatchCallback( RegexMatchCallback* callback ) ;
	
	/*
	 * Set the stream for this matcher.
	 * For use when there needs to be another
	 * match
	 */
	void setStream( std::istream& stream ) ;

	/*
	 * Execute the regex `code` on the stream
	 */
	void executeRegex( PcreRegex* regex, size_t ovec_size=10 ) ;

private:

	/*
	 * shift the buffer down.
	 */
	void shift( size_t offset ) ;

	void dispatch_ovector( int* ovec, size_t match ) ;

	/*
	 * The stream to try to match
	 * patterns on.
	 */
	std::istream* m_stream ;

	/*
	 * The buffer to read from the
	 * stream into
	 */
	char* m_buffer ;

	/* The length of the buffer... the
	 * number of valid characters in the
	 * buffer */
	size_t m_buffer_len ;

	/* the total size allocated to the buffer */
	size_t m_buffer_size ;

	/* The callback currently registered */
	RegexMatchCallback* m_callback ;
};

#endif
