#pragma once

#include <string>
#include "xcs/xci/XCI.hpp"

#ifndef USE_PRECOMPILED_HEADERS
#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <dlfcn.h>
#endif
#endif

using namespace std;

class XCILoader {
public:
#ifdef _WIN32
    HINSTANCE lib_handle;
#else
    void* lib_handle;
#endif

    XCILoader(void);
    virtual ~XCILoader(void);

    xcs::xci::XCI* Load();
    void LoadLib(const std::string & filepath);
    void* LoadLibrarySymbol(const std::string & symbol);
};

