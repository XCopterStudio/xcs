/* 
 * File:   UXci.cpp
 * Author: michal
 * 
 * Created on January 31, 2014, 11:07 PM
 */

#include "xcs/xci/XCI.hpp"


#include "UXci.hpp"

#include <iostream>

#include "xcs/LibraryLoader.hpp"
#include "xcs/SymbolLoader.hpp"
#include "xcs/xci/data_receiver.hpp"


using namespace xcs::urbi;
using namespace xcs::xci::parrot;
using namespace std;

UXci::UXci(const std::string& name) : ::urbi::UObject(name) {
    UBindFunction(UXci, init);
    UBindFunction(UXci, xciInit);
    UBindFunction(UXci, command);
    UBindFunction(UXci, flyParam);
}

void UXci::init(const std::string& driver) {
    //    LibraryLoader & libraryLoader = LibraryLoader::getInstance(driver);
    //    libraryLoader.load(driver);
    //    SymbolLoader<XciFactoryFunction*> symbolLoader(libraryLoader);
    //    XciFactoryFunction* factory = symbolLoader.loadSymbol("CreateXci");
    //    xci_ = factory();
    DataReceiver DataReceiver;
    xci_ = new XCI_Parrot(DataReceiver);
    cerr << "Inited " << endl;
}

void UXci::xciInit() {
    xci_->init();
}
void UXci::command(const std::string& command) {
    xci_->command(command);
}

void UXci::flyParam(double roll, double pitch, double yaw, double gaz) {
    xci_->flyParam(roll, pitch, yaw, gaz);
}

UXci::~UXci() {

}

UStart(UXci);


