/* 
 * File:   syntactic_types.hpp
 * Author: michal
 *
 * Created on February 25, 2014, 11:17 AM
 */

#ifndef SYNTACTICTYPES_HPP
#define	SYNTACTICTYPES_HPP

#include <cstdint>
#include <cstddef>

namespace xcs {
namespace nodes {

struct BitmapType {

    BitmapType(const size_t width = 0, const size_t height = 0, uint8_t * const data = nullptr) :
      width(width),
      height(height),
      data(data) {
    };
    
    const size_t width;
    const size_t height;
    uint8_t * const data;
};
}
}

#endif	/* SYNTACTICTYPES_HPP */

