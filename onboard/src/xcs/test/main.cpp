#include <iostream>
#include <xcs/library_loader.hpp>
#include <xcs/symbol_loader.hpp>
#include <xcs/xci/xci.hpp>

using namespace xcs;

int main(int argc, char** argv) {
    LibraryLoader &ll = LibraryLoader::getInstance("xci_dodo");
    ll.load("./build/lib/libxci_dodo.so");
    SymbolLoader<xcs::xci::XciFactoryFunction *> sl(ll);
    auto sym = sl.loadSymbol("CreateXci");
    
    return 0;
}
