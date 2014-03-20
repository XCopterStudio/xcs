#include "u_line_finder.hpp"
#include <thread>
#include <chrono>
#include <cmath>

using namespace xcs::urbi;
using namespace std;

const size_t ULineFinder::REFRESH_PERIOD = 100; // ms


const double ULineFinder::DEFAULT_DEVIATION = 0;
const double ULineFinder::DEFAULT_DISTANCE = 0;

ULineFinder::ULineFinder(const string& name) :
  ::urbi::UObject(name),
  hasFrame_(false),
  lastReceivedFrameNo_(1), // must be greater than lastProcessedFrame_ at the beginning
  lastProcessedFrameNo_(0),
  distanceUnit_(1), // must be non-zero value
  distance_(DEFAULT_DISTANCE),
  deviation_(DEFAULT_DEVIATION),
  distanceDer_(0),
  deviationDer_(0),
  expectedDistance_(DEFAULT_DISTANCE),
  expectedDeviation_(DEFAULT_DEVIATION),
  lineType_(LINE_NONE),
  hystStrength_(0),
  hystDerStrength_(0) {
    UBindFunction(ULineFinder, init);

    UBindVar(ULineFinder, video);
    UNotifyChange(video, &ULineFinder::onChangeVideo);
    
    UBindVar(ULineFinder, theta);
    UBindVar(ULineFinder, phi);

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
    UBindVar(ULineFinder, distanceAging);
    UBindVar(ULineFinder, deviationAging);
    UBindVar(ULineFinder, hystDirThreshold);
    UBindVar(ULineFinder, hystCenterThreshold);
    UBindVar(ULineFinder, hystForgetRatio);
    UBindVar(ULineFinder, hystForgetThreshold);
    UBindVar(ULineFinder, hystForgetDerRatio);
    UBindVar(ULineFinder, hystForgetDerThreshold);
    
    UBindVar(ULineFinder, cameraParam);    

    UBindVarRename(ULineFinder, distanceUVar, "distance");
    UBindVarRename(ULineFinder, deviationUVar, "deviation");
    UBindVar(ULineFinder, hasLine);
}

void ULineFinder::init() {
    cout << "Initing";
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
    distanceAging = 0.05; // how much previous distance is taken into account
    deviationAging = 0.2; // how much previous deviation is taken into account
    hystDirThreshold = M_PI / 3; // angle in radians for hysteresis filtering
    hystCenterThreshold = 0.2; // distance in relative units for hysteresis filtering
    hystForgetRatio = 0.9; // forgetting factor (1 = no forgetting factor, 0 = no remembering)
    hystForgetThreshold = 0.1; // threshold of forgotten line
    hystForgetDerRatio = 0.5; // derivative forgetting factor (1 = no forgetting factor, 0 = no remembering; used for scaling)
    hystForgetDerThreshold = 0.1; // derivative forgetting threshold (1 = no forgetting factor, 0 = no remembering; used "zeroing" the derivative)
    cameraParam = 0.78;    // experimentally determined camera intrinsic
    /*
     * Output vars
     */
    distanceUVar = 0;
    deviationUVar = 0;
    hasLine = lineType_ != LINE_NONE;

    cv::namedWindow("HSV->InRange", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Canny", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Lines", cv::WINDOW_AUTOSIZE);

    USetUpdate(REFRESH_PERIOD);
}

int ULineFinder::update() {
    const static size_t waitDelay = 20;
    processFrame();
    cv::waitKey(waitDelay); // re-render image windows

    return 0; // Urbi undocumented, return value probably doesn't matter
}

void ULineFinder::onChangeVideo(::urbi::UVar& uvar) {
    lastFrame_ = uvar;
    hasFrame_ = true;
    lastReceivedFrameNo_ += 1;
    // adapt to different size
    if ((imageHeight_ != lastFrame_.height) || (imageWidth_ != lastFrame_.width)) {
        imageHeight_ = lastFrame_.height;
        imageWidth_ = lastFrame_.width;
        distanceUnit_ = hypot(lastFrame_.height, lastFrame_.width);
        imageCenter_ = cv::Point(lastFrame_.width / 2, lastFrame_.height / 2);
    }

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
    if (!hasFrame_ || lastProcessedFrameNo_ >= lastReceivedFrameNo_) {
        return;
    }
    lastProcessedFrameNo_ = lastReceivedFrameNo_;
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
    // convert to HSV
    cv::cvtColor(mid, mid, CV_BGR2HSV);

    if (lastProcessedFrameNo_ % static_cast<int> (autoHsvValueRangeFreq) == 0) {
        adjustValueRange(mid); // adjusting on blurred and HSV image
    }

    // highlight ROI (region of interest)
    cv::inRange(mid, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, static_cast<double> (hsvValueRange)), mid);
    cv::imshow("HSV->InRange", mid);
    // transform to edge map
    cv::Canny(mid, mid, static_cast<double> (cannyT1), static_cast<double> (cannyT2), static_cast<int> (cannyApertureSize), static_cast<double> (cannyL2Gradient));
    cv::imshow("Canny", mid);
    /*
     * 3. Detect lines
     */
    cv::vector<RawLineType> lines;
    cv::HoughLinesP(mid, lines, static_cast<double> (houghRho), static_cast<double> (houghTheta), static_cast<int> (houghT), static_cast<double> (houghMinLength), static_cast<double> (houghMaxGap));

    /*
     * 3.5 Filter out outlying lines
     */

    cv::vector<RawLineType> filteredLines;
    RawLineType avg;
    bool hasAvg(false);
    calculateExpectedLine();
    calculateReferencePoint();

    if (lines.size() == 0) { // no line detected
        useRememberedLine();
    } else {
        switch (lineType_) {
            case LINE_VISUAL:
            case LINE_REMEMBERED:
                filteredLines = useOnlyGoodLines(lines);
                // line type is unchanged
                break;
            case LINE_NONE:
                filteredLines = lines; // without previous line assume all lines are correct
                lineType_ = LINE_VISUAL;
                break;
            default:
                throw runtime_error("Unknown line type.");
                break;
        }

        if (filteredLines.size() == 0) {
            useRememberedLine();
        } else {
            lineType_ = LINE_VISUAL;
            /*
             * Find new dominant line
             */
            avg = cv::mean(filteredLines);
            hasAvg = true;
            normalizeOrientation(avg);


            double newDistance(pointLineDistance(referencePoint_, avg) / distanceUnit_);
            double newDeviation = lineDirection(avg); // TODO check orientation and shift

            /*
             * Calculate derivations
             */
            distanceDer_ = (newDistance - distance_) / REFRESH_PERIOD;
            deviationDer_ = (newDeviation - deviation_) / REFRESH_PERIOD;
            hystDerStrength_ = 1;

            /*
             * Combine line with previous data
             */
            double distAging(static_cast<double> (distanceAging));
            distance_ = distAging * distance_ + (1 - distAging) * newDistance;

            double devAging(static_cast<double> (deviationAging));
            deviation_ = devAging * deviation_ + (1 - devAging) * newDeviation;
        }

    }

    /*
     * Draw output
     */
    // mean line
    if (cv::countNonZero(avg)) {
        cv::line(src, cv::Point(avg[0], avg[1]), cv::Point(avg[2], avg[3]), cv::Scalar(128, 128, 255), 3, CV_AA);
        cv::circle(src, cv::Point(avg[2], avg[3]), 5, cv::Scalar(128, 128, 255), 3, CV_AA);
    }

    if (lineType_ != LINE_NONE) {
        // distance circle
        cv::Scalar distanceColor((distance_ > 0) ? cv::Scalar(0, 255, 255) : cv::Scalar(0, 255, 0));
        cv::circle(src, referencePoint_, abs(distance_) * distanceUnit_, distanceColor, 3, CV_AA);

        // central deviation line
        cv::Point heading(referencePoint_.x + 50 * sin(deviation_), referencePoint_.y - 50 * cos(deviation_));
        cv::line(src, referencePoint_, heading, cv::Scalar(128, 128, 128), 1, CV_AA);

        // followed line

        double forgetThr = static_cast<double> (hystForgetThreshold);
        double strength = (hystStrength_ <= forgetThr) ? 0 : (hystStrength_ - forgetThr) / (1 - forgetThr);
        cv::Scalar hystColor;
        if (lineType_ == LINE_VISUAL) {
            hystColor = cv::Scalar(0, 0, 255);
        } else {
            hystColor = cv::Scalar(0, 128, 255);
            hystColor *= strength;
        }
        // expected line
        drawFullLine(src, expectedDistance_, expectedDeviation_, cv::Scalar(128, 0, 128), 2);

        // followed line
        drawFullLine(src, distance_, deviation_, hystColor);
    }

    // detected lines are last (top layer)    
    for (auto l : lines) {
        cv::line(src, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 200, 200), 2, CV_AA);
    }
    for (auto l : filteredLines) {
        cv::line(src, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 0, 0), 2, CV_AA);
    }

    // reference point
    cv::circle(src, referencePoint_, 5, cv::Scalar(255, 255, 255), 3, CV_AA);


    /*
     * Set output
     */
    hasLine = lineType_ != LINE_NONE;
    deviationUVar = deviation_;
    distanceUVar = distance_;

    /*
     * Display annotated frame
     */
    cv::imshow("Lines", src);
}

void ULineFinder::useRememberedLine() {
    switch (lineType_) {
        case LINE_NONE:// when nothing present even death rejects (how to write this in English?)
            distance_ = DEFAULT_DISTANCE;
            deviation_ = DEFAULT_DEVIATION;
            break;
        case LINE_VISUAL:
            hystStrength_ = 1; // fresh memory is 100%
            lineType_ = LINE_REMEMBERED;
            // use expected position
            distance_ = expectedDistance_;
            deviation_ = expectedDeviation_;
            break;
        case LINE_REMEMBERED:
            hystStrength_ *= static_cast<double> (hystForgetRatio);
            if (hystStrength_ <= static_cast<double> (hystForgetThreshold)) { // we've forgotten the line
                hystStrength_ = 0;
                lineType_ = LINE_NONE;
            } else {// use expected position
                distance_ = expectedDistance_;
                deviation_ = expectedDeviation_;
            }
            break;
        default:
            throw runtime_error("Unknown line type.");
            break;
    }
}

cv::vector<ULineFinder::RawLineType> ULineFinder::useOnlyGoodLines(cv::vector<ULineFinder::RawLineType> lines) {
    auto expectedDistance = expectedDistance_;
    auto expectedDeviation = expectedDeviation_;
    cv::vector<ULineFinder::RawLineType> result;

    cv::Point deltaPoint(distanceUnit_ * expectedDistance * cos(expectedDeviation), distanceUnit_ * expectedDistance * sin(expectedDeviation));
    deltaPoint += referencePoint_;

    for (auto lineCandidate : lines) {
        normalizeOrientation(lineCandidate);


        cv::Point center((lineCandidate[0] + lineCandidate[2]) / 2, (lineCandidate[3] + lineCandidate[1]) / 2);
        // check line validity
        double dirDiff = expectedDeviation - lineDirection(lineCandidate);
        dirDiff = abs(dirDiff);

        double centerDiff(pointLineDistance2(center, expectedDeviation, deltaPoint));
        centerDiff = abs(centerDiff);
        centerDiff /= distanceUnit_; // normalize for relative units

        // hystStrength effectively broadens search region
        auto factor = (lineType_ == LINE_VISUAL) ? 1 : hystStrength_;
        if (factor * dirDiff <= static_cast<double> (hystDirThreshold) && factor * centerDiff <= static_cast<double> (hystCenterThreshold)) {
            result.push_back(lineCandidate);
        }
    }

    return result;

}

void ULineFinder::calculateExpectedLine() {
    auto factor = static_cast<double> (hystForgetDerRatio);
    auto threshold = static_cast<double> (hystForgetDerThreshold);
    if (hystDerStrength_ *= factor < threshold) {
        distanceDer_ = 0;
        deviationDer_ = 0;
    }

    expectedDistance_ = distance_ + distanceDer_ * REFRESH_PERIOD * hystDerStrength_;
    expectedDeviation_ = deviation_ + deviationDer_ * REFRESH_PERIOD* hystDerStrength_;
}

void ULineFinder::calculateReferencePoint() {
    auto tanTheta = tan(static_cast<double> (theta));
    auto tanPhi = tan(static_cast<double> (phi));
    auto camParam = static_cast<double> (cameraParam);

    auto refDistance = camParam * hypot(tanTheta, tanPhi);
    auto refDeviation = atan2(tanPhi, tanTheta);

    referencePoint_ = imageCenter_ + cv::Point(distanceUnit_ * refDistance * sin(refDeviation), distanceUnit_ * refDistance * cos(refDeviation));
}

UStart(ULineFinder);
