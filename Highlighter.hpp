#ifndef HIGHLIGHTER_H_
#define HIGHLIGHTER_H_

#include <cstdlib>
#include <vector>
#include <iostream>

class Highlighter {
public:
    class TokenIterator {
    public:
        /* The opeator that
         * parses the next
         * chunk of the string
         */
        virtual void operator++() = 0;

        /*
         * The operator that returns
         * the current string */
        virtual const std::string& operator*() = 0;
    } ;

    /* Run a parse algorithm to fill a vector of
     * tokens */
    virtual TokenIterator* getTokenIterator( const std::string& str ) = 0;

    /* Runs the highlight on a string which was
     * read in from a file */
    /* This function may be run in parallel, so protection
     * sould be used */
    virtual void runHighlight( const std::string& str ) = 0;

    /* Return the groups that are used to high
     * light */
    virtual const std::vector<std::string>& getHighlightGroups() const = 0;

    /* Returns keywords for the passed highlight group */
    virtual const std::vector<std::string>& getHighlightsForGroup( const std::string& group ) = 0;
} ;

#endif
