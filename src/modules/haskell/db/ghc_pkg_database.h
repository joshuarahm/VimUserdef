#ifndef GHC_PKG_DATABASE_H_
#define GHC_PKG_DATABASE_H_

/*
 * Author: jrahm
 * created: 2014/01/24
 * ghc_pkg_database.h: <description>
 */

#ifdef __cplusplus
extern "C" {
#endif
#include "radiation.h"
#ifdef __cplusplus
}
#endif

#ifndef __cplusplus
struct ghc_database ;
typedef struct ghc_database GhcPackageDatabase ;
#else
class GhcPackageDatabase ;
#endif

typedef struct HASKELL_RADIATOR {
    EXTENDS_RADIATOR( struct HASKELL_RADIATOR ) ;
    GhcPackageDatabase* package_database ;
} haskell_radiator_t ;

#ifdef __cplusplus

#include "radiation_cpp.hpp"

#include <map>
#include <string>
#include <vector>

class GhcPackage {
private:
    typedef std::map<
        std::string,
        std::vector<std::string>
    > ListMap ;
    std::string name ;

    ListMap string_map ;

public:
    typedef ListMap::iterator iterator ;

    inline std::vector<std::string>& getVector( const std::string& str ) {
        return string_map[str];
    }

    inline void setName( std::string& name ) {
        this->name = name ;
    }

    inline const std::string& getName() const {
        return name ;
    }

    inline iterator begin() {
        return string_map.begin() ;
    }

    inline iterator end() {
        return string_map.end() ;
    }
} ;

class GhcPackageDatabase {
private:
    typedef std::map<std::string,GhcPackage*> PackageMap ;
    /* Map of import packages to Packages */
    PackageMap packages ;

public:
    typedef PackageMap::iterator iterator ;

    /* add a package to the database */
    inline void addPackage( const std::string& name, GhcPackage* package ) {
        packages[name] = package ;
    }

    GhcPackage* getPackage( const std::string& name ) {
        return packages[name] ;
    }

    inline iterator begin() {
        return packages.begin() ;
    }

    inline iterator end() {
        return packages.end() ;
    }
} ;


extern "C" {
#endif

GhcPackageDatabase* CreateGhcDatabase( const char* ghc_pkg_binary );

int haskell_radiate_file(
    struct RADIATOR* radiator,
    const char* filename,
    const char* filetype,
    const char* env ) ;

#ifdef __cplusplus
}
#endif

#endif /* GHC_PKG_DATABASE_H_ */
