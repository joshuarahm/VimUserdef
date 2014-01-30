#include "haskell_radiator.hpp"

extern "C" {
#   include "util/subprocess.h"
}

#include <ext/stdio_filebuf.h>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <sys/stat.h>
#include <sstream>

#include "cpp/StreamMatcher.hpp"

struct HASKELL_RADIATOR haskell_radiator ;

using namespace std ;
using namespace __gnu_cxx ;

PcreRegex* import_regex ;
PcreRegex* function_regex ;



HaskellMatchCallback* callback ;

int haskell_radiate_file(
    radiator_t* radiator,
    const char* filename,
    const char* filetype,
    const char* env ) {

	(void) filetype ;
	(void) env ;
    
    Radiator rad( radiator ) ;
    ifstream stream( filename ) ;
    StreamMatcher matcher ;

    if( ! stream ) {
        rad.error("Unable to open file.") ;
        return -1 ;
    }

    matcher.setStream( stream ) ;
	matcher.setRegexMatchCallback( callback ) ;
	matcher.executeRegex( import_regex, 10 ) ;

    return 0 ;
}

int haskell_init( void* arg ) {
	(void) arg ;

    radiator_t* ths = &haskell_radiator.super ;
    init_radiator( ths, haskell_radiate_file ) ;
    char* ghc_pkg = ths->query( ths, "radiation_haskell_ghcpkg", "ghc-pkg" ) ;
    haskell_radiator.package_database = CreateGhcDatabase( ghc_pkg ) ;
	callback = new HaskellMatchCallback( new HaskellRadiator(&haskell_radiator) ) ;

	try {
    	import_regex = PcreRegex::compile("import\\s+(?:qualified\\s+)?(\\S+)", PCRE_DOTALL) ;
    	function_regex = PcreRegex::compile("^\\s\\s(\\w+)\\s::", PCRE_DOTALL) ;
	} catch( PcreCompileError& err ) {
		ths->error( ths, err.error ) ;
		lprintf("Unable to compile regex.\n") ;
		return -1 ;
	} catch( ... ) {
		ths->error( ths, "Unable to compile regex.\n" ) ;
		lprintf("Unable to compile regex.\n") ;
		return -1 ;
	}

	lprintf("Initialized haskell module\n") ;
	delete[] ghc_pkg ;

	return RADIATION_OK ;
} ;




