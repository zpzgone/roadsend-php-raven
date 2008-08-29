/* ***** BEGIN LICENSE BLOCK *****
 * Roadsend PHP Compiler Runtime Libraries
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 * ***** END LICENSE BLOCK ***** */

#ifndef PEXTENSIONBASE_H_
#define PEXTENSIONBASE_H_

#include <boost/bind.hpp>
#include <string>
#include "pVar.h"

namespace rphp {

    class pRuntimeEngine;

    class pExtBase {

    protected:
        std::string extName;
        pRuntimeEngine* runtime;

        void registerBuiltin(pUString name, pFunPointer1 f);

    public:
        pExtBase(pRuntimeEngine *r, std::string eName) : runtime(r), extName(eName) { }

        virtual void extensionStartup() = 0;
        virtual void extensionShutdown() = 0;

    };

}

#endif /* PEXTENSIONBASE_H_ */
