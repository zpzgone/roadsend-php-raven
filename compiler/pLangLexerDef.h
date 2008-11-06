/* ***** BEGIN LICENSE BLOCK *****
;; Roadsend PHP Compiler
;;
;; Copyright (c) 2008 Shannon Weyrick <weyrick@roadsend.com>
;;
;; This program is free software; you can redistribute it and/or
;; modify it under the terms of the GNU General Public License
;; as published by the Free Software Foundation; either version 2
;; of the License, or (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, write to the Free Software
;; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
   ***** END LICENSE BLOCK *****
*/

#ifndef RPHP_PLANGLEXERDEF_H_
#define RPHP_PLANGLEXERDEF_H_

//#define BOOST_SPIRIT_LEXERTL_DEBUG
//#define BOOST_SPIRIT_DEBUG

#include <boost/range/iterator_range.hpp>
#include <boost/spirit/include/lex_lexer_lexertl.hpp>
#include <string>

#include "rphp_grammar.h"

namespace rphp { namespace lexer {

using namespace boost::spirit::lex;

template <typename Lexer>
struct rphpLangTokens : lexer_def<Lexer>
{

    template <typename Self>
    void def (Self& self)
    {


        // HTML (default) state tokens
        self
            = token_def<>("<\\?|<\\?PHP|<\\?php", T_OPEN_TAG)
            | token_def<>(".+|\\n+", T_INLINE_HTML)
            ;

        // PHP state tokens
        self("PHP")
            = token_def<>('(') | ')' | '{' | '}'
            | token_def<>("=", T_ASSIGN)
            | token_def<>(";", T_SEMI)
            | token_def<>("\\?>", T_CLOSE_TAG)
            | token_def<>("if", T_IF)
            | token_def<>("while", T_WHILE)
            | token_def<>("else", T_ELSE)
            | token_def<>("echo", T_ECHO)
            | token_def<>("[a-zA-Z_][a-zA-Z0-9_]*", T_IDENTIFIER)
            | token_def<>("\\$[a-zA-Z_][a-zA-Z0-9_]*", T_VARIABLE)
            | token_def<>("[0-9]+", T_LNUMBER)
            | token_def<>("([0-9]*[\\.][0-9]+)|([0-9]+[\\.][0-9]*)", T_DNUMBER)
            | token_def<>("[ \\t\\n]+", T_WHITESPACE)
            | token_def<>("\\/\\*[^*]*\\*+([^/*][^*]*\\*+)*\\/" /* multiline comment */)
            | token_def<>("\\/\\/.*$" /* single line comment */)
            | token_def<>("[bu]*[\"][^\"]*[\"]", T_CONSTANT_ENCAPSED_STRING)
            ;

    }

};

// iterator type used to expose the underlying input stream
// TODO: parameterize this for wstring
typedef std::string::iterator sourceIteratorType;

// token type. this should list all types used as return values in the
// token lexer definition
typedef lexertl_token<sourceIteratorType> languageTokenType;

// iterator pair used as value for matched tokens
typedef boost::iterator_range<languageTokenType::iterator_type> tokenPairType;

// Here we use the lexertl based lexer engine.
typedef lexertl_lexer<languageTokenType> lexerEngineType;

// specialize the tokens with this engine
typedef rphpLangTokens<lexerEngineType> pLangTokens;

// actual lexer type
typedef boost::spirit::lex::lexer<pLangTokens> pLangLexer;

// this is the iterator type exposed by the lexer, which dereferences to
// a token
typedef boost::spirit::lex::lexer<pLangTokens>::iterator_type tokIteratorType;

} } // namespace

#endif /* RPHP_PLANGLEXERDEF_H_ */
