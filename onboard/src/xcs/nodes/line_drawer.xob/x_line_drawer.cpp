#include "x_line_drawer.hpp"
#include <cmath>
#include <mutex>

using namespace xcs;
using namespace xcs::nodes;
using namespace std;

XLineDrawer::XLineDrawer(const string& name) :
  XObject(name),
  rotation("ROTATION"),
  hasFrame_(false) {
    UBindFunction(XLineDrawer, init);
    UBindFunctionRename(XLineDrawer, drawFullLineU, "drawFullLine"); //TODO better casting of cv::Scalar

    /*
     * Inputs
     */
    UBindVar(XLineDrawer, video);
    UNotifyChange(video, &XLineDrawer::onChangeVideo);

    XBindVar(rotation);

    /*
     * Parameters
     */
    UBindVar(XLineDrawer, cameraParam);
    UBindVar(XLineDrawer, fps);
    UNotifyChange(fps, &XLineDrawer::onChangeFps);

}

void XLineDrawer::init() {
    cv::namedWindow("Lines", cv::WINDOW_AUTOSIZE);

    onChangeFps(10); // default FPS
}

int XLineDrawer::update() {
    const static size_t waitDelay = 10;
    if (!hasFrame_) {
        return 0;
    }
    /*
     * This workaround copies image data.
     * http://stackoverflow.com/a/11354422/1351874
     */
    cv::Mat src(lastFrame_.height, lastFrame_.width, CV_8UC3, lastFrame_.data);
    CvMat tmp = src;
    src = cv::Mat(&tmp, true);

    {
        lock_guard<mutex> lock(drawTasksMtx_);
        for (auto drawTask : drawTasks_) {
            switch (drawTask.type) {
                case TASK_LINE:
                    cv::line(src, drawTask.point1, drawTask.point2, drawTask.color, drawTask.dimension1);
                    break;
                case TASK_CIRCLE:
                    cv::circle(src, drawTask.point1, drawTask.dimension1, drawTask.color, 3);
                    break;
            }
        }
        drawTasks_.clear();
    }

    cv::imshow("Lines", src);
    cv::waitKey(waitDelay); // re-render image windows

    return 0; // Urbi undocumented, return value probably doesn't matter
}

void XLineDrawer::onChangeVideo(::urbi::UVar& uvar) {
    lastFrame_ = uvar;
    hasFrame_ = true;
    lineUtils_.setDimensions(lastFrame_.width, lastFrame_.height);
    auto rotationVector = static_cast<EulerianVector>(rotation.data()); // TODO use directoy operator on XVar
    lineUtils_.updateReferencePoint(rotationVector.theta, rotationVector.phi, cameraParam);
}

void XLineDrawer::onChangeFps(int value) {
    if (value <= 0) {
        USetUpdate(-1); // disabled timer
    } else {
        USetUpdate(1000 / value);
    }
}

void XLineDrawer::drawLine(cv::Point begin, cv::Point end, cv::Scalar color, size_t width) {
    lock_guard<mutex> lock(drawTasksMtx_);
    DrawTask task;
    task.point1 = begin;
    task.point2 = end;
    task.color = color;
    task.dimension1 = width;
    task.type = TASK_LINE;
    drawTasks_.push_back(task);
}

void XLineDrawer::drawCircle(cv::Point center, cv::Scalar color, size_t radius) {
    lock_guard<mutex> lock(drawTasksMtx_);
    DrawTask task;
    task.point1 = center;
    task.color = color;
    task.dimension1 = radius;
    task.type = TASK_CIRCLE;
    drawTasks_.push_back(task);
}

/*!
 * Very ugly geometry.
 */
void XLineDrawer::drawFullLine(double distance, double deviation, cv::Scalar color, size_t width, bool withCircle) {
    if (abs(cos(deviation)) < 1e-6) { // TODO general solution for skewed lines would be better
        cv::Point leftPoint(0, lineUtils_.referencePoint.y + distance * lineUtils_.distanceUnit);
        cv::Point rightPoint(lineUtils_.width, lineUtils_.referencePoint.y + distance * lineUtils_.distanceUnit);
        drawLine(leftPoint, rightPoint, color, width);
    } else {
        auto horizOffset = distance * lineUtils_.distanceUnit / cos(deviation);
        cv::Point bottomPoint(lineUtils_.referencePoint.x + horizOffset - tan(deviation) * (static_cast<int> (lineUtils_.height) - lineUtils_.referencePoint.y), lineUtils_.height);
        cv::Point topPoint(lineUtils_.referencePoint.x + horizOffset + tan(deviation) * lineUtils_.referencePoint.y, 0);
        drawLine(bottomPoint, topPoint, color, width);

        // mark orientation (circle is head)
        if (cos(deviation) > 0) {
            drawCircle(topPoint, color, width + 2);
        } else {
            drawCircle(bottomPoint, color, width + 2);
        }
    }

    if (withCircle && abs(distance) < 100) { // this prevents fail in OpenCV for very large doubles (which become negative numbers)
        // intersection circle
        cv::Point deltaPoint = lineUtils_.getDeltaPoint(distance, deviation);
        drawCircle(deltaPoint, color, width + 3);

        // distance circle
        cv::Scalar distanceColor((distance > 0) ? cv::Scalar(0, 255, 255) : cv::Scalar(0, 255, 0));
        drawCircle(lineUtils_.referencePoint, distanceColor, abs(distance) * lineUtils_.distanceUnit);
    }

}

void XLineDrawer::drawFullLineU(double distance, double deviation, size_t color, size_t width, bool withCircle) {
    cv::Scalar cvColor;
    switch (color) {
        case 1:
            cvColor = cv::Scalar(255, 128, 0);
            break;
        case 2:
            cvColor = cv::Scalar(255, 0, 128);
            break;
        case 3:
            cvColor = cv::Scalar(0, 128, 255);
            break;
        default:
            cvColor = cv::Scalar(0, 0, 0);
            break;
    }
    drawFullLine(distance, deviation, cvColor, width, withCircle);
}

void XLineDrawer::drawLine(xcs::nodes::line_finder::LineUtils::RawLineType line, cv::Scalar color, size_t width) {
    drawLine(cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), color, width);
}


UStart(XLineDrawer);
