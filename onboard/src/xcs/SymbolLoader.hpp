/* 
 * File:   SymbolLoader.hpp
 * Author: michal
 *
 * Created on February 1, 2014, 12:18 AM
 */

#ifndef SYMBOLLOADER_HPP
#define	SYMBOLLOADER_HPP

#include <string>
#include "LibraryLoader.hpp"
#include "xcs/Exception.hpp"

namespace xcs {

template<typename SymbolType>
class SymbolLoader {
public:

    /*!
     * \param libraryLoader Expects loaded library loader.
     */
    SymbolLoader(const LibraryLoader& libraryLoader) : libraryLoader_(libraryLoader) {
    }

    virtual ~SymbolLoader() {
    }

    SymbolType loadSymbol(const std::string &symbolName) {
#ifdef _WIN32  
        // load symbol
        void* fn_handle = GetProcAddress(libraryLoader_.libHandle(), symbolName.c_str());
        if (fn_handle == NULL) {
            throw Exception("Cannot load symbol '" + symbolName + "' from '" + libraryLoader_.libraryName() + "'.");
        }
        return fn_handle;
#else
        // reset errors
        dlerror();
        // load symbol
        void* fn_handle = dlsym(libraryLoader_.libHandle(), symbolName.c_str());
        const char* dlsym_error = dlerror();
        if (dlsym_error) {
            throw Exception("Cannot load symbol '" + symbolName + "' from '" + libraryLoader_.libraryName() + "'.");
        }
        return reinterpret_cast<SymbolType> (fn_handle);
#endif
    }


private:
    const LibraryLoader& libraryLoader_;

};
}
#endif	/* SYMBOLLOADER_HPP */

