#ifndef HIGHLIGHTERJAVA_HPP_
#define HIGHLIGHTERJAVA_HPP_

/*
 * Author: jrahm
 * created: 2013/11/16
 * HighlighterJava.hpp: <description>
 */

#include "Highlighter.hpp"

#include <vector>

class JavaHighlighter : public Highlighter {
public:

    inline JavaHighlighter() {
        m_highlight_groups.push_back( "Class" );
        m_highlight_groups.push_back( "Function" );

        m_highlight_match_groups.push_back( "Member" );
        m_highlight_match_groups.push_back( "Package" );
    }

    virtual inline const std::vector<std::string>& getHighlightKeywordGroups() const {
        return m_highlight_groups;
    }

    virtual inline const std::vector<std::string>& getHighlightMatchGroups() const {
        return m_highlight_match_groups;
    }

    virtual const std::set<std::string>* getHighlightsForGroup( const std::string& group ) {
        if( group == "Function" ) {
            return & m_methods;
        } else if ( group == "Class" ) {
            return & m_classes;
        } else if ( group == "Package" ) {
            return &m_packages;
        } else if ( group == "Member" ) {
            return &m_members;
        }

        return NULL;
    }

    virtual void highlightFile( const std::string& filename ) ;

private:
    std::vector< std::string > m_highlight_groups;
    std::vector< std::string > m_highlight_match_groups;

    std::set< std::string > m_methods;
    std::set< std::string > m_classes;
    std::set< std::string > m_packages;
    std::set< std::string > m_members;
};

#endif /* HIGHLIGHTERJAVA_HPP_ */
