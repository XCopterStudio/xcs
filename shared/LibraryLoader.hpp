/* 
 * File:   LibraryLoader.hpp
 * Author: michal
 *
 * Created on February 1, 2014, 12:09 AM
 */

#ifndef LIBRARYLOADER_HPP
#define	LIBRARYLOADER_HPP

#include <string>

#include "Multiton.hpp"

#ifndef USE_PRECOMPILED_HEADERS
#ifdef _WIN32
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
 * \todo Thread safety.
 */
class LibraryLoader : public Multiton<LibraryLoader, std::string> {
    friend class Multiton<LibraryLoader, std::string>;
public:
#ifdef _WIN32
    HINSTANCE;
    typedef HINSTANCE HandleType;
#else
    typedef void* HandleType;
#endif
    LibraryLoader(const std::string& libraryName);
    virtual ~LibraryLoader();

    bool isLoaded() const;
    void unload();
    void load();
    HandleType libHandle() const;
    std::string libraryName() const;

private:

    HandleType libHandle_;
    std::string libraryName_;

    std::string getFilename() const;

};

}

#endif	/* LIBRARYLOADER_HPP */

