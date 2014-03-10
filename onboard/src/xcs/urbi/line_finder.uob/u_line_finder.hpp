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
    /*!
     * Output params
     */
    ::urbi::UVar distance;
    ::urbi::UVar deviation;
    ::urbi::UVar line;

    ULineFinder(const std::string &);
    void init();

    /*!
     * Urbi period handler
     */
    virtual int update();
private:
    static const size_t REFRESH_PERIOD;
    void onChangeVideo(::urbi::UVar &uvar);
    void processFrame();

    bool hasFrame_;
    ::urbi::UImage lastFrame_;
    size_t lastReceivedFrameNo_;
    size_t lastProcessedFrameNo_;

    // processing results
    size_t imageHeight_;
    size_t imageWidth_;
    cv::Point imageCenter_;
    double distance_;
};

}
}

#endif // U_LINE_FINDER_HPP
