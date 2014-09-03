/* 
 * File:   LibraryLoader.hpp
 * Author: michal
 *
 * Created on February 1, 2014, 12:09 AM
 */

#ifndef LIBRARYLOADER_HPP
#define	LIBRARYLOADER_HPP

#include <string>
#include <mutex> 

#include "multiton.hpp"
#include <xcs/xcs_export.h>

#ifndef USE_PRECOMPILED_HEADERS
#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <direct.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <dlfcn.h>
#endif
#endif


namespace xcs {

/*!
 * Class implements multiton pattern -- one instance for each loaded library.
 */
class XCS_EXPORT LibraryLoader : public Multiton<LibraryLoader, std::string> {
    friend class Multiton<LibraryLoader, std::string>;
public:
#ifdef _WIN32
    typedef HMODULE HandleType;
#else
    typedef void* HandleType;
#endif
    LibraryLoader(const std::string& libraryName);
    virtual ~LibraryLoader();

    bool isLoaded() const;

    /*!
     * Release the library from memory.
     */
    void unload();

    /*!
     * Loads the library by the instance name or the name provided.
     * \note If loaded, does nothing.
     * \param filename Full filename (relative to LD_LIBRARY_PATH or absolute) with the library.
     */
    void load(const std::string& filename = "");
        
    HandleType libHandle() const;
    
    std::string libraryName() const;

private:
    HandleType libHandle_;

    std::string libraryName_;

    std::mutex mutex_;

    std::string getFilename() const;

};

}

#endif	/* LIBRARYLOADER_HPP */

