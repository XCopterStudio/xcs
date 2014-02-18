/* 
 * File:   UXci.cpp
 * Author: michal
 * 
 * Created on January 31, 2014, 11:07 PM
 */

#include "UXci.hpp"
//#include "xcs/xci/parrot/XCI_Parrot.hpp"
#include "xcs/xci/dodo/XciDodo.hpp"

#include <iostream>

#include "xcs/LibraryLoader.hpp"
#include "xcs/SymbolLoader.hpp"


using namespace xcs::urbi;
using namespace xcs::xci;
using namespace std;

// TODO should be removed when dynamic loading will be solved
//using namespace xcs::xci::parrot;
using namespace xcs::xci::dodo;

UXci::UXci(const std::string& name) :
  ::urbi::UObject(name),
  inited_(false),
  roll_(0),
  pitch_(0),
  yaw_(0),
  gaz_(0),
  flyParamPersistence_(0) {
    UBindFunction(UXci, init);
    UBindFunction(UXci, xciInit);
    UBindFunction(UXci, doCommand);
    UBindFunction(UXci, flyParam);

    UBindVar(UXci, flyParamPersistence);
    UNotifyChange(flyParamPersistence, &UXci::setFlyParamPersistence);

    UBindVar(UXci, roll);
    UNotifyChange(roll, &UXci::onChangeRoll);
    UBindVar(UXci, pitch);
    UNotifyChange(pitch, &UXci::onChangePitch);
    UBindVar(UXci, yaw);
    UNotifyChange(yaw, &UXci::onChangeYaw);
    UBindVar(UXci, gaz);
    UNotifyChange(gaz, &UXci::onChangeGaz);

    UBindVar(UXci, command);
    UNotifyChange(command, &UXci::doCommand);

}

void UXci::init(const std::string& driver) {
    // TODO resolve dynamic loading & linking
    //    LibraryLoader & libraryLoader = LibraryLoader::getInstance(driver);
    //    libraryLoader.load(driver);
    //    SymbolLoader<XciFactoryFunction*> symbolLoader(libraryLoader);
    //    XciFactoryFunction* factory = symbolLoader.loadSymbol("CreateXci");
    //    xci_ = factory();

    xci_ = new XciDodo(dataReceiver_);
    initOutputs();
}

void UXci::xciInit() {
    if (!inited_) {
        xci_->init();
        flyParamThread_ = move(thread(&UXci::keepFlyParam, this));
    } else {
        cerr << "[UXci] already called init." << endl; //TODO general way for runtime warnings
    }
}

void UXci::doCommand(const std::string& command) {
    xci_->command(command);
}

void UXci::flyParam(double roll, double pitch, double yaw, double gaz) {
    xci_->flyParam(roll, pitch, yaw, gaz);
}

void UXci::setFlyParamPersistence(unsigned int value) {
    unique_lock<mutex> lock(flyParamMtx_);
    flyParamPersistence_ = value;
    flyParamCond_.notify_all();
}

void UXci::onChangeRoll(double roll) {
    roll_ = roll;
}

void UXci::onChangePitch(double pitch) {
    pitch_ = pitch;
}

void UXci::onChangeYaw(double yaw) {
    yaw_ = yaw;
}

void UXci::onChangeGaz(double gaz) {
    gaz_ = gaz;
}

void UXci::initOutputs() {
    for (auto sensor : xci_->sensorList()) {
        ::urbi::UVar *uvar = new ::urbi::UVar();
        UBindVarRename(UXci, *uvar, sensor.name);
        cout << "Registered sensor " << sensor.name << endl;
        dataReceiver_.registerOutput(sensor.name, uvar);
    }
}

void UXci::keepFlyParam() {
    while (1) {
        unique_lock<mutex> lock(flyParamMtx_);
        flyParamCond_.wait(lock, [this] {
            return flyParamPersistence_ > 0;
        });
        flyParam(roll_, pitch_, yaw_, gaz_);
        this_thread::sleep_for(chrono::milliseconds(flyParamPersistence_));
    }
}

UXci::~UXci() {

}

UStart(UXci);


