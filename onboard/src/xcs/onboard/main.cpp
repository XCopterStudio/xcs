/* 
 * File:   main.cpp
 * Author: michal
 *
 * Created on January 22, 2014, 1:16 PM
 */

#include "Adapter.hpp"
#include "XCILoader.hpp"
#include "Onboard.hpp"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    vector<string> args(argv, argv + argc);

    if (args.size() < 3) {
        cerr << "Usage: " << args[0] << " IP port" << endl;
        return 1;
    }

    istringstream ss(args[2]);
    int port;
    ss >> port;
    Onboard onboard;
    Adapter adapter(onboard);
    adapter.start(args[1], port);

    return 0;
}

