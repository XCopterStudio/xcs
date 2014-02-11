#include "XCILoader.hpp"
#include <iostream>

using namespace xcs::xci;

XCILoader::XCILoader(void) {
}

XCILoader::~XCILoader(void) {
}
typedef XCI* factoryLib();

XCI* XCILoader::Load() {
#ifdef _WIN32
    const std::string path = "xci.dll";
#else
    const std::string path = "libxci.so";
#endif
    LoadLib(path);
    return ((factoryLib*) LoadLibrarySymbol("CreateXci"))();
}

void XCILoader::LoadLib(const std::string & filepath) {
#ifdef _WIN32
    lib_handle = LoadLibrary(TEXT(filepath.c_str()));
    if (!lib_handle) {
        std::cerr << "Cannot load library: " << TEXT(filepath.c_str()) << std::endl;
    }
#else
    // reset errors
    dlerror();
    lib_handle = dlopen(filepath.c_str(), RTLD_NOW);
    if (!lib_handle) {
        std::cerr << "Cannot load library: " << dlerror() << std::endl;
    }
#endif
}

void* XCILoader::LoadLibrarySymbol(const std::string & symbol) {
#ifdef _WIN32  
    // load symbol
    void* fn_handle = GetProcAddress(lib_handle, symbol.c_str());
    if (fn_handle == NULL) {
        std::cerr << "Cannot load symbol: " << std::endl;
    }
    return fn_handle;
#else
    // reset errors
    dlerror();
    // load symbol
    void* fn_handle = dlsym(lib_handle, symbol.c_str());
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Cannot load symbol: " << dlsym_error << std::endl;
    }
    return fn_handle;
#endif
}