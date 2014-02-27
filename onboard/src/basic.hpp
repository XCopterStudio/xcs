#ifndef BASIC_H
#define BASIC_H

#include <cmath>

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

inline double miliDegreesToRadias(int degrees) {
    return (double) (degrees * M_PI / 180000);
}

#endif