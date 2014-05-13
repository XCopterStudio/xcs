#include "u_line_finder.hpp"
#include <thread>
#include <chrono>
#include <cmath>
#include <xcs/xcs_fce.hpp>

using namespace xcs::urbi;
using namespace xcs::urbi::line_finder;
using namespace std;

const size_t ULineFinder::REFRESH_PERIOD = 66; // ms
const size_t ULineFinder::STUCK_TOLERANCE = 2; // timer periods

ULineFinder::ULineFinder(const string& name) :
  ::urbi::UObject(name),
  lineDrawer_(nullptr),
  hasFrame_(false),
  lastReceivedFrameNo_(1), // must be greater than lastProcessedFrame_ at the beginning
  lastProcessedFrameNo_(0),
  stuckCounter_(0),
  distance_(0),
  deviation_(0),
  lineType_(LINE_NONE) {
    UBindFunction(ULineFinder, init);

    UBindVar(ULineFinder, video);
    UNotifyChange(video, &ULineFinder::onChangeVideo);

    UBindVar(ULineFinder, theta);
    UBindVar(ULineFinder, phi);
    UBindVar(ULineFinder, cameraParam);
    UBindVarRename(ULineFinder, expectedDistanceUVar, "expectedDistance");
    UBindVarRename(ULineFinder, expectedDeviationUVar, "expectedDeviation");


    UBindVar(ULineFinder, blurRange);
    UBindVar(ULineFinder, autoHsvValueRangeEnabled);
    UBindVar(ULineFinder, autoHsvValueRangeRatio);
    UBindVar(ULineFinder, autoHsvValueRangeFreq);
    UBindVar(ULineFinder, hsvValueRange);
    UBindVar(ULineFinder, cannyT1);
    UBindVar(ULineFinder, cannyT2);
    UBindVar(ULineFinder, cannyApertureSize);
    UBindVar(ULineFinder, cannyL2Gradient);
    UBindVar(ULineFinder, houghRho);
    UBindVar(ULineFinder, houghTheta);
    UBindVar(ULineFinder, houghT);
    UBindVar(ULineFinder, houghMinLength);
    UBindVar(ULineFinder, houghMaxGap);
    UBindVar(ULineFinder, hystDeviationThr);
    UBindVar(ULineFinder, hystDistanceThr);
    UBindVar(ULineFinder, curvatureTolerance);


    UBindVarRename(ULineFinder, curvatureUVar, "curvature");
    UBindVarRename(ULineFinder, distanceUVar, "distance");
    UBindVarRename(ULineFinder, deviationUVar, "deviation");
    UBindVar(ULineFinder, hasLine);

    UBindFunction(ULineFinder, setLineDrawer);
}

void ULineFinder::init() {
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
    distanceUVar = 0;
    deviationUVar = 0;
    hasLine = isLineVisual(lineType_);

    cv::namedWindow("HSV->InRange", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Canny", cv::WINDOW_AUTOSIZE);

    USetUpdate(REFRESH_PERIOD);
}

int ULineFinder::update() {
    //TODO fallback to lost line when video is stucked
    if (!hasFrame_ || lastProcessedFrameNo_ >= lastReceivedFrameNo_) {
        if (++stuckCounter_ > STUCK_TOLERANCE) {
            lineType_ = LINE_NONE;
            hasLine = false;
        }
        return 0;
    }
    lastProcessedFrameNo_ = lastReceivedFrameNo_;

    processFrame();

    const static size_t waitDelay = 10;
    cv::waitKey(waitDelay); // re-render image windows

    return 0; // Urbi undocumented, return value probably doesn't matter
}

void ULineFinder::setLineDrawer(UObject *drawer) {
    //lineDrawer_ = dynamic_cast<ULineDrawer *>(drawer);
    lineDrawer_ = (ULineDrawer *) (drawer); //TODO missing typeinfo when linking
}

void ULineFinder::onChangeVideo(::urbi::UVar& uvar) {
    lastFrame_ = uvar;
    hasFrame_ = true;
    lastReceivedFrameNo_ += 1;
    stuckCounter_ = 0;
    // adapt to different size
    lineUtils_.setDimensions(lastFrame_.width, lastFrame_.height);
    lineUtils_.updateReferencePoint(theta, phi, cameraParam);
}

void ULineFinder::adjustValueRange(cv::Mat hsvImage) {
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

void ULineFinder::processFrame() {
    /*
     * Image Processing
     */
    cv::Mat src(lastFrame_.height, lastFrame_.width, CV_8UC3, lastFrame_.data);
    cv::Mat mid;

    /*
     * 1. Denoise
     */
    cv::GaussianBlur(src, mid, cv::Size(static_cast<int> (blurRange), static_cast<int> (blurRange)), 0, 0);

    /*
     * 2. Differentiate line pixels from others
     */
    cv::cvtColor(mid, mid, CV_BGR2HSV);

    if (lastProcessedFrameNo_ % static_cast<int> (autoHsvValueRangeFreq) == 0) {
        adjustValueRange(mid); // adjusting on blurred and HSV image
    }

    // highlight ROI (region of interest)
    cv::inRange(mid, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, static_cast<double> (hsvValueRange)), mid);
    cv::imshow("HSV->InRange", mid);
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
    } else {
        useOnlyGoodLines(lines, filteredLines, curvatureLines);

        if (filteredLines.size() == 0) {
            lineType_ = LINE_NONE;
        } else {
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
        }

    }
    cv::imshow("Canny", mid);

    /*
     * 5. Draw output
     */
    drawDebugLines(lines, filteredLines);

    /*
     * 6. Notify output
     */
    curvatureUVar = curvature_;
    distanceUVar = distance_;
    deviationUVar = deviation_;
    hasLine = isLineVisual(lineType_); // must be last as it notifies other UObjects
}

void ULineFinder::useOnlyGoodLines(cv::vector<LineUtils::RawLineType> lines, cv::vector<LineUtils::RawLineType> &goodLines, cv::vector<LineUtils::RawLineType> &curvatureLines) {
    auto expectedDistance = static_cast<double> (expectedDistanceUVar);
    auto expectedDeviation = static_cast<double> (expectedDeviationUVar);


    cv::Point deltaPoint(lineUtils_.getDeltaPoint(expectedDistance, expectedDeviation));

    for (auto lineCandidate : lines) {
        LineUtils::normalizeOrientation(lineCandidate);

        cv::Point center = LineUtils::lineCenter(lineCandidate);
        // check line validity
        double devDiff = expectedDeviation - LineUtils::lineDirection(lineCandidate);
        /* Orientation is ignored */
        devDiff = abs(devDiff);
        devDiff = fmod(devDiff, M_PI);

        double distDiff(LineUtils::pointLineDistance2(center, expectedDeviation, deltaPoint));
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

double ULineFinder::calculateCurvature(xcs::urbi::line_finder::LineUtils::RawLineType meanLine, cv::vector<xcs::urbi::line_finder::LineUtils::RawLineType>& curvatureLines, cv::Mat image) {
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

void ULineFinder::drawDebugLines(const cv::vector<xcs::urbi::line_finder::LineUtils::RawLineType>& lines, const cv::vector<xcs::urbi::line_finder::LineUtils::RawLineType>& filteredLines) {

    // expected line
    auto expectedDistance = static_cast<double> (expectedDistanceUVar);
    auto expectedDeviation = static_cast<double> (expectedDeviationUVar);
    auto distRange = static_cast<double> (hystDistanceThr);
    auto devRange = static_cast<double> (hystDeviationThr);
    auto expectedDeltaPoint = lineUtils_.getDeltaPoint(expectedDistance, expectedDeviation);

    lineDrawer_->drawFullLine(expectedDistance, expectedDeviation, cv::Scalar(128, 0, 128), 2);

    // expected distance range
    lineDrawer_->drawFullLine(expectedDistance + distRange, expectedDeviation, cv::Scalar(128, 0, 128), 1, false);
    lineDrawer_->drawFullLine(expectedDistance - distRange, expectedDeviation, cv::Scalar(128, 0, 128), 1, false);

    // expected deviation range
    double devLength = 100; // px        
    cv::Point posVector(devLength * sin(expectedDeviation + devRange), -devLength * cos(expectedDeviation + devRange));
    cv::Point negVector(devLength * sin(expectedDeviation - devRange), -devLength * cos(expectedDeviation - devRange));

    lineDrawer_->drawLine(expectedDeltaPoint, expectedDeltaPoint + posVector, cv::Scalar(128, 0, 128), 1);
    lineDrawer_->drawLine(expectedDeltaPoint, expectedDeltaPoint + negVector, cv::Scalar(128, 0, 128), 1);

    if (isLineVisual(lineType_)) {
        // followed line
        lineDrawer_->drawFullLine(distance_, deviation_, cv::Scalar(0, 0, 255));
    }

    // detected lines are last (top layer)    
    for (auto l : lines) {
        lineDrawer_->drawLine(l, cv::Scalar(255, 200, 200), 2);
    }
    for (auto l : filteredLines) {
        lineDrawer_->drawLine(l, cv::Scalar(255, 0, 0), 2);
    }

    // reference point
    lineDrawer_->drawCircle(lineUtils_.referencePoint, cv::Scalar(255, 255, 255), 5);

    // image center
    lineDrawer_->drawCircle(lineUtils_.imageCenter, cv::Scalar(128, 128, 128), 3);


}

UStart(ULineFinder);
