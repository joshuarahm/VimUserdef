#include "HighlighterC.hpp"
#include "StringWordIterator.hpp"
#include <unistd.h>
#include <cstdio>
#include <algorithm>
#include <functional> 
#include <cctype>
#include <locale>
#include <pcrecpp.h>
#include <set>

using namespace std;

void CTokenIterator::operator++() {
    static const string typedef_str = "typedef";

    /* This acts as a buffer */
    this->m_cur_str.clear();

    if( ! this->hasNext() )
        return;

    size_t i = m_offset;
    size_t to = m_parse_str.length();

    bool is_typedef = false;

    for( ; i < to ; ++ i ) {
        char ch = m_parse_str[i];
        if( m_in_quotes && ch == '\\' ) {
            // No questions asked
            m_cur_str += m_parse_str[i ++];
        } else {
            /* Are we in a typedef block.
             * If so, then we are not allowed
             * to break a statement on a }
             */
            is_typedef = is_typedef ||
                m_parse_str.compare( i, typedef_str.length(), typedef_str ) == 0;
            /* Set the variables to reflect the
             * character position */
            p_UpdateForChar( ch );
            if( ! m_in_quotes && m_body == 0 ) {
                /* If I am not in quotes and
                 * am at the outer body level */
                if( ch == ';' ) {
                    // end of statement
                    i ++;
                    break ;
                } else if( ch == '\n' && m_in_hash ) {
                    m_in_hash = false;
                    i ++;
                    break ;
                } else if ( ch == '}' && ! is_typedef ) {
                    m_cur_str += ch;
                    i ++;
                    break ;
                }
            }
            // otherwise append the char
            if( isspace( ch ) && !m_in_quotes ){
                if( !m_space ) { 
                    m_cur_str += ch;
                    m_space = true;
                }
            } else {
                m_space = false;
                m_cur_str += ch;
            }
        }
    }

    // update the new offset
    m_offset = i;

    return ;
}

static int extract_function_typedef( const string& str, string& buf ) {
    static const pcrecpp::RE regex("\\(\\s*\\*\\s*(\\w+)\\s*\\)");
    
    if (regex.error().length() > 0) {
        cerr << "PCRE compilation failed with error: " << regex.error() << endl;
        return false;
    }
    if( regex.PartialMatch( str.c_str(), &buf ) ) {
        return true;
    }

    return false;
}

int CHighlighter::p_TryParseFunction( const string& str, vector<string>& into ) {
    static const pcrecpp::RE regex( "\\s*(\\w+(\\s*\\*\\s*)*\\s+)+\\**(\\w+)\\s*\\(" );

    string buf;
    if (regex.error().length() > 0) {
        cerr << "PCRE compilation failed with error: " << regex.error() << endl;
        return false;
    }

    if( regex.PartialMatch( str.c_str(), &buf, &buf, &buf ) ) {
        p_AddTo( buf, into );
        return true;
    }

    return false;
}

static int try_extact_no_attr( const string& str, string& buf ) {
    static const pcrecpp::RE regex("(\\w+)\\s+__attribute__");
    
    if (regex.error().length() > 0) {
        cerr << "PCRE compilation failed with error: " << regex.error() << endl;
        return false;
    }
    if( regex.PartialMatch( str.c_str(), &buf ) ) {
        return true;
    }

    return false;
}

int CHighlighter::p_ParseEnumConstants (const string& str, vector<string>& into ) {;
    size_t i;
    for( i = 0; i < str.length() && str[i] != '{'; i ++ );
    if( i == str.length() ) {
        return 0;
    }
    ++ i;
    string buf;
    for( ; i < str.length() && i != '}'; ++ i ) {
        while( i < str.length() && isspace(str[i]) ) i ++;
        while( i < str.length() && isalnum(str[i]) )
            buf += str[i++];
        while( i < str.length() && isspace(str[i]) ) i ++;
        p_AddTo( buf, into );
        buf.clear();
    }
}

void CHighlighter::p_ParseToken( string& token ) {
    /* Iterate through the words of the
     * token */
    StringWordIterator words( token );
    string buf;

    words.nextWord( buf );
    if( buf == "typedef" ) {
        words.nextWord( buf );
        if( buf == "struct" || buf == "enum" || buf == "union" ) {
            if( buf == "enum" ) {
                p_ParseEnumConstants( token, this->m_types );
            }
            if( words.nextWord( buf ) ) {
                p_AddTo( buf, m_types );
            }
        }
        if( ! words.lastWord( buf ) ) {
             extract_function_typedef( token, buf ) ||
             try_extact_no_attr( token, buf );
            //printf("BUF: %s\n", buf.c_str() );;
        }
        p_AddTo( buf, m_types );
    }
    else if ( p_TryParseFunction( token, this->m_functions ) ) {
    
    }
    else if ( buf == "struct" || buf == "enum" || buf == "union" ) {
        if( buf == "enum" ) {
            p_ParseEnumConstants( token, this->m_constants );
        }
        if( words.nextWord( buf ) ) {
            p_AddTo( buf, m_types );
        }
    }
}

void CHighlighter::runHighlight( const string& str ) {
    CTokenIterator tokens( str );

    for( ; tokens.hasNext() ; ++ tokens ) {
//        printf( "TOKEN: %s\n", (*tokens).c_str() );
        p_ParseToken( tokens.get() );
    }
}

const vector<string>* CHighlighter::getHighlightsForGroup( const string& group ) {
    if( group == "Type" ) {
        return & m_types;
    } else if ( group == "Constant" ) {
        return & m_constants;
    } else if ( group == "Function" ) {
        return & m_functions;
    }
    return NULL;
}

int main( int argc, char** argv ) {

    if( argc < 2 ) {
        return -1;
    }

    string output;

    char buf[4096];
    char* cc = getenv("CC");
    char* cflags = getenv("CFLAGS");
    if( cc == NULL ) cc = (char*)"gcc";
    if( cflags == NULL ) cflags = (char*)"";

    snprintf( buf, sizeof(buf), "%s -E %s %s", cc, argv[1], cflags );
    FILE* tmp = popen( buf, "r" );
    size_t bytes_read;

    while( (bytes_read = fread( buf, 1, sizeof(buf) - 1, tmp )) > 0 ) {
        buf[bytes_read] = '\0';
        output.append( buf, 0, bytes_read );
    }

    CHighlighter highlight;
    highlight.runHighlight( output );

    for( vector<string>::const_iterator itr = highlight.getHighlightGroups().begin() ;
        itr != highlight.getHighlightGroups().end() ; ++ itr ) {
        const vector<string>* vec = highlight.getHighlightsForGroup( *itr );
        if( vec != NULL ) {
            for( vector<string>::const_iterator itr2 = vec->begin() ;
                itr2 != vec->end() ; ++ itr2 ) {
                printf("syn keyword userdef%s %s\n", itr->c_str(), itr2->c_str() );
            }
        }
    }

    return 0;
}
