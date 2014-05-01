#ifndef XCS_FCE_H
#define XCS_FCE_H

#include <cmath>

namespace xcs{
    
    template <class type>
    inline type inBoundary(const type &lowerBoundary, const type &upperBoundary, const type &value){
        if (value < lowerBoundary){
            return lowerBoundary;
        }else if (value > upperBoundary){
            return upperBoundary;
        }else{
            return value;
        }
    }

    template <class type>
    inline type inBoundary(const type &boundary, const type &value){
        if (value < -boundary){
            return -boundary;
        }
        else if (value > boundary){
            return boundary;
        }
        else{
            return value;
        }
    }

    inline double normAngle(const double &angle){
        double temp = angle;
        while (temp > M_PI) temp -= M_PI;
        while (temp < -M_PI) temp += M_PI;

        return temp;
    }
}

#endif