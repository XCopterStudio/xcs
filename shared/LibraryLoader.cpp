/* 
 * File:   LibraryLoader.cpp
 * Author: michal
 * 
 * Created on February 1, 2014, 12:18 AM
 */

#include <map>
#include <iostream>
#include "LibraryLoader.hpp"
#include "xcs/Exception.hpp"

using namespace xcs;
using namespace std;

LibraryLoader::LibraryLoader(const std::string& libraryName) : libHandle_(0), libraryName_(libraryName) {
    load();
}

LibraryLoader::~LibraryLoader() {
    unload();
}

LibraryLoader::HandleType LibraryLoader::libHandle() const {
    return libHandle_;
}

std::string LibraryLoader::libraryName() const {
    return libraryName_;
}


bool LibraryLoader::isLoaded() const {
    return libHandle_ != 0;
}

void LibraryLoader::load() {
    string filename = getFilename();
#ifdef _WIN32
    libHandle_ = LoadLibrary(TEXT(filename.c_str()));
#else
    dlerror(); // reset errors
    libHandle_ = dlopen(filename.c_str(), RTLD_NOW);
#endif

    if (!libHandle_) {
        throw Exception("Cannot load library '" + libraryName_ + "', searched in '" + filename + "'.");
    }
}

void LibraryLoader::unload() {
    if (!isLoaded()) {
        return;
    }
#ifdef _WIN32
    FreeLibrary(libHandle_);
#else
    dlclose(libHandle_);
#endif    
}

std::string LibraryLoader::getFilename() const {
#ifdef _WIN32
    return libraryName_ + ".dll";
#else
    return "./lib" + libraryName_ + ".so"; //TODO really search always relative to working directory?
#endif    
}








