/* ***** BEGIN LICENSE BLOCK *****
;; Roadsend PHP Compiler
;;
;; Copyright (c) 2009 Shannon Weyrick <weyrick@roadsend.com>
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

#include "rphp/analysis/pBaseTransformer.h"

#include <iostream>
#include <unicode/ustream.h>

namespace rphp { namespace AST {

pBaseTransformer::dispatchFunction pBaseTransformer::preDispatchTable_[] = {

#define STMT(CLASS, PARENT) reinterpret_cast<dispatchFunction>( &pBaseTransformer::transform_pre_##CLASS ),
#include "rphp/analysis/astNodes.def"

};

pBaseTransformer::dispatchFunction pBaseTransformer::postDispatchTable_[] = {

#define STMT(CLASS, PARENT) reinterpret_cast<dispatchFunction>( &pBaseTransformer::transform_post_##CLASS ),
#include "rphp/analysis/astNodes.def"

};

stmt* pBaseTransformer::transform(stmt* s) {

    stmt* rNode;

    // PRE
    rNode = (this->*preDispatchTable_[s->getKind()])(s);
    if (rNode != s) {
        s->destroy(context_);
        s = rNode;
    }

    // CHILDREN
    stmt* child(0);
    for (stmt::child_iterator i = s->child_begin(), e = s->child_end(); i != e; ) {
      if ( (child = *i++) ) {
          rNode = transform(child);
          if (rNode != child) {
              (*i)->destroy(context_);
              *i = rNode;
          }
      }
    }

    // POST
    rNode = (this->*postDispatchTable_[s->getKind()])(s);
    if (rNode != s) {
        s->destroy(context_);
        s = rNode;
    }

    return s;

}


} } // namespace
