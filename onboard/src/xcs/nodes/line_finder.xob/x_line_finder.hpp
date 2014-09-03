#ifndef U_LINE_FINDER_HPP
#define U_LINE_FINDER_HPP



#include <vector>
#include <cstdint>
#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_var.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>
#include <xcs/types/eulerian_vector.hpp>

#include <opencv2/opencv.hpp>

#include <xcs/nodes/line_drawer.xob/x_line_drawer.hpp>
#include "line_utils.hpp"


namespace xcs {
namespace nodes {

class XLineFinder : public XObject {
public:
    /*
     * Inputs
     */
    XInputPort<::urbi::UImage> video;
    XInputPort<xcs::EulerianVector> rotation;
    XInputPort<double> expectedDistance; // intentionally UVar
    XInputPort<double> expectedDeviation; // intentionally UVar

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
    XVar<double> distance;
    XVar<double> deviation;
    XVar<double> hasLine;
    XVar<double> curvature;
    XVar<::urbi::UImage> enhancedVideo;

    XLineFinder(const std::string &);
    void init();
private:

    enum LineType {
        LINE_NONE, //! No visible line.
        LINE_VISUAL, //! Visible line.
    };

    static const size_t REFRESH_PERIOD;
    static const size_t STUCK_TOLERANCE;

    cv::Mat lastFrame_;
    size_t lastProcessedFrameNo_;
    double distance_;
    double deviation_;
    double curvature_;

    xcs::EulerianVector rotation_;
    double expectedDistance_;
    double expectedDeviation_;

    // processing results
    LineType lineType_;
    xcs::nodes::line_finder::LineUtils lineUtils_;

    inline bool isLineVisual(LineType lineType) {
        return lineType == LINE_VISUAL;
    }

    void onChangeVideo(::urbi::UImage &frame);
    void onChangeRotation(xcs::EulerianVector &rotation){ rotation_ = rotation; };
    void onChangeExpectedDistance(double distance){ distance_ = distance; };
    void onChangeExpectedDeviation(double deviation){ deviation_ = deviation; };
    
    void adjustValueRange(cv::Mat image);

    void useOnlyGoodLines(cv::vector<xcs::nodes::line_finder::LineUtils::RawLineType> lines, cv::vector<xcs::nodes::line_finder::LineUtils::RawLineType> & goodLines, cv::vector<xcs::nodes::line_finder::LineUtils::RawLineType> & curvatureLines);
    
    double calculateCurvature(xcs::nodes::line_finder::LineUtils::RawLineType meanLine, cv::vector<xcs::nodes::line_finder::LineUtils::RawLineType>& curvatureLines, cv::Mat image);

    void drawDebugLines(cv::Mat& canvas, const cv::vector<xcs::nodes::line_finder::LineUtils::RawLineType>& lines, const cv::vector<xcs::nodes::line_finder::LineUtils::RawLineType>& filteredLines);

    /*!
    * Very ugly geometry.
    */
    void drawFullLine(cv::Mat& canvas, double distance, double deviation, cv::Scalar color, size_t width = 3, bool withCircle = false);
};

}
}

#endif // U_LINE_FINDER_HPP
