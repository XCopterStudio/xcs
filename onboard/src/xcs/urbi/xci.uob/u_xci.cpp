/* 
 * File:   UXci.cpp
 * Author: michal
 * 
 * Created on January 31, 2014, 11:07 PM
 */

#include "u_xci.hpp"

#include <iostream>

#include <xcs/library_loader.hpp>
#include <xcs/symbol_loader.hpp>


using namespace xcs::urbi;
using namespace xcs::xci;
using namespace std;

UXci::UXci(const std::string& name) :
  ::urbi::UObject(name),
  inited_(false),
  roll_(0),
  pitch_(0),
  yaw_(0),
  gaz_(0),
  flyParamActive_(false),
  flyParamPersistence_(0) {
    UBindFunction(UXci, init);
    UBindFunction(UXci, xciInit);
    UBindFunction(UXci, doCommand);
    UBindFunction(UXci, flyParam);
    UBindFunction(UXci, dumpConfiguration);
    UBindFunction(UXci, setConfiguration);

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
    LibraryLoader & libraryLoader = LibraryLoader::getInstance(driver);
    libraryLoader.load();
    SymbolLoader<XciFactoryFunction*> symbolLoader(libraryLoader);
    XciFactoryFunction* factory = symbolLoader.loadSymbol("CreateXci");
    xci_ = factory(dataReceiver_);
    ::urbi::UBinary bin;
    ::urbi::UImage img;
    bin.type;

    initOutputs();
}

void UXci::xciInit() {
    if (!inited_) {
        xci_->init();
        setFlyParamPersistence(stoi(xci_->parameter(xci::XCI_PARAM_FP_PERSISTENCE)));
        flyParamThread_ = move(thread(&UXci::keepFlyParam, this));
        inited_ = true; // TODO check this variable in all commands to the drone
    } else {
        cerr << "[UXci] already called init." << endl; //TODO general way for runtime warnings (in Urbi)
    }
}

void UXci::doCommand(const std::string& command) {
    setFlyParamActive(false);
    xci_->command(command);
}

void UXci::flyParam(double roll, double pitch, double yaw, double gaz) {
    roll_ = roll;
    pitch_ = pitch;
    yaw_ = yaw;
    gaz_ = gaz;
    sendFlyParam();
    setFlyParamActive();
}

void UXci::dumpConfiguration() {
    xci_->configuration();
}

void UXci::setConfiguration(const std::string& key, const std::string& value) {
    xci_->configuration(key, value);
}

void UXci::onChangeRoll(double roll) {
    roll_ = roll;
    sendFlyParam();
    setFlyParamActive();
}

void UXci::onChangePitch(double pitch) {
    pitch_ = pitch;
    sendFlyParam();
    setFlyParamActive();
}

void UXci::onChangeYaw(double yaw) {
    yaw_ = yaw;
    sendFlyParam();
    setFlyParamActive();
}

void UXci::onChangeGaz(double gaz) {
    gaz_ = gaz;
    sendFlyParam();
    setFlyParamActive();
}

void UXci::initOutputs() {
    for (auto sensor : xci_->sensorList()) {
        ::urbi::UVar *uvar = new ::urbi::UVar();
        UBindVarRename(UXci, *uvar, sensor.name);
        cout << "Registered sensor " << sensor.name << endl;
        dataReceiver_.registerOutput(sensor.name, uvar);
    }
}

void UXci::setFlyParamPersistence(unsigned int value) {
    flyParamPersistence_ = value;
    setFlyParamActive(flyParamActive_); // notifies
}

void UXci::setFlyParamActive(bool value) {
    if (!flyParamThread_.joinable()) { // thread haven't started yet
        flyParamActive_ = value;
        return;
    } else {
        lock_guard<mutex> lock(flyParamMtx_);
        flyParamActive_ = value;
    }
    flyParamCond_.notify_all();
}

void UXci::keepFlyParam() {
    unique_lock<mutex> lock(flyParamMtx_, defer_lock_t());

    while (1) {
        lock.lock();
        flyParamCond_.wait(lock, [this] {
            return flyParamActive_ && flyParamPersistence_ > 0;
        });

        unsigned int localPersistence(flyParamPersistence_);
        lock.unlock();

        sendFlyParam();
        this_thread::sleep_for(chrono::milliseconds(localPersistence));
    }
}

void UXci::sendFlyParam() {
    xci_->flyParam(roll_, pitch_, yaw_, gaz_);
}

UXci::~UXci() {

}

UStart(UXci);


