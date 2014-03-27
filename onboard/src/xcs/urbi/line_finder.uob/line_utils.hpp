/* 
 * File:   line_utils.hpp
 * Author: michal
 *
 * Created on March 20, 2014, 3:04 PM
 */

#ifndef LINE_UTILS_HPP
#define	LINE_UTILS_HPP
#include <opencv2/opencv.hpp>
#include <cmath>

namespace xcs {
namespace urbi {
namespace line_finder {

struct LineUtils {
public:
    typedef cv::Vec4i RawLineType;
    //double cameraParam;
    double distanceUnit;
    cv::Point imageCenter;
    cv::Point referencePoint;
    size_t width;
    size_t height;

    inline void setDimensions(size_t width, size_t height) {
        this->width = width;
        this->height = height;
        distanceUnit = hypot(height, width);
        imageCenter = cv::Point(width / 2, height / 2);
    }

    inline void updateReferencePoint(double theta, double phi, double cameraParam) {
        auto tanTheta = tan(theta);
        auto tanPhi = tan(phi);

        auto refDistance = cameraParam * hypot(tanTheta, tanPhi);
        auto refDeviation = atan2(tanPhi, tanTheta);

        referencePoint = imageCenter + cv::Point(distanceUnit * refDistance * sin(refDeviation), distanceUnit * refDistance * cos(refDeviation));
    }

    inline cv::Point getDeltaPoint(double distance, double deviation) {
        cv::Point deltaPoint(distanceUnit * distance * cos(deviation), distanceUnit * distance * sin(deviation));
        return deltaPoint + referencePoint;
    }

    /*!
     * When line has zero legth, distance between its beginning and the point is returned.
     * 
     * \return signed distance between point and line
     */
    static inline double pointLineDistance(cv::Point point, RawLineType line) {
        cv::Point norm(line[3] - line[1], line[0] - line[2]);
        if (abs(norm.x) <= 0 && abs(norm.y) <= 0) { // avoid division by zero
            return hypot(line[0] - point.x, line[1] - point.y);
        }
        auto c = -norm.dot(cv::Point(line[0], line[1]));
        return (norm.dot(point) + c) / hypot(norm.x, norm.y);
    }

    static inline double pointLineDistance2(cv::Point point, double deviation, cv::Point lineOrigin) {
        cv::Point2d norm(cos(deviation), sin(deviation)); // this is correct normal vector, deviation is "top oriented"
        auto c = -norm.dot(lineOrigin);
        return (norm.dot(point) + c) / hypot(norm.x, norm.y);
    }

    static inline void normalizeOrientation(RawLineType &line) {
        if (line[1] < line[3]) {
            std::swap(line[0], line[2]);
            std::swap(line[1], line[3]);
        }
    }

    static inline double lineDirection(RawLineType line) {
        cv::Point dir(line[2] - line[0], line[1] - line[3]);
        return atan2(dir.x, dir.y);
    }

    static inline cv::Point lineCenter(RawLineType line) {
        return cv::Point((line[0] + line[2]) / 2, (line[3] + line[1]) / 2);
    }
    
    static inline double lineLength(RawLineType line) {
        cv::Point dir(line[2] - line[0], line[1] - line[3]);
        return hypot(dir.x, dir.y);
    }
};
}
}
}

#endif	/* LINE_UTILS_HPP */

