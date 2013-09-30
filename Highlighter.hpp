#ifndef HIGHLIGHTER_H_
#define HIGHLIGHTER_H_

#include <cstdlib>
#include <vector>
#include <iostream>

class Highlighter {
public:
    virtual void highlightFile ( const std::string& filename ) = 0;
    /* Return the groups that are used to high
     * light */
    virtual const std::vector<std::string>& getHighlightGroups() const = 0;

    /* Returns keywords for the passed highlight group */
    virtual const std::vector<std::string>* getHighlightsForGroup( const std::string& group ) = 0;
} ;

#endif
