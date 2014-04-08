#ifndef BITMAPTYPE_HPP
#define	BITMAPTYPE_HPP

#include <cstdint>
#include <cstddef>

namespace xcs {

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

#endif	/* BITMAPTYPE_HPP */

