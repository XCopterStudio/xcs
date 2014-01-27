/* 
 * File:   Onboard.cpp
 * Author: michal
 * 
 * Created on January 21, 2014, 8:49 PM
 */

#include <vector>
#include <sstream>
#include <thread>
#include <iterator>
#include <cstdarg>
#include "Onboard.hpp"
#include "XCI.hpp"
#include "XCILoader.hpp"

using namespace std;

Onboard::Onboard() {
    init();
}

Onboard::~Onboard() {
    delete xci;
}

bool Onboard::DoCommand(const std::string& cmd, float roll, float pitch, float yaw, float gaz) {
    // check init state
    if (state != Onboard::INIT) {
        return false;
    }

    if (cmd == "TakeOff") {
        xci->sendCommand("TakeOff");
    } else if (cmd == "Land") {
        xci->sendCommand("Land");
    } else if (cmd == "EmegrencyStop") {
        xci->sendCommand("EmegrencyStop");
    } else if (cmd == "Normal") {
        xci->sendCommand("Normal");
    } else if (cmd == "Reset") {
        xci->sendCommand("Reset");
    } else if (cmd == "FlyParam") {
        xci->sendFlyParam(roll, pitch, yaw, gaz);
    } else {
        //TODO: unknown command exception
        return false;
    }
    return true;
}

void Onboard::init() {
    state = Onboard::UNDEFINED;

    // init XCI
    xci = getXci();
    if (xci == NULL) {
        state = Onboard::SEROR;
    } else {
        try {
            xci->init();
            state = Onboard::INIT;
        } catch (...) {
            state = Onboard::SEROR;
        }
    }
}

XCI* Onboard::getXci() {
    // TODO: vytvorit instanci XCI
    XCILoader loader;
    return loader.Load();
}