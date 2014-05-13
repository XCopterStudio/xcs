#ifndef BITMAPTYPE_HPP
#define	BITMAPTYPE_HPP

#include <cstdint>
#include <cstddef>

#include "timestamp.hpp"

namespace xcs {

/*!
 * Structure is used for deserialization of a bitmap.
 * A video player is needed to obtain BitmapType from the information.
 */
struct FrameInfo {
    int frameNumber;
    /*! Timestamp of frame creation, relative to reference point. */
    Timestamp frameTimestamp;

    FrameInfo(int frameNumber = 0, Timestamp frameTimestap = -1) :
      frameNumber(frameNumber),
      frameTimestamp(frameTimestamp) {

    }

};

struct BitmapType {
    const size_t width;
    const size_t height;
    uint8_t * const data;

    BitmapType(const size_t width = 0, const size_t height = 0, uint8_t * const data = nullptr) :
      width(width),
      height(height),
      data(data) {
    }

    template<typename T>
            static FrameInfo deserialize(T &stream) {
        int frameNumber;
        stream >> frameNumber;
        return FrameInfo(frameNumber);
    }
};

}

#endif	/* BITMAPTYPE_HPP */

