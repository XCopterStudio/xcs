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
#include "xcs/xci/xci.hpp"
#include "XCILoader.hpp"

using namespace std;
using namespace xcs::xci;

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
        xci->command("TakeOff");
    } else if (cmd == "Land") {
        xci->command("Land");
    } else if (cmd == "EmegrencyStop") {
        xci->command("EmegrencyStop");
    } else if (cmd == "Normal") {
        xci->command("Normal");
    } else if (cmd == "Reset") {
        xci->command("Reset");
    } else if (cmd == "FlyParam") {
        xci->flyParam(roll, pitch, yaw, gaz);
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