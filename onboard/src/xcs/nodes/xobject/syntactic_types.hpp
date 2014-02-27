/* 
 * File:   syntactic_types.hpp
 * Author: michal
 *
 * Created on February 25, 2014, 11:17 AM
 */

#ifndef SYNTACTICTYPES_HPP
#define	SYNTACTICTYPES_HPP

#include <cstdint>

namespace xcs {
namespace nodes {

struct BitmapType {
    unsigned int width;
    unsigned int height;
    uint8_t* data;
};
}
}

#endif	/* SYNTACTICTYPES_HPP */

