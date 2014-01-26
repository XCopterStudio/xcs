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

bool Onboard::DoCommand(const string& cmd, ...) {
    // check init state
    if (state != Onboard::INIT) {
        return false;
    }

    //TODO: upravit pocty a frekventovanost posilani prikazu
    if (cmd == "TakeOff") {
        for (int i = 0; i < 1000; ++i) {
            xci->sendCommand("TakeOff");
            this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    } else if (cmd == "Land") {
        for (int i = 0; i < 1000; ++i) {
            xci->sendCommand("Land");
            this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    } else if (cmd == "EmegrencyStop") {
        for (int i = 0; i < 200; ++i) {
            xci->sendCommand("EmegrencyStop");
            this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    } else if (cmd == "Normal") {
        for (int i = 0; i < 5; ++i) {
            xci->sendCommand("Normal");
            this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } else if (cmd == "Reset") {
        for (int i = 0; i < 5; ++i) {
            xci->sendCommand("Reset");
            this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } else if (cmd == "FlyParam") {
        va_list args; // TODO would variadic template be better? (float -> double)
        va_start(args, cmd);
        double roll = va_arg(args, double);
        double pitch = va_arg(args, double);
        double yaw = va_arg(args, double);
        double gaz = va_arg(args, double);
        xci->sendFlyParam(roll, pitch, yaw, gaz);
        va_end(args);
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