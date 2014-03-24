#include "u_line_finder.hpp"
#include <thread>
#include <chrono>
#include <cmath>

using namespace xcs::urbi;
using namespace xcs::urbi::line_finder;
using namespace std;

const size_t ULineFinder::REFRESH_PERIOD = 66; // ms
const size_t ULineFinder::STUCK_TOLERANCE = 2; // timer periods


const double ULineFinder::DEFAULT_DEVIATION = 0;
const double ULineFinder::DEFAULT_DISTANCE = 0;

ULineFinder::ULineFinder(const string& name) :
  ::urbi::UObject(name),
  lineDrawer_(nullptr),
  hasFrame_(false),
  lastReceivedFrameNo_(1), // must be greater than lastProcessedFrame_ at the beginning
  lastProcessedFrameNo_(0),
  stuckCounter_(0),
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
    UBindVar(ULineFinder, distanceAging);
    UBindVar(ULineFinder, deviationAging);
    UBindVar(ULineFinder, hystDirThreshold);
    UBindVar(ULineFinder, hystCenterThreshold);
    UBindVar(ULineFinder, hystForgetRatio);
    UBindVar(ULineFinder, hystForgetThreshold);
    UBindVar(ULineFinder, hystForgetDerRatio);
    UBindVar(ULineFinder, hystForgetDerThreshold);


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
    distanceAging = 0.05; // how much previous distance is taken into account
    deviationAging = 0.2; // how much previous deviation is taken into account
    hystDirThreshold = M_PI / 3; // angle in radians for hysteresis filtering
    hystCenterThreshold = 0.2; // distance in relative units for hysteresis filtering
    hystForgetRatio = 0.9; // forgetting factor (1 = no forgetting factor, 0 = no remembering)
    hystForgetThreshold = 0.1; // threshold of forgotten line
    hystForgetDerRatio = 0.5; // derivative forgetting factor (1 = no forgetting factor, 0 = no remembering; used for scaling)
    hystForgetDerThreshold = 0.1; // derivative forgetting threshold (1 = no forgetting factor, 0 = no remembering; used "zeroing" the derivative)
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
            lineType_ = LINE_REMEMBERED;
            hasLine = false;
        }
        return 0;
    }
    lastProcessedFrameNo_ = lastReceivedFrameNo_;

    processFrame();

    const static size_t waitDelay = 20;
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
    cv::vector<LineUtils::RawLineType> lines;
    cv::HoughLinesP(mid, lines, static_cast<double> (houghRho), static_cast<double> (houghTheta), static_cast<int> (houghT), static_cast<double> (houghMinLength), static_cast<double> (houghMaxGap));

    /*
     * 3.5 Filter out outlying lines
     */

    cv::vector<LineUtils::RawLineType> filteredLines;
    LineUtils::RawLineType avg;
    bool hasAvg(false);
    calculateExpectedLine();

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
                filteredLines = useOnlyGoodLines(lines);
                //filteredLines = lines; // without previous line assume all lines are correct
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
            LineUtils::normalizeOrientation(avg);


            double newDistance(LineUtils::pointLineDistance(lineUtils_.referencePoint, avg) / lineUtils_.distanceUnit);
            double newDeviation = LineUtils::lineDirection(avg);

            /*
             * Calculate derivatives
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
        //lineDrawer_->drawPoint(cv::Point(avg[2], avg[3]), cv::Scalar(128, 128, 255)); //TODO is it necessary
        lineDrawer_->drawLine(avg, cv::Scalar(128, 128, 255), 3);
    }

    if (isLineVisual(lineType_)) {
        // distance circle
        cv::Scalar distanceColor((distance_ > 0) ? cv::Scalar(0, 255, 255) : cv::Scalar(0, 255, 0));
        lineDrawer_->drawCircle(lineUtils_.referencePoint, distanceColor, abs(distance_) * lineUtils_.distanceUnit);

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
        lineDrawer_->drawFullLine(expectedDistance_, expectedDeviation_, cv::Scalar(128, 0, 128), 2);

        // followed line
        lineDrawer_->drawFullLine(distance_, deviation_, hystColor);

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

    /*
     * Set output
     */
    deviationUVar = deviation_;
    distanceUVar = distance_;
    hasLine = isLineVisual(lineType_); // must be last as it notifies other UObjects
}

void ULineFinder::useRememberedLine() {
    switch (lineType_) { // dispatch on previous line type
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
                lineType_ = LINE_NONE; //TODO any more settings here?
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

cv::vector<LineUtils::RawLineType> ULineFinder::useOnlyGoodLines(cv::vector<LineUtils::RawLineType> lines) {
    auto expectedDistance = expectedDistance_;
    auto expectedDeviation = expectedDeviation_;
    cv::vector<LineUtils::RawLineType> result;

    cv::Point deltaPoint(lineUtils_.distanceUnit * expectedDistance * cos(expectedDeviation), lineUtils_.distanceUnit * expectedDistance * sin(expectedDeviation));
    deltaPoint += lineUtils_.referencePoint;

    for (auto lineCandidate : lines) {
        LineUtils::normalizeOrientation(lineCandidate);


        cv::Point center((lineCandidate[0] + lineCandidate[2]) / 2, (lineCandidate[3] + lineCandidate[1]) / 2);
        // check line validity
        double dirDiff = expectedDeviation - LineUtils::lineDirection(lineCandidate);
        dirDiff = abs(dirDiff);

        double centerDiff(LineUtils::pointLineDistance2(center, expectedDeviation, deltaPoint));
        centerDiff = abs(centerDiff);
        centerDiff /= lineUtils_.distanceUnit; // normalize for relative units

        // hystStrength effectively broadens search region
        auto dirFactor = (lineType_ == LINE_NONE) ? 0 :
                (lineType_ == LINE_VISUAL) ? 1 :
                0.8; // TODO another parameter -- tolerance for nonvisual detection

        auto centerFactor = (lineType_ == LINE_VISUAL) ? 1 : hystStrength_;
        if (dirFactor * dirDiff <= static_cast<double> (hystDirThreshold) && centerFactor * centerDiff <= static_cast<double> (hystCenterThreshold)) {
            result.push_back(lineCandidate);
        }
    }

    return result;

}

void ULineFinder::calculateExpectedLine() {
    switch (lineType_) {
        case LINE_NONE:
            expectedDistance_ = static_cast<double>(expectedDistanceUVar);
            expectedDeviation_ = static_cast<double>(expectedDeviationUVar);
            break;
        default:
            auto factor = static_cast<double> (hystForgetDerRatio);
            auto threshold = static_cast<double> (hystForgetDerThreshold);
            if (hystDerStrength_ *= factor < threshold) {
                distanceDer_ = 0;
                deviationDer_ = 0;
            }

            expectedDistance_ = distance_ + distanceDer_ * REFRESH_PERIOD * hystDerStrength_;
            expectedDeviation_ = deviation_ + deviationDer_ * REFRESH_PERIOD* hystDerStrength_;
            break;
    }
}

UStart(ULineFinder);
