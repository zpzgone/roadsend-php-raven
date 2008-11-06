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

%include {   

#include <iostream>
#include <string>
#include <cctype> // for toupper
#include <algorithm>
#include <unicode/unistr.h>

#include "pAST.h"
#include "pModule.h"
#include "pLangLexerDef.h"

using namespace rphp;

}  

%name rphpParse
%token_type {lexer::tokenPairType*}
%default_type {lexer::tokenPairType*}
%extra_argument {pModule* pMod}

// these don't exist in the parser, just in the lexer
%type T_WHITESPACE {int}
%type T_OPEN_TAG {int}
%type T_CLOSE_TAG {int}

// tokens
%type T_INLINE_HTML {int}
%type T_IF {int}
%type T_WHILE {int}
%type T_ELSE {int}
%type T_ASSIGN {int}

// xxx temp, these are real nodes
%type T_IDENTIFIER {int}

%syntax_error {  
  std::cerr << "Syntax error, unexpected: '" << *TOKEN << "'" << std::endl;
}   
%stack_overflow {  
  std::cerr << "Parser stack overflow" << std::endl;
}   

%type module {int}
module ::= statement_list.

%type statement_list {int}
statement_list ::= .
statement_list ::= statement_list statement(A). { pMod->getAST().push_back(A); }

/******** STATEMENTS ********/
%type statement {AST::stmt*}
statement(A) ::= inlineHTML(B). { A = B; }
statement(A) ::= echo(B) T_SEMI. { A = B; }
statement(A) ::= expr(B) T_SEMI. { A = B; }
statement ::= T_SEMI.

// echo
%type echo {AST::echoStmt*}
echo(A) ::= T_ECHO expr(B). { A = new AST::echoStmt(B); }

// inline html
%type inlineHTML {AST::inlineHtml*}
inlineHTML(A) ::= T_INLINE_HTML(B).
{
    A = new AST::inlineHtml(std::string((*B).begin(), (*B).end()));
}

/****** EXPRESSIONS *********/
%type expr {AST::expr*}
expr(A) ::= literal(B). { A = B; }
expr(A) ::= assignment(B). { A = B; }
expr(A) ::= lval(B). { A = B; }

/** LITERALS **/
%type literal {AST::literalExpr*}

// literal string, double quotes
literal(A) ::= T_CONSTANT_ENCAPSED_STRING(B).
{
  // binary specifier?
  bool doUnicode = pMod->defaultUnicode();
  std::string::iterator start;
  if ( *(*B).begin() == 'b') {
      // binary
      doUnicode = false;
      start = ++(*B).begin();
  }
  else if ( *(*B).begin() == 'u') {
      // unicode
      doUnicode = true;
      start = ++(*B).begin();
  }
  else {
      // according to module default
      start = (*B).begin();
  }
  // substring out the quotes, special case for empty string
  if (++start == (*B).end()) {
    if (doUnicode) {
        A = new AST::literalString(UnicodeString());
    } else {
        A = new AST::literalString(std::string());
    }
  }
  else {
    if (doUnicode) {
        std::string tmp(start, --(*B).end());
        A = new AST::literalString(UnicodeString(tmp.data(), tmp.length(), pMod->encoding()));
    } else {
        A = new AST::literalString(std::string(start, --(*B).end()));
    }
  }
}

// literal integers (decimal)
literal(A) ::= T_LNUMBER(B). { A = new AST::literalInt(std::string((*B).begin(), (*B).end())); }

// literal integers (float)
literal(A) ::= T_DNUMBER(B). { A = new AST::literalFloat(std::string((*B).begin(), (*B).end())); }

// literal identifier: null, true, false or string
literal(A) ::= T_IDENTIFIER(B).
{
    // case insensitive checks
    std::string ciTmp((*B).begin(), (*B).end());
    transform(ciTmp.begin(), ciTmp.end(), ciTmp.begin(), toupper);
    if (ciTmp == "NULL") {
        A = new AST::literalNull();
    }
    else if (ciTmp == "TRUE") {
        A = new AST::literalBool(true);
    }
    else if (ciTmp == "FALSE") {
        A = new AST::literalBool(false);
    }
    else {
        // default to normal string
        A = new AST::literalString(std::string((*B).begin(), (*B).end()));
    }
}

/** ASSIGNMENT **/
%type assignment {AST::assignment*}
assignment(A) ::= lval(L) T_ASSIGN expr(R).
{
    A = new AST::assignment(L, R);
}

/** LVALS **/
%type lval {AST::expr*}
lval(A) ::= variable_lVal(B). { A = B; }

%type variable_lVal {AST::var*}
variable_lVal(A) ::= T_VARIABLE(B).
{
    // strip $. TODO: unicode identifiers?
    A = new AST::var(std::string(++(*B).begin(), (*B).end()));
}

