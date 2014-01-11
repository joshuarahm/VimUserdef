#ifndef HIGHLIGHTER_H_
#define HIGHLIGHTER_H_

#include <cstdlib>
#include <vector>
#include <iostream>
#include <set>

class Highlighter {
public:
    virtual void highlightFile ( const std::string& filename ) = 0;
    /* Return the groups that are used to high
     * light */
    virtual const std::vector<std::string>& getHighlightKeywordGroups() const = 0;
    virtual const std::vector<std::string>& getHighlightMatchGroups() const = 0;

    /* Returns keywords for the passed highlight group */
    virtual const std::set<std::string>* getHighlightsForGroup( const std::string& group ) = 0;
} ;

#endif
