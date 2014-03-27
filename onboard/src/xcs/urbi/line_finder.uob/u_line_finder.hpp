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
    /*
     * Inputs
     */
    ::urbi::InputPort video;
    ::urbi::UVar theta; // intentionally UVar
    ::urbi::UVar phi; // intentionally UVar
    ::urbi::UVar expectedDistanceUVar; // intentionally UVar
    ::urbi::UVar expectedDeviationUVar; // intentionally UVar

    /*
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

    /*
     * Line finding params
     */
    ::urbi::UVar hystDistanceThr;
    ::urbi::UVar hystDeviationThr;
    ::urbi::UVar curvatureTolerance;

    ::urbi::UVar cameraParam;

    /*!
     * Output params
     */
    ::urbi::UVar distanceUVar;
    ::urbi::UVar deviationUVar;
    ::urbi::UVar hasLine;
    ::urbi::UVar curvatureUVar;

    ULineFinder(const std::string &);
    void init();

    /*!
     * Urbi period handler
     */
    virtual int update();

    void setLineDrawer(UObject *drawer);
private:

    enum LineType {
        LINE_NONE, //! No visible line.
        LINE_VISUAL, //! Visible line.
    };

    static const size_t REFRESH_PERIOD;
    static const size_t STUCK_TOLERANCE;


    xcs::urbi::ULineDrawer *lineDrawer_;

    bool hasFrame_;
    ::urbi::UImage lastFrame_;
    size_t lastReceivedFrameNo_;
    size_t lastProcessedFrameNo_;
    size_t stuckCounter_;
    double distance_;
    double deviation_;
    double curvature_;

    // processing results
    LineType lineType_;
    xcs::urbi::line_finder::LineUtils lineUtils_;

    inline bool isLineVisual(LineType lineType) {
        return lineType == LINE_VISUAL;
    }

    void onChangeVideo(::urbi::UVar &uvar);

    void adjustValueRange(cv::Mat image);

    void processFrame();

    void useOnlyGoodLines(cv::vector<xcs::urbi::line_finder::LineUtils::RawLineType> lines, cv::vector<xcs::urbi::line_finder::LineUtils::RawLineType> & goodLines, cv::vector<xcs::urbi::line_finder::LineUtils::RawLineType> & curvatureLines);
    
    double calculateCurvature(xcs::urbi::line_finder::LineUtils::RawLineType meanLine, cv::vector<xcs::urbi::line_finder::LineUtils::RawLineType>& curvatureLines, cv::Mat image);

    void drawDebugLines(const cv::vector<xcs::urbi::line_finder::LineUtils::RawLineType>& lines, const cv::vector<xcs::urbi::line_finder::LineUtils::RawLineType>& filteredLines);

};

}
}

#endif // U_LINE_FINDER_HPP
