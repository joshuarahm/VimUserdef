#include "HighlighterJava.hpp"
#include <iostream>
#include <fstream>
#include <queue>
#include <pcrecpp.h>
#include <sstream>

#include <unistd.h>

using namespace std;

void readImportClasses( istream& instream, vector<string>& into ) {
    /* Should match java import statements */
    /* import<blah>(com.blah.Blah); */
    static const pcrecpp::RE import_regex( "\\s*import.*\\s((\\w|\\.)+)\\s*$" );
    static const pcrecpp::RE ending_regex( "(public|private|protected)\\s*class" );

    string instr;
    string import_class;

    getline( instream, instr, ';' );
    while( !ending_regex.PartialMatch(instr.c_str()) && ! instream.eof() ) {
        if( import_regex.PartialMatch( instr.c_str(), &import_class, (string*)NULL ) ) {
            into.push_back( import_class );
        }
        getline( instream, instr, ';' );
    }
}

void stringSplit( const std::string& str, char delim, std::set<string>& into ) {
    size_t index = 0;
    size_t next;
    while( index < str.length() ) {
        next = str.find( delim, index );
        if( next < str.length() ) {
            string tmp = str.substr( index, next - index );
            into.insert( string("\"") + tmp + "\\.\""  );
        }
        index = next + 1;
    }
}

void JavaHighlighter::highlightFile( const string& file ) {
    static const pcrecpp::RE class_regex( "class ((\\w+\\.)*)(\\w+) " );
    static const pcrecpp::RE function_regex( " (\\w+)\\(.*\\);" );
    static const pcrecpp::RE member_regex( " (\\w+);");

    ifstream instream;
    instream.open( file.c_str() );
    vector<string> imports;

    const char* cmd_args = getenv( "JAVAP_FLAGS" );
    if( cmd_args == NULL ) cmd_args = "";

    readImportClasses( instream, imports ); 

    stringstream cmd;
    cmd << "/usr/bin/javap ";
    cmd << cmd_args << " ";

    for( vector<string>::iterator i = imports.begin(); i != imports.end(); ++ i ) {
        // cout << "Import: " << *i << endl;
        cmd << *i << " ";
    }

    cerr << "Command: " << cmd.str() << endl;
    FILE* tmp = popen( cmd.str().c_str(), "r" ) ;

    char buf[4096];
    string piece;
    string package;
    string last_class;
    std::vector<string> packages;
    while( fgets( buf, sizeof( buf ), tmp ) != NULL ) {
        // printf( "%s", buf );
        if( function_regex.PartialMatch( buf, &piece ) ) {
            m_methods.insert( piece );
        } else if( class_regex.PartialMatch( buf, &package, (string*)NULL, &last_class ) ) {
            m_classes.insert( last_class );
            stringSplit( package, '.', m_packages );
        } else if( member_regex.PartialMatch( buf, &piece ) ) {
            m_members.insert( string("\"") + "\\." + piece + "\"" );
        }
    }
}
