/* 
 * File:   Integrator.hpp
 * Author: michal
 *
 * Created on January 21, 2014, 5:21 PM
 */

#ifndef INTEGRATOR_HPP
#define	INTEGRATOR_HPP

struct Vector3 {
    float x;
    float y;
    float z;

    Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {
    }

};

class Integrator {
public:
    Integrator();
    Integrator(const Integrator& orig);
    virtual ~Integrator();

    void reset();

    Vector3 getResult();

    void addSample(Vector3 acceleration, float dTime);

private:
    Vector3 result;
};

#endif	/* INTEGRATOR_HPP */

