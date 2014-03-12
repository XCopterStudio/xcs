#ifndef U_LINE_FINDER_HPP
#define U_LINE_FINDER_HPP

#include <vector>
#include <cstdint>
#include <urbi/uobject.hh>
#include <opencv2/opencv.hpp>


namespace xcs {
namespace urbi {

class ULineFinder : public ::urbi::UObject {
public:

    ::urbi::InputPort video;

    /*!
     * Image processing params
     */
    ::urbi::UVar blurRange;
    ::urbi::UVar autoHsvValueRangeEnabled;
    ::urbi::UVar autoHsvValueRangeRatio;
    ::urbi::UVar autoHsvValueRangeFreq;
    ::urbi::UVar hsvValueRange;
    ::urbi::UVar cannyT1;
    ::urbi::UVar cannyT2;
    ::urbi::UVar cannyApertureSize;
    ::urbi::UVar cannyL2Gradient;
    ::urbi::UVar houghRho;
    ::urbi::UVar houghTheta;
    ::urbi::UVar houghT;
    ::urbi::UVar houghMinLength;
    ::urbi::UVar houghMaxGap;

    ::urbi::UVar distanceAging;
    ::urbi::UVar deviationAging;

    ::urbi::UVar hystDirThreshold;
    ::urbi::UVar hystCenterThreshold;

    ::urbi::UVar hystForgetRatio;
    ::urbi::UVar hystForgetThreshold;
    /*!
     * Output params
     */
    ::urbi::UVar distanceUVar;
    ::urbi::UVar deviationUVar;
    ::urbi::UVar hasLine;

    ULineFinder(const std::string &);
    void init();

    /*!
     * Urbi period handler
     */
    virtual int update();
private:
    typedef cv::Vec4i RawLineType;

    enum LineType {
        LINE_NONE,
        LINE_VISUAL,
        LINE_REMEMBERED
    };

    static const size_t REFRESH_PERIOD;


    static const double DEFAULT_DEVIATION; //! deviation set when no clues are present
    static const double DEFAULT_DISTANCE; //! distance set when no clues are present

    void onChangeVideo(::urbi::UVar &uvar);
    void adjustValueRange(cv::Mat image);
    void processFrame();
    void useRememberedLine();
    cv::vector<RawLineType> useOnlyGoodLines(cv::vector<RawLineType> lines);

    bool hasFrame_;
    ::urbi::UImage lastFrame_;
    size_t lastReceivedFrameNo_;
    size_t lastProcessedFrameNo_;

    double distanceUnit_; //! Length of reference unit in pixels
    // processing results
    size_t imageHeight_;
    size_t imageWidth_;
    cv::Point imageCenter_;
    double distance_;
    double deviation_;
    LineType lineType_;
    double hystStrength_;

    /*!
     * When line has zero legth, distance between its beginning and the point is returned.
     * 
     * \return signed distance between point and line
     */
    inline double pointLineDistance(cv::Point point, RawLineType line) {
        cv::Point norm(line[3] - line[1], line[0] - line[2]);
        if (abs(norm.x) <= 0 && abs(norm.y) <= 0) { // avoid division by zero
            return hypot(line[0] - point.x, line[1] - point.y);
        }
        auto c = -norm.dot(cv::Point(line[0], line[1]));
        return (norm.dot(point) + c) / hypot(norm.x, norm.y);
    }

    inline double pointLineDistance2(cv::Point point, double deviation, cv::Point lineOrigin) {
        cv::Point norm(cos(deviation), sin(deviation));
        if (abs(norm.x) <= 0 && abs(norm.y) <= 0) { // avoid division by zero
            return hypot(lineOrigin.x - point.x, lineOrigin.y - point.y);
        }
        auto c = -norm.dot(lineOrigin);
        return (norm.dot(point) + c) / hypot(norm.x, norm.y);
    }

    inline void normalizeOrientation(RawLineType &line) {
        if (line[1] < line[3]) {
            std::swap(line[0], line[2]);
            std::swap(line[1], line[3]);
        }
    }

    inline double lineDirection(RawLineType line) {
        cv::Point dir(line[2] - line[0], line[1] - line[3]);
        return atan2(dir.x, dir.y);
    }
};

}
}

#endif // U_LINE_FINDER_HPP
