#ifndef HIGHLIGHTERC_HPP_
#define HIGHLIGHTERC_HPP_

#include "Highlighter.hpp"
#include <set>
#include <algorithm>

class CTokenIterator {
    public:
        inline CTokenIterator( const std::string& str ) :
              m_cur_str(""), m_parse_str( str ), m_offset(0)
            , m_in_quotes(false), m_body(0), m_space(false),m_in_sing_quotes(false) {
            this->operator++();
        }

        virtual void operator++();

        virtual inline const std::string& operator*() const
            { return m_cur_str ; }
        
        virtual inline std::string& get() 
            { return m_cur_str ; }

        virtual inline bool hasNext() const
            { return !(m_offset == m_parse_str.length() && m_cur_str.length() == 0); }
    private:

        inline void p_UpdateForChar( char ch ) {
            switch ( ch ) {
            case '"':
                if ( ! m_in_sing_quotes )
                    m_in_quotes = ! m_in_quotes;
                break;
            case '\'':
                if ( ! m_in_quotes )
                    m_in_sing_quotes = ! m_in_sing_quotes;
                break;
            case '{':
                if( ! ( m_in_quotes || m_in_sing_quotes )  )
                    m_body ++;
                break;
            case '}':
                if( ! (m_in_quotes || m_in_sing_quotes) )
                    m_body --;
                break;
            case '#':
                if( ! (m_in_quotes || m_in_sing_quotes) )
                    m_in_hash = true;
                break ;
            }
        }

        std::string   m_cur_str;
        const std::string&   m_parse_str;
        size_t        m_offset;

        bool          m_in_quotes;
        int           m_body;
        bool          m_space; // used for consolidating whitespace
        bool          m_in_hash;
        bool          m_in_sing_quotes ;
};

class CHighlighter : public Highlighter {
public:
    inline CHighlighter() {
            m_highlight_groups.push_back( "Type" );
            m_highlight_groups.push_back( "Constant" );
            m_highlight_groups.push_back( "Function" );
            m_blacklist.insert("auto");
            m_blacklist.insert("break");
            m_blacklist.insert("case");
            m_blacklist.insert("char");
            m_blacklist.insert("const");
            m_blacklist.insert("continue");
            m_blacklist.insert("default");
            m_blacklist.insert("do");
            m_blacklist.insert("double");
            m_blacklist.insert("else");
            m_blacklist.insert("enum");
            m_blacklist.insert("extern");
            m_blacklist.insert("float");
            m_blacklist.insert("for");
            m_blacklist.insert("goto");
            m_blacklist.insert("if");
            m_blacklist.insert("int");
            m_blacklist.insert("long");
            m_blacklist.insert("register");
            m_blacklist.insert("return");
            m_blacklist.insert("short");
            m_blacklist.insert("signed");
            m_blacklist.insert("sizeof");
            m_blacklist.insert("static");
            m_blacklist.insert("struct");
            m_blacklist.insert("switch");
            m_blacklist.insert("typedef");
            m_blacklist.insert("union");
            m_blacklist.insert("unsigned");
            m_blacklist.insert("void");
            m_blacklist.insert("volatile");
            m_blacklist.insert("while");
        }
    /* Runs the highlight on a string which was
     * read in from a file */
    /* This function may be run in parallel, so protection
     * sould be used */
    virtual void runHighlight( const std::string& str ) ;

    /* Run highlighting on a specific file */
    virtual void highlightFile( const std::string& filename );

    static inline std::string &ltrim(std::string &s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
            return s;
    }

    static inline std::string &rtrim(std::string &s) {
            s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
            return s;
    }
    /* Return the groups that are used to high
     * light */
    virtual inline const std::vector<std::string>& getHighlightKeywordGroups() const 
        { return m_highlight_groups; }

    virtual inline const std::vector<std::string>& getHighlightMatchGroups() const 
        { return m_highlight_match_groups; }

    /* Returns keywords for the passed highlight group */
    virtual const std::set<std::string>* getHighlightsForGroup( const std::string& group ) ;
private:
    virtual void p_ParseToken( std::string& token ) ;

    inline void p_AddTo( const std::string& a_str, std::set<std::string>& vec ) {
        std::string str = a_str ;
        rtrim( ltrim( str ) ) ;
        if( str.length() > 0 && m_blacklist.find( str ) == m_blacklist.end() )
            vec.insert( str );
    }

    int p_ParseEnumConstants( const std::string& str, std::set<std::string>& into );
    int p_TryParseFunction( const std::string& str, std::set<std::string>& into );

    std::set<std::string> m_types;
    std::set<std::string> m_constants;
    std::set<std::string> m_functions;
    std::vector<std::string> m_highlight_groups;
    std::vector<std::string> m_highlight_match_groups;
    std::set<std::string> m_blacklist;
};

#endif
