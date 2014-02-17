#ifndef BASIC_H
#define BASIC_H

#define _USE_MATH_DEFINES

#include <cmath>

//! Check if value is in interval <-range,range> and if not set value to the limits of the interval.

/*!
	\param value is checked on interval <-range,range>
	\param range set size of interval it should be positive number
	\return value from interval if input value is outside of the interval then return one of the interval boundary
*/
template <class type>
type valueInRange(const type value,const type range){
	type absRange = 0;

	// make from input range positive number
	if(range < 0)
		absRange = -range;
	else
		absRange = range;

	// value is smaller then -range
	if(value < -range){
		return - range;
	}else{ // value is greater then -range
		if(value < range){ // value is greater then -range and smaller then range
			return value;
		}else{ // value is greater then range
			return range; 
		}
	}
}

inline double miliDegreesToRadias(int degrees){
    return (double)( degrees * M_PI / 180000);
}

#endif