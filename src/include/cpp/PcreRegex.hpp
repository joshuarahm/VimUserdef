#ifndef PCREREGEX_HPP_
#define PCREREGEX_HPP_

/*
 * Author: jrahm
 * created: 2014/01/26
 * PcreRegex.hpp: <description>
 */

#include <pcre.h>
#include <string>

/* 
 * a class to abstract away the
 * ovector in pcre exec
 */
class PcreOutVector {
public:
	/*
	 * Construct a new out vector allocating
	 * len spots for the output vector
	 */
	PcreOutVector( size_t len=10 ) ;

	/*
	 * Deconstruct the out vector
	 */
	~PcreOutVector() ;

	int operator[]( size_t i ) const ;

	/*
	 * Returns the substring of buffer that the group
	 * represents and stores the result in buffer.
	 *
	 * If there is no substring for the group then
	 * -1 is returned otherwise 0 is.
	 */
	int getSubstring( const char* str, int group, std::string& buffer ) const ;

	/*
	 * Convert this vector into an
	 * int *
	 */
	operator int*() ;

	inline size_t getLength() {
		return len ;
	}

private:
	size_t len ;
	int* outp ;
} ;

class PcreCompileError {
public:
	const char* error ;
	int offset ;
};

class PcreRegex {
public:
	/*
	 * Compile a regular expression
	 */
	static PcreRegex* compile( const char* pattern, int options=0 ) ;

	/*
	 * Execute the regex on the subject
	 */
	int execute( const char* subject, int length, PcreOutVector& vector, int options=0 ) ;

private:
	/* the regular expression */
	pcre* regex ;

	/* extra arguments */
	pcre_extra* extra ;
} ;

#endif /* PCREREGEX_HPP_ */
