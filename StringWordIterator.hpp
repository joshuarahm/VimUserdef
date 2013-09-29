#ifndef STRING_WORD_ITERATOR_HPP_
#define STRING_WORD_ITERATOR_HPP_

#include <cstdlib>
#include <string>

class StringWordIterator {
public:
    StringWordIterator( const std::string& s ) :
        str( s ), offset_front( 0 ), offset_back( s.length() - 1 ) {}

    bool nextWord( std::string& word );
    bool lastWord( std::string& word );
    inline int nextChar() { return offset_front < str.length() ? str[offset_front++] : -1; }
    inline void rewind( int n ) { offset_front -= n ; }

private:
    const std::string& str;
    size_t offset_front;
    int offset_back;
};

#endif // STRING_WORD_ITERATOR_HPP_
