#ifndef U_LINE_FINDER_HPP
#define U_LINE_FINDER_HPP



#include <vector>
#include <cstdint>
#include <urbi/uobject.hh>
#include <opencv2/opencv.hpp>

#include <xcs/urbi/line_drawer.uob/u_line_drawer.hpp>
#include "line_utils.hpp"


namespace xcs {
namespace urbi {

class ULineFinder : public ::urbi::UObject {
public:
    /*!
     * Inputs
     */
    ::urbi::InputPort video;
    ::urbi::UVar theta; // intentionally UVar
    ::urbi::UVar phi; // intentionally UVar
    ::urbi::UVar psi; // intentionally UVar

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

    ::urbi::UVar hystForgetDerRatio;
    ::urbi::UVar hystForgetDerThreshold;

    ::urbi::UVar cameraParam;

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

    void setLineDrawer(UObject *drawer);
private:

    enum LineType {
        LINE_NONE,
        LINE_VISUAL,
        LINE_REMEMBERED
    };

    static const size_t REFRESH_PERIOD;


    static const double DEFAULT_DEVIATION; //! deviation set when no clues are present
    static const double DEFAULT_DISTANCE; //! distance set when no clues are present

    xcs::urbi::ULineDrawer *lineDrawer_;

    void onChangeVideo(::urbi::UVar &uvar);
    void adjustValueRange(cv::Mat image);
    void processFrame();
    void useRememberedLine();
    cv::vector<xcs::urbi::line_finder::LineUtils::RawLineType> useOnlyGoodLines(cv::vector<xcs::urbi::line_finder::LineUtils::RawLineType> lines);
    void calculateExpectedLine();

    bool hasFrame_;
    ::urbi::UImage lastFrame_;
    size_t lastReceivedFrameNo_;
    size_t lastProcessedFrameNo_;

    // processing results
    double distance_;
    double deviation_;
    double distanceDer_;
    double deviationDer_;
    double expectedDistance_;
    double expectedDeviation_;
    LineType lineType_;
    double hystStrength_;
    double hystDerStrength_;
    double nonVisualOffset_; //! Angular difference between "non-visual" line and visual line (as of last time seen)
    xcs::urbi::line_finder::LineUtils lineUtils_;

    inline bool isLineVisual(LineType lineType) {
        return lineType == LINE_VISUAL || lineType == LINE_REMEMBERED;
    }

};

}
}

#endif // U_LINE_FINDER_HPP
