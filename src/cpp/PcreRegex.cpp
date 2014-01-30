#include "cpp/PcreRegex.hpp"

using namespace std ;

PcreOutVector::PcreOutVector( size_t len ) {
	this->outp = new int[len * 3] ;
	this->len = len * 3 ;
}

PcreOutVector::~PcreOutVector() {
	delete[] outp ;
}

int PcreOutVector::operator[]( size_t i ) const {
	if( i > this->len ) {
		return -1 ;
	}

	return outp[ i ] ; 
}

int PcreOutVector::getSubstring( const char* str, int group, std::string& buffer ) const {
	int off1 = (*this)[ 2*group ] ;
	int off2 = (*this)[ 2*group+1 ] ;

	if( off1 == -1 || off2 == -1 ) {
		return -1 ;
	}

	buffer.assign( str, off1, off2 - off1 ) ;
	return 0;
}

PcreOutVector::operator int*() {
	return this->outp ;
}

PcreRegex* PcreRegex::compile( const char* pattern, int options ) {
	PcreRegex* regex = new PcreRegex();
	PcreCompileError error ; 

	pcre *re = pcre_compile( pattern, options, &error.error, &error.offset, NULL ) ;

	if ( ! re ) {
		delete regex ;
		throw error;
	}

	error.offset = -1;
	pcre_extra* extra = pcre_study( re, 0, &error.error ) ;

	if ( ! extra ) {
		delete regex ;
		throw error ;
	}
	
	regex->regex = re ;
	regex->extra = extra ;
	return regex ;
}

int PcreRegex::execute( const char* subject, int length, PcreOutVector& vector, int options ) {
	return pcre_exec( this->regex, this->extra, subject,
		length, 0, options, (int*)vector, vector.getLength() );
}
