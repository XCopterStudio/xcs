/* 
 * File:   Forest.cpp
 * Author: michal
 * 
 * Created on January 21, 2014, 8:49 PM
 */

#include "Forest.hpp"
#include <string>
#include <iostream>

Forest::Forest() {
}

Forest::Forest(const Forest& orig) {
}

Forest::~Forest() {
}

void Forest::doCommand(std::string command) {
    std::cout << "Command: " << command << std::endl;
}

