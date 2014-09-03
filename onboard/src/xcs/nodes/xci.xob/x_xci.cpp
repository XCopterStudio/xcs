/*
 * File:   XXci.cpp
 * Author: michal
 *
 * Created on January 31, 2014, 11:07 PM
 */

#include "x_xci.hpp"

#include <iostream>
#include <mutex>

#include <xcs/logging.hpp>
#include <xcs/nodes/xobject/x.h>
#include <xcs/nodes/xobject/x_type.hpp>
#include <xcs/library_loader.hpp>
#include <xcs/symbol_loader.hpp>

using namespace xcs::nodes;
using namespace xcs::xci;
using namespace xcs;
using namespace std;

XXci::XXci(const std::string& name) :
  xcs::nodes::XObject(name),
  flyControlPersistence("FLY_CONTROL_PERSISTENCE"),
  setFlyControlPersistence("FLY_CONTROL_PERSISTENCE"),
  roll("ROLL"),
  pitch("PITCH"),
  yaw("YAW"),
  gaz("GAZ"),
  flyControl("FLY_CONTROL"),
  command("COMMAND"),
  xci_(nullptr),
  xciInited_(false),
  roll_(0),
  pitch_(0),
  yaw_(0),
  gaz_(0),
  flyControlActive_(false),
  flyControlAlive_(false),
  flyControlPersistence_(0) {
    XBindFunction(XXci, init);
    XBindFunction(XXci, getConfiguration);
    XBindFunction(XXci, dumpConfiguration);
    XBindFunction(XXci, setConfiguration);

    XBindVarF(setFlyControlPersistence, &XXci::onChangeFlyControlPersistence);
    XBindVar(flyControlPersistence);

    XBindVarF(flyControl, &XXci::onChangeFly);

    XBindPrivateVarF(roll, &XXci::onChangeRoll);
    XBindPrivateVarF(pitch, &XXci::onChangePitch);
    XBindPrivateVarF(yaw, &XXci::onChangeYaw);
    XBindPrivateVarF(gaz, &XXci::onChangeGaz);

    XBindVarF(command, &XXci::onChangeCommand);

}

void XXci::init(const std::string& driver) {
    LibraryLoader& libraryLoader = LibraryLoader::getInstance(driver);
    libraryLoader.load();
    SymbolLoader<XciFactoryFunction*> symbolLoader(libraryLoader);
    XciFactoryFunction* factory = symbolLoader.loadSymbol("CreateXci");
    xci_ = factory(dataReceiver_);

    initOutputs();
}

void XXci::stateChanged(XObject::State state) {
    switch (state) {
        case XObject::STATE_STARTED:
            xciStart();
            break;
        case XObject::STATE_STOPPED:
            xciStop();
            break;
    }
}

void XXci::xciStart() {
    xci_->init();

    if (!xciInited_) {
        std::string controlPersistence = xci_->configuration("XCI_PARAM_FP_PERSISTENCE");
        if (controlPersistence == "") {
            XCS_LOG_FATAL("We cannot obtain XCI_PARAM_FP_PERSISTENCE from xci.");
        }
        onChangeFlyControlPersistence(stoi(controlPersistence));
        flyControlAlive_ = true;
        flyControlThread_ = move(thread(&XXci::keepFlyControl, this));
        xciInited_ = true;
    }
}

void XXci::xciStop() {
    if (!xciInited_) {
        XCS_LOG_WARN("[XXci] not initialized.");
    } else {
        setFlyControlActive(false); // NOTE: after re-starting flyControl persistence won't be deliberately active
        xci_->stop();
    }
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
    {
        mutex mtx;
        unique_lock<mutex>lock(mtx);

        roll_ = fp.roll;
        pitch_ = fp.pitch;
        yaw_ = fp.yaw;
        gaz_ = fp.gaz;
    }
    sendFlyControl();
    setFlyControlActive();
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

void XXci::onChangeCommand(const std::string& command) {
    setFlyControlActive(false);
    {
        mutex mtx;
        unique_lock<mutex>lock(mtx);

        roll_ = 0;
        pitch_ = 0;
        yaw_ = 0;
        gaz_ = 0;
    }
    xci_->command(command);
}

void XXci::initOutputs() {
    for (auto sensor : xci_->sensorList()) {
        SimpleXVar &xvar = dataReceiver_.registerOutput(sensor.name, XType(sensor.syntacticType, sensor.semanticType, XType::DATAFLOWTYPE_XVAR));
        XBindVarRename(xvar, sensor.name);
        XCS_LOG_INFO("Registered sensor " << sensor.name << ".");
    }
}

void XXci::stopFlyControlsThread() {
    if (flyControlThread_.joinable()) {
        flyControlAlive_ = false;
        flyControlCond_.notify_one();
        flyControlThread_.join();
    }
}

void XXci::onChangeFlyControlPersistence(unsigned int value) {
    flyControlPersistence = flyControlPersistence_ = value;

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
    if (xci_ != nullptr) {
        delete xci_;
    }
}

XStart(XXci);


