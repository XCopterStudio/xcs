#ifndef BITMAPTYPE_HPP
#define	BITMAPTYPE_HPP

#include <cstdint>
#include <cstddef>

namespace xcs {

struct BitmapType {
    const size_t width;
    const size_t height;
    uint8_t * const data;

    BitmapType(const size_t width = 0, const size_t height = 0, uint8_t * const data = nullptr) :
      width(width),
      height(height),
      data(data) {
    }
};

struct BitmapTypeChronologic : public BitmapType {
    long int time; // in milliseconds

    BitmapTypeChronologic(const size_t width = 0, const size_t height = 0, uint8_t * const data = nullptr, long int time = -1) :
      BitmapType(width, height, data),
      time(time) {
    }

    BitmapTypeChronologic(const BitmapType &bitmap, long int time = -1) :
      BitmapType(bitmap), time(time) {
    }

};

}

#endif	/* BITMAPTYPE_HPP */

