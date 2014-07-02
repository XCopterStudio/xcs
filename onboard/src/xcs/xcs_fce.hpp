#ifndef XCS_FCE_H
#define XCS_FCE_H

#include <cmath>

namespace xcs{
    /*!
    \param value is checked on interval [\param from, \param to]
    \param range set size of interval it should be positive number
    \return value from interval if input value is outside of the interval then return one of the interval boundary
    */
    template <typename T>
    inline T valueInRange(const T value, const T from, const T to) {
        return (value < from) ? from :
            (value > to) ? to :
            value;

    }

    /*!
    * Check if value is in interval <-range,range> and if not set value to the limits of the interval.

    \param value is checked on interval <-range,range>
    \param range set size of interval it should be positive number
    \return value from interval if input value is outside of the interval then return one of the interval boundary
    */
    template <typename T>
    inline T valueInRange(const T value, const T range) {
        return valueInRange(value, -range, range);
    }

    inline double degreesToRadians(const double degrees){
        return M_PI * (degrees / 180.0);
    }

    inline double radiansToDegrees(const double radians){
        return 180.0 * (radians / M_PI);
    }

    inline double miliDegreesToRadias(const int degrees) {
        return (double)(degrees * M_PI / 180000);
    }

    template <typename T>
    int sgn(const T val) {
        return (T(0) < val) - (val < T(0));
    }

    inline double normAngle(const double angle){
        double temp = angle;
        while (temp > M_PI) temp -= 2*M_PI;
        while (temp < -M_PI) temp += 2*M_PI;

        return temp;
    }
}

#endif