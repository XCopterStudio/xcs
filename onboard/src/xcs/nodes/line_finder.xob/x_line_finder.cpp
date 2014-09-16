#include "x_line_finder.hpp"
#include <thread>
#include <chrono>
#include <cmath>
#include <xcs/xcs_fce.hpp>
#include <xcs/nodes/xobject/x.h>

using namespace xcs::nodes;
using namespace xcs::nodes::line_finder;
using namespace std;

const size_t XLineFinder::REFRESH_PERIOD = 66; // ms
const size_t XLineFinder::STUCK_TOLERANCE = 2; // timer periods

XLineFinder::XLineFinder(const string& name) :
XObject(name),
video("CAMERA"),
rotation("ROTATION"),
expectedDistance("DISTANCE"),
expectedDeviation("DEVIATION"),
distance("DISTANCE"),
deviation("DEVIATION"),
hasLine("DECISION"),
curvature("CURVATURE"),
enhancedVideo("CAMERA"),
lastProcessedFrameNo_(0),
distance_(0),
deviation_(0),
lineType_(LINE_NONE)
  {
    XBindFunction(XLineFinder, init);

    XBindVar(video);
    UNotifyThreadedChange(video.data(), &XLineFinder::onChangeVideo, urbi::LOCK_FUNCTION_DROP);

    XBindVarF(rotation, &XLineFinder::onChangeRotation);
    UBindVar(XLineFinder, cameraParam);
    XBindVarF(expectedDistance, &XLineFinder::onChangeExpectedDistance);
    XBindVarF(expectedDeviation, &XLineFinder::onChangeExpectedDeviation);


    UBindVar(XLineFinder, blurRange);
    UBindVar(XLineFinder, autoHsvValueRangeEnabled);
    UBindVar(XLineFinder, autoHsvValueRangeRatio);
    UBindVar(XLineFinder, autoHsvValueRangeFreq);
    UBindVar(XLineFinder, hsvValueRange);
    UBindVar(XLineFinder, cannyT1);
    UBindVar(XLineFinder, cannyT2);
    UBindVar(XLineFinder, cannyApertureSize);
    UBindVar(XLineFinder, cannyL2Gradient);
    UBindVar(XLineFinder, houghRho);
    UBindVar(XLineFinder, houghTheta);
    UBindVar(XLineFinder, houghT);
    UBindVar(XLineFinder, houghMinLength);
    UBindVar(XLineFinder, houghMaxGap);
    UBindVar(XLineFinder, hystDeviationThr);
    UBindVar(XLineFinder, hystDeviationThrBase);
    UBindVar(XLineFinder, hystDeviationThrRate);
    UBindVar(XLineFinder, hystDistanceThr);
    UBindVar(XLineFinder, curvatureTolerance);
    

    XBindVar(distance);
    XBindVar(deviation);
    XBindVar(hasLine);
    XBindVar(curvature);
    XBindVar(enhancedVideo);
}

void XLineFinder::init() {
    /*
     * Set default parameters
     */
    blurRange = 5;
    autoHsvValueRangeEnabled = true; //whether auto-thresholding is enabled
    autoHsvValueRangeRatio = 0.4; // what part of min-max value difference is taken into value range
    autoHsvValueRangeFreq = 30; // how often is autoadjusting done (every n-th frame)
    hsvValueRange = 120;
    cannyT1 = 200;
    cannyT2 = 200;
    cannyApertureSize = 3;
    cannyL2Gradient = false;
    houghRho = 4;
    houghTheta = CV_PI / 180;
    houghT = 70;
    houghMinLength = 100;
    houghMaxGap = 40;
    hystDeviationThr = M_PI / 3; // angle in radians for hysteresis filtering
    hystDistanceThr = 0.2; // distance in relative units for hysteresis filtering
    curvatureTolerance = 2; // threshold factors for curvature estimation
    cameraParam = 0.78; // experimentally determined camera intrinsic
    /*
     * Output vars
     */
    distance = 0;
    deviation = 0;
    hasLine = isLineVisual(lineType_);

    USetUpdate(REFRESH_PERIOD);
}

void XLineFinder::onChangeVideo(::urbi::UImage frame) {
    lastProcessedFrameNo_++;

    if (frame.height != lastFrame_.rows || frame.width != lastFrame_.cols){
        lastFrame_.create(frame.height, frame.width, CV_8UC3);
    }
    //cv::Mat imageCV(image.height, image.width, CV_8UC3, image.data);
    memcpy(lastFrame_.data, frame.data, frame.height*frame.width * 3);
    /*
    * Image Processing
    */
    cv::Mat mid;

    // adapt to different size
    lineUtils_.setDimensions(lastFrame_.cols, lastFrame_.rows);
    lineUtils_.updateReferencePoint(rotation_.theta, rotation_.phi, cameraParam);

    /*
    * 1. Denoise
    */
    cv::GaussianBlur(lastFrame_, mid, cv::Size(static_cast<int> (blurRange), static_cast<int> (blurRange)), 0, 0);

    /*
    * 2. Differentiate line pixels from others
    */
    cv::cvtColor(mid, mid, CV_BGR2HSV);

    if (lastProcessedFrameNo_ % static_cast<int> (autoHsvValueRangeFreq) == 0) {
        adjustValueRange(mid); // adjusting on blurred and HSV image
    }

    // highlight ROI (region of interest)
    cv::inRange(mid, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, static_cast<double> (hsvValueRange)), mid);

    // transform to edge map
    cv::Canny(mid, mid, static_cast<double> (cannyT1), static_cast<double> (cannyT2), static_cast<int> (cannyApertureSize), static_cast<double> (cannyL2Gradient));
    /*
    * 3. Detect lines
    */
    cv::vector<LineUtils::RawLineType> lines;
    cv::HoughLinesP(mid, lines, static_cast<double> (houghRho), static_cast<double> (houghTheta), static_cast<int> (houghT), static_cast<double> (houghMinLength), static_cast<double> (houghMaxGap));

    /*
    * 4. Filter out outlying lines
    */
    cv::vector<LineUtils::RawLineType> filteredLines;
    cv::vector<LineUtils::RawLineType> curvatureLines;
    LineUtils::RawLineType avg;
    bool hasAvg(false);

    if (lines.size() == 0) { // no line detected
        lineType_ = LINE_NONE;
    }
    else {
        useOnlyGoodLines(lines, filteredLines, curvatureLines);

        if (filteredLines.size() == 0) {
            lineType_ = LINE_NONE;
        }
        else {
            lineType_ = LINE_VISUAL;
            /*
            * Find new dominant line
            */
            avg = cv::mean(filteredLines);
            hasAvg = true;
            LineUtils::normalizeOrientation(avg);

            distance_ = LineUtils::pointLineDistance(lineUtils_.referencePoint, avg) / lineUtils_.distanceUnit;
            deviation_ = LineUtils::lineDirection(avg);

            curvature_ = calculateCurvature(avg, curvatureLines, mid);
            hystDeviationThr = static_cast<double>(hystDeviationThrBase) + abs(curvature_) * static_cast<double>(hystDeviationThrRate); // TODO this may need smoothing
        }

    }


    /*
    * 5. Draw output
    */
    
    drawDebugLines(lastFrame_, lines, filteredLines);
    urbi::UImage bin;
    bin.width = lastFrame_.cols;
    bin.height = lastFrame_.rows;
    bin.size = lastFrame_.rows * lastFrame_.cols * 3;
    bin.imageFormat = urbi::IMAGE_RGB;
    bin.data = lastFrame_.data;
    enhancedVideo = bin;

    /*
    * 6. Notify output
    */
    curvature = curvature_;
    distance = distance_;
    deviation = deviation_;
    hasLine = isLineVisual(lineType_); // must be last as it notifies other UObjects
}

void XLineFinder::adjustValueRange(cv::Mat hsvImage) {
    if (!static_cast<bool> (autoHsvValueRangeEnabled)) {
        return;
    }

    cv::Mat hue(hsvImage.rows, hsvImage.cols, CV_8UC1);
    cv::Mat saturation(hsvImage.rows, hsvImage.cols, CV_8UC1);
    cv::Mat value(hsvImage.rows, hsvImage.cols, CV_8UC1);
    cv::Mat splitted[] = {hue, saturation, value};
    cv::split(hsvImage, splitted);

    double minVal(0), maxVal(0);
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(value, &minVal, &maxVal, &minLoc, &maxLoc);
    double diff = maxVal - minVal;

    hsvValueRange = static_cast<int> (minVal + diff * static_cast<double> (autoHsvValueRangeRatio));
}

void XLineFinder::useOnlyGoodLines(cv::vector<LineUtils::RawLineType> lines, cv::vector<LineUtils::RawLineType> &goodLines, cv::vector<LineUtils::RawLineType> &curvatureLines) {

    cv::Point deltaPoint(lineUtils_.getDeltaPoint(expectedDistance_, expectedDeviation_));

    for (auto lineCandidate : lines) {
        LineUtils::normalizeOrientation(lineCandidate);

        cv::Point center = LineUtils::lineCenter(lineCandidate);
        // check line validity
        double devDiff = expectedDeviation_ - LineUtils::lineDirection(lineCandidate);
        /* Orientation is ignored */
        devDiff = abs(devDiff);
        devDiff = fmod(devDiff, M_PI);

        double distDiff(LineUtils::pointLineDistance2(center, expectedDeviation_, deltaPoint));
        distDiff = abs(distDiff);
        distDiff /= lineUtils_.distanceUnit; // normalize for relative units

        // factors are currently unused
        auto devFactor = 1;
        auto distFactor = 1;
        if (devFactor * devDiff <= static_cast<double> (hystDeviationThr) && distFactor * distDiff <= static_cast<double> (hystDistanceThr)) {
            goodLines.push_back(lineCandidate);
            curvatureLines.push_back(lineCandidate);
        }
        auto toleranceFactor = static_cast<double> (curvatureTolerance); // factor is applied to angle only
        if (devDiff <= toleranceFactor * static_cast<double> (hystDeviationThr) && distDiff <= static_cast<double> (hystDistanceThr)) {
            curvatureLines.push_back(lineCandidate);
        }
    }
}

double XLineFinder::calculateCurvature(xcs::nodes::line_finder::LineUtils::RawLineType meanLine, cv::vector<xcs::nodes::line_finder::LineUtils::RawLineType>& curvatureLines, cv::Mat image) {
    cv::Point meanLineCenter(LineUtils::lineCenter(meanLine));

    /*
     * Change and transform line coordinates
     */
    cv::vector<cv::Point> centerPoints;
    cv::vector<double> deviations;
    for (auto line : curvatureLines) {
        auto center = LineUtils::lineCenter(line);
        // translate
        center = center - meanLineCenter;
        // rotate
        auto rotation = -deviation_;
        cv::Point centerTr(center.x * cos(rotation) - center.y * sin(rotation), center.x * sin(rotation) + center.y * cos(rotation));
        centerPoints.push_back(centerTr);

        auto deviation = LineUtils::lineDirection(line);
        deviation = deviation - deviation_;
        deviations.push_back(deviation);
    }

    /*
     * Sum all curvature contributions
     */
    auto l = LineUtils::lineLength(meanLine) / 2;
    double curvature = 0;
    for (size_t i = 0; i < centerPoints.size(); ++i) {
        auto center = centerPoints[i];
        auto deviation = deviations[i];
        auto dbgCenter = LineUtils::lineCenter(curvatureLines[i]);

        if (abs(center.y) < l) {
            // TODO remove cv::putText(image, "IN", dbgCenter, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 0));
            continue;
        }
        if (sgn(deviation) * sgn(-center.x * center.y) <= 0) {
            //TODO remove cv::putText(image, "BAD", dbgCenter, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 0));
            continue;
        }


        auto curvaturePart = deviation / hypot(center.x, center.y);
        curvaturePart *= sgn(center.y);
        // TODO remove cv::putText(image, to_string(dbgCurv), dbgCenter, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 0));
        curvature += curvaturePart;
    }
    return curvature;
}

void XLineFinder::drawDebugLines(cv::Mat& canvas, const cv::vector<xcs::nodes::line_finder::LineUtils::RawLineType>& lines, const cv::vector<xcs::nodes::line_finder::LineUtils::RawLineType>& filteredLines) {

    // expected line
    auto distRange = static_cast<double> (hystDistanceThr);
    auto devRange = static_cast<double> (hystDeviationThr);
    auto expectedDeltaPoint = lineUtils_.getDeltaPoint(expectedDistance_, expectedDeviation_);

    drawFullLine(canvas, expectedDistance_, expectedDeviation_, cv::Scalar(128, 0, 128), 2);

    // expected distance range
    drawFullLine(canvas, expectedDistance_ + distRange, expectedDeviation_, cv::Scalar(128, 0, 128), 1, false);
    drawFullLine(canvas, expectedDistance_ - distRange, expectedDeviation_, cv::Scalar(128, 0, 128), 1, false);

    // expected deviation range
    double devLength = 100; // px        
    cv::Point posVector(devLength * sin(expectedDeviation_ + devRange), -devLength * cos(expectedDeviation_ + devRange));
    cv::Point negVector(devLength * sin(expectedDeviation_ - devRange), -devLength * cos(expectedDeviation_ - devRange));

    cv::line(canvas, expectedDeltaPoint, expectedDeltaPoint + posVector, cv::Scalar(128, 0, 128), 1);
    cv::line(canvas, expectedDeltaPoint, expectedDeltaPoint + negVector, cv::Scalar(128, 0, 128), 1);

    if (isLineVisual(lineType_)) {
        // followed line
        drawFullLine(canvas, distance_, deviation_, cv::Scalar(0, 0, 255));
    }

    // detected lines are last (top layer)    
    for (auto line : lines) {
        cv::line(canvas, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), cv::Scalar(255, 200, 200), 2);
    }
    for (auto line : filteredLines) {
        cv::line(canvas, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), cv::Scalar(255, 200, 200), 2);
    }

    // reference point
    cv::circle(canvas, lineUtils_.referencePoint, 5, cv::Scalar(255, 255, 255));

    // image center
    cv::circle(canvas, lineUtils_.imageCenter, 3, cv::Scalar(128, 128, 128));

}

/*!
* Very ugly geometry.
*/
void XLineFinder::drawFullLine(cv::Mat& canvas, double distance, double deviation, cv::Scalar color, size_t width, bool withCircle) {
    if (abs(cos(deviation)) < 1e-6) { // TODO general solution for skewed lines would be better
        cv::Point leftPoint(0, lineUtils_.referencePoint.y + distance * lineUtils_.distanceUnit);
        cv::Point rightPoint(lineUtils_.width, lineUtils_.referencePoint.y + distance * lineUtils_.distanceUnit);
        cv::line(canvas, leftPoint, rightPoint, color, width);
    }
    else {
        auto horizOffset = distance * lineUtils_.distanceUnit / cos(deviation);
        cv::Point bottomPoint(lineUtils_.referencePoint.x + horizOffset - tan(deviation) * (static_cast<int> (lineUtils_.height) - lineUtils_.referencePoint.y), lineUtils_.height);
        cv::Point topPoint(lineUtils_.referencePoint.x + horizOffset + tan(deviation) * lineUtils_.referencePoint.y, 0);
        cv::line(canvas, bottomPoint, topPoint, color, width);

        // mark orientation (circle is head)
        if (cos(deviation) > 0) {
            cv::circle(canvas, topPoint, width + 2, color);
        }
        else {
            cv::circle(canvas, bottomPoint, width + 2, color);
        }
    }

    if (withCircle && abs(distance) < 100) { // this prevents fail in OpenCV for very large doubles (which become negative numbers)
        // intersection circle
        cv::Point deltaPoint = lineUtils_.getDeltaPoint(distance, deviation);
        cv::circle(canvas, deltaPoint, width + 3, color);

        // distance circle
        cv::Scalar distanceColor((distance > 0) ? cv::Scalar(0, 255, 255) : cv::Scalar(0, 255, 0));
        cv::circle(canvas, lineUtils_.referencePoint, abs(distance) * lineUtils_.distanceUnit, distanceColor);
    }

}

XStart(XLineFinder);
