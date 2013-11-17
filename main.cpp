#include "Highlighter.hpp"
#include "HighlighterC.hpp"
#include "HighlighterJava.hpp"

#include <string>
#include <vector>

#include <cstring>
#include <map>

#include <cstdio>

using namespace std;

static Highlighter* newCHighlighter() { return new CHighlighter(); }
static Highlighter* newJavaHighlighter() { return new JavaHighlighter(); }

static map<string,Highlighter*(*)()> highlighter_map;

static const char* get_ext( const char* file ) {
    const char* ret = strrchr( file, '.' );
    return ret ? ret + 1 : NULL;
}

static void buildMap( ) {
    highlighter_map["c"] = newCHighlighter;
    highlighter_map["h"] = newCHighlighter;
    highlighter_map["java"] = newJavaHighlighter;
}

int main( int argc, char** argv ) {
    const char* ext;
    if( argc < 2 ) {
        return 1;
    }

    /* Build the mappoints between file extensions
     * and highlighting types */
    buildMap();

    /* The file extension */
    ext = get_ext(argv[1]);

    /* Get the correct factory to build to
     * correct highlighter */
    Highlighter*(*factory)() = highlighter_map[ext];
    if( ! factory ) {
        cerr << "No highlighting available for: " << ext << " extension!" << endl;;
        return 2;
    }
    

    Highlighter* highlighter = factory();

    /* run the highlighter on the file
     * given */
    highlighter->highlightFile( argv[1] );

    for( vector<string>::const_iterator itr = highlighter->getHighlightKeywordGroups().begin() ;
        itr != highlighter->getHighlightKeywordGroups().end() ; ++ itr ) {
        /* Iterate through all the highlighting
         * groups and print out the vim command
         * to add that highlight */
        const set<string>* vec = highlighter->getHighlightsForGroup( *itr );
        if( vec != NULL ) {
            /* Make sure that we still have a vector */
            for( set<string>::const_iterator itr2 = vec->begin() ;
                itr2 != vec->end() ; ++ itr2 ) {

                /* Print the VIM command, this way Vim just has
                 * to execute each line of the output and nothing
                 * more */
                printf("syn keyword userdef%s %s\n", itr->c_str(), itr2->c_str() );
            }
        }
    }

    for( vector<string>::const_iterator itr = highlighter->getHighlightMatchGroups().begin() ;
        itr != highlighter->getHighlightMatchGroups().end() ; ++ itr ) {
        /* Iterate through all the highlighting
         * groups and print out the vim command
         * to add that highlight */
        const set<string>* vec = highlighter->getHighlightsForGroup( *itr );
        if( vec != NULL ) {
            /* Make sure that we still have a vector */
            for( set<string>::const_iterator itr2 = vec->begin() ;
                itr2 != vec->end() ; ++ itr2 ) {

                /* Print the VIM command, this way Vim just has
                 * to execute each line of the output and nothing
                 * more */
                printf("syn match userdef%s %s\n", itr->c_str(), itr2->c_str() );
            }
        }
    }

    return 0;
}
