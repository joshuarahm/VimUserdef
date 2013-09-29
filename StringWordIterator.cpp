#include "StringWordIterator.hpp"

#include <cctype>
#include <cstdio>

inline bool is_id_char( char ch ) {
    return ch == '_' || isalnum( ch ) ;
}

bool StringWordIterator::nextWord( std::string& word ) {
    word.clear();
    while( offset_front < str.length() &&
        isspace( str[offset_front] ) )
        offset_front ++;

    while( offset_front < str.length() &&
        is_id_char( str[offset_front] ) ) {
        word += str[offset_front++];
    }

    if( word.length() == 0 ) {
        return false;
    }
    return true;
}

bool StringWordIterator::lastWord( std::string& word ) {
    word.clear();
    while( offset_back >= 0 && 
        isspace( str[ offset_back ] ) ) {
        offset_back --;
    }

    size_t tmp = offset_back;
    while( offset_back >= 0 &&
        is_id_char( str[ offset_back ] ) ) {
        offset_back --;
    }

    // printf( "tmp: %d; offset_back: %d\n", (int)tmp, (int)offset_back );
    word = str.substr( offset_back + 1, tmp - offset_back + 1 );

    if( word.length() == 0 )
        return false;
    return true;
}
