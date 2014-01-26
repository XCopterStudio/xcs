/* 
 * File:   testAdapter.cpp
 * Author: michal
 *
 * Created on January 21, 2014, 8:55 PM
 */
#include <iostream>

#include "Forest.hpp"
#include "Adapter.hpp"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    std::cout << "Start" << std::endl;
    Forest forest;
    Adapter adapter(forest);

    adapter.start("127.0.0.1", 1234);

    return 0;
}

