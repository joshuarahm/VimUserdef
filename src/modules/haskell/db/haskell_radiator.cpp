#include "ghc_pkg_database.h"

extern "C" {
#include "util/subprocess.h"
}

#include <ext/stdio_filebuf.h>
#include <iostream>

using namespace std ;
using namespace __gnu_cxx ;

int haskell_radiate_file(
    radiator_t* radiator,
    const char* filename,
    const char* filetype,
    const char* env ) {

    (void) filename ;
    (void) filetype ;
    (void) env ;
    
    return 0 ;
}

GhcPackage* parsePackage( istream& stream ) {
    static const string exposed("exposed-modules: ") ;

    if( stream.eof() ) {
        return NULL ;
    }
    
    string word ;
    std::vector<std::string>* vec ;

    GhcPackage* ret = new GhcPackage() ;

    do {
        stream >> word ;

        if( word == "---" ) {
            return ret ;
        }
        
        if ( word.size() > 0 && word.at(word.size()-1) == ':' ) {
            word = word.substr(0,word.size() - 1) ;

            if ( word == "name" ) {
                stream >> word ;
                ret->setName( word ) ;
            } else {
                vec = &ret->getVector( word ) ;
            } 
        } else if( word.size() > 0 ) {
            vec->push_back( word ) ;
        }


    } while( ! stream.eof() ) ;

    return ret ;
}

GhcPackageDatabase* CreateGhcDatabase( const char* ghc_pkg_binary ) {
    GhcPackageDatabase* ret = new GhcPackageDatabase() ;

    char* args[] = {(char*)"ghc-pkg",(char*)"dump", NULL} ;
    FILE* fd = spawn_oevp( ghc_pkg_binary, args ) ;

    if( ! fd ) {
        perror("Unable to open file") ;
        return NULL ;
    }

    stdio_filebuf<char> fdbuf(fd, std::ios::in);
    istream is( &fdbuf ) ;

    GhcPackage* package ;
    while( (package = parsePackage( is ) ) != NULL ) {
        ret->addPackage( package->getName(), package ) ;
    }

    return ret ;
}
