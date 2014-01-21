/* 
 * File:   Integrator.cpp
 * Author: michal
 * 
 * Created on January 21, 2014, 5:21 PM
 */

#include "Integrator.hpp"

Integrator::Integrator() {
}

Integrator::Integrator(const Integrator& orig) {
}

Integrator::~Integrator() {
}

Integrator::reset() {
    result = Vector3();
}

Integrator::getResult() {
    return result;
}

Integrator::addSample(Vector3 acceleration, float dTime) {
    result.x += acceleration.x * dTime;
    result.y += acceleration.y * dTime;
    result.z += acceleration.z * dTime;
}