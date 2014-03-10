/*
 * File:   XXci.cpp
 * Author: michal
 *
 * Created on January 31, 2014, 11:07 PM
 */

#include "x_xci.hpp"

#include <iostream>

#include <xcs/library_loader.hpp>
#include <xcs/symbol_loader.hpp>


using namespace xcs::nodes;
using namespace xcs::xci;
using namespace std;

XXci::XXci(const std::string& name) :
    xcs::nodes::XObject(name),
    flyParamPersistence("FLY_PARAM_PERSISTENCE"),
    roll("ROLL"),
    pitch("PITCH"),
    yaw("YAW"),
    gaz("GAZ"),
    command("COMMAND"),
    inited_(false),
    roll_(0),
    pitch_(0),
    yaw_(0),
    gaz_(0),
    flyParamActive_(false),
    flyParamPersistence_(0) {
    XBindFunction(XXci, init);
    XBindFunction(XXci, xciInit);
    XBindFunction(XXci, doCommand);
    XBindFunction(XXci, flyParam);
    XBindFunction(XXci, dumpConfiguration);
    XBindFunction(XXci, setConfiguration);

    XBindVarF(flyParamPersistence, &XXci::setFlyParamPersistence);

    XBindVarF(roll, &XXci::onChangeRoll);
    XBindVarF(pitch, &XXci::onChangePitch);
    XBindVarF(yaw, &XXci::onChangeYaw);
    XBindVarF(gaz, &XXci::onChangeGaz);

    XBindVarF(command, &XXci::doCommand);
}

void XXci::init(const std::string& driver) {
    LibraryLoader& libraryLoader = LibraryLoader::getInstance(driver);
    libraryLoader.load();
    SymbolLoader<XciFactoryFunction*> symbolLoader(libraryLoader);
    XciFactoryFunction* factory = symbolLoader.loadSymbol("CreateXci");
    xci_ = factory(dataReceiver_);
    ::urbi::UBinary bin;
    ::urbi::UImage img;
    bin.type;

    initOutputs();
}

void XXci::xciInit() {
    if (!inited_) {
        xci_->init();
        setFlyParamPersistence(stoi(xci_->parameter(xci::XCI_PARAM_FP_PERSISTENCE)));
        flyParamThread_ = move(thread(&XXci::keepFlyParam, this));
        inited_ = true; // TODO check this variable in all commands to the drone
    }
    else {
        cerr << "[XXci] already called init." << endl; //TODO general way for runtime warnings (in Urbi)
    }
}

void XXci::doCommand(const std::string& command) {
    setFlyParamActive(false);
    xci_->command(command);
}

void XXci::flyParam(double roll, double pitch, double yaw, double gaz) {
    roll_ = roll;
    pitch_ = pitch;
    yaw_ = yaw;
    gaz_ = gaz;
    sendFlyParam();
    setFlyParamActive();
}

void XXci::dumpConfiguration() {
    xci_->configuration();
}

void XXci::setConfiguration(const std::string& key, const std::string& value) {
    xci_->configuration(key, value);
}

void XXci::onChangeRoll(double roll) {
    roll_ = roll;
    sendFlyParam();
    setFlyParamActive();
}

void XXci::onChangePitch(double pitch) {
    pitch_ = pitch;
    sendFlyParam();
    setFlyParamActive();
}

void XXci::onChangeYaw(double yaw) {
    yaw_ = yaw;
    sendFlyParam();
    setFlyParamActive();
}

void XXci::onChangeGaz(double gaz) {
    gaz_ = gaz;
    sendFlyParam();
    setFlyParamActive();
}

void XXci::initOutputs() {
    for (auto sensor : xci_->sensorList()) {
        ::urbi::UVar* uvar = new ::urbi::UVar();
        UBindVarRename(XXci, *uvar, sensor.name);
        cout << "Registered sensor " << sensor.name << endl;
        dataReceiver_.registerOutput(sensor.name, uvar);
    }
}

void XXci::setFlyParamPersistence(unsigned int value) {
    flyParamPersistence_ = value;
    setFlyParamActive(flyParamActive_); // notifies
}

void XXci::setFlyParamActive(bool value) {
    if (!flyParamThread_.joinable()) { // thread haven't started yet
        flyParamActive_ = value;
        return;
    }
    else {
        lock_guard<mutex> lock(flyParamMtx_);
        flyParamActive_ = value;
    }
    flyParamCond_.notify_all();
}

void XXci::keepFlyParam() {
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

void XXci::sendFlyParam() {
    xci_->flyParam(roll_, pitch_, yaw_, gaz_);
}

XXci::~XXci() {

}

XStart(XXci);

