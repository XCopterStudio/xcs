/*
 * File:   XXci.cpp
 * Author: michal
 *
 * Created on January 31, 2014, 11:07 PM
 */

#include "x_xci.hpp"

#include <iostream>

#include <xcs/nodes/xobject/x.hpp>
#include <xcs/library_loader.hpp>
#include <xcs/symbol_loader.hpp>

using namespace xcs::nodes;
using namespace xcs::nodes::xci;
using namespace xcs::xci;
using namespace std;

XXci::XXci(const std::string& name) :
  xcs::nodes::XObject(name),
  flyControlPersistence("FLY_CONTROL_PERSISTENCE"),
  roll("ROLL"),
  pitch("PITCH"),
  yaw("YAW"),
  gaz("GAZ"),
  fly("FLY_CONTROL"),
  command("COMMAND"),
  inited_(false),
  roll_(0),
  pitch_(0),
  yaw_(0),
  gaz_(0),
  flyControlActive_(false),
  flyControlAlive_(false),
  flyControlPersistence_(0) {
    XBindFunction(XXci, init);
    XBindFunction(XXci, xciInit);
    XBindFunction(XXci, doCommand);
    XBindFunction(XXci, flyControl);
    XBindFunction(XXci, getConfiguration);
    XBindFunction(XXci, dumpConfiguration);
    XBindFunction(XXci, setConfiguration);

    XBindVarF(flyControlPersistence, &XXci::setFlyControlPersistence);

    XBindVarF(fly, &XXci::onChangeFly);

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
        setFlyControlPersistence(stoi(xci_->parameter(xcs::xci::XCI_PARAM_FP_PERSISTENCE)));
        flyControlAlive_ = true;
        flyControlThread_ = move(thread(&XXci::keepFlyControl, this));
        inited_ = true; // TODO check this variable in all commands to the drone
    } else {
        cerr << "[XXci] already called init." << endl; //TODO general way for runtime warnings (in Urbi)
    }
}

void XXci::doCommand(const std::string& command) {
    setFlyControlActive(false);
    roll_ = 0;
    pitch_ = 0;
    yaw_ = 0;
    gaz_ = 0;
    xci_->command(command);
}

void XXci::flyControl(double roll, double pitch, double yaw, double gaz) {
    //TODO here should be lock to atomic update of RPYG
    roll_ = roll;
    pitch_ = pitch;
    yaw_ = yaw;
    gaz_ = gaz;
    sendFlyControl();
    setFlyControlActive();
}

std::string XXci::getConfiguration(const std::string &key) {
    return xci_->configuration(key);
}

InformationMap XXci::dumpConfiguration() {
    return xci_->configuration();
}

void XXci::setConfiguration(const std::string& key, const std::string& value) {
    xci_->configuration(key, value);
}

void XXci::onChangeFly(FlyControl fp) {
    flyControl(fp.roll, fp.pitch, fp.yaw, fp.gaz);
    //cerr << "***************** " << fp.roll << " - " << fp.pitch << " - " << fp.yaw << " - " << fp.gaz;
}

void XXci::onChangeRoll(double roll) {
    roll_ = roll;
    sendFlyControl();
    setFlyControlActive();
}

void XXci::onChangePitch(double pitch) {
    pitch_ = pitch;
    sendFlyControl();
    setFlyControlActive();
}

void XXci::onChangeYaw(double yaw) {
    yaw_ = yaw;
    sendFlyControl();
    setFlyControlActive();
}

void XXci::onChangeGaz(double gaz) {
    gaz_ = gaz;
    sendFlyControl();
    setFlyControlActive();
}

void XXci::initOutputs() {
    for (auto sensor : xci_->sensorList()) {
        // todo: xvar and uBindVarRename
        ::urbi::UVar* uvar = new ::urbi::UVar();
        UBindVarRename(XXci, *uvar, sensor.name);
        cout << "Registered sensor " << sensor.name << endl;
        dataReceiver_.registerOutput(sensor.name, uvar);
    }
}

void XXci::stopFlyControlsThread() {
    flyControlAlive_ = false;
    flyControlCond_.notify_one();
    flyControlThread_.join();
}

void XXci::setFlyControlPersistence(unsigned int value) {
    flyControlPersistence_ = value;
    setFlyControlActive(flyControlActive_); // notifies
}

void XXci::setFlyControlActive(bool value) {
    if (!flyControlThread_.joinable()) { // thread haven't started yet
        flyControlActive_ = value;
        return;
    } else {
        lock_guard<mutex> lock(flyControlMtx_);
        flyControlActive_ = value;
    }
    flyControlCond_.notify_one();
}

void XXci::keepFlyControl() {
    unique_lock<mutex> lock(flyControlMtx_, defer_lock_t());

    while (flyControlAlive_) {
        lock.lock();
        flyControlCond_.wait(lock, [this] {
            return !flyControlAlive_ || (flyControlActive_ && flyControlPersistence_ > 0);
        });

        unsigned int localPersistence(flyControlPersistence_);
        lock.unlock();
        if (!flyControlAlive_) {
            break;
        }

        sendFlyControl();
        this_thread::sleep_for(chrono::milliseconds(localPersistence));
    }
}

void XXci::sendFlyControl() {
    xci_->flyControl(roll_, pitch_, yaw_, gaz_);
}

XXci::~XXci() {
    stopFlyControlsThread();
}

XStart(XXci);


