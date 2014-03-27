#include "u_line_drawer.hpp"
#include <cmath>
#include <mutex>

using namespace xcs::urbi;
using namespace std;

ULineDrawer::DrawTask::DrawTask() : type(TASK_VOID) {
}

ULineDrawer::DrawTask::DrawTask(const DrawTask &drawTask) {
    this->type = drawTask.type;
    switch (drawTask.type) {
        case TASK_CIRCLE:
            this->dataCircle = drawTask.dataCircle;
            break;
        case TASK_LINE:
            this->dataLine = drawTask.dataLine;
            break;
        default:
            this->type = TASK_VOID;
            break;
    }
}

ULineDrawer::ULineDrawer(const string& name) :
  ::urbi::UObject(name),
  hasFrame_(false) {
    UBindFunction(ULineDrawer, init);
    UBindFunctionRename(ULineDrawer, drawFullLineU, "drawFullLine"); //TODO better casting of cv::Scalar

    /*
     * Inputs
     */
    UBindVar(ULineDrawer, video);
    UNotifyChange(video, &ULineDrawer::onChangeVideo);

    UBindVar(ULineDrawer, theta);
    UBindVar(ULineDrawer, phi);

    /*
     * Parameters
     */
    UBindVar(ULineDrawer, cameraParam);
    UBindVar(ULineDrawer, fps);
    UNotifyChange(fps, &ULineDrawer::onChangeFps);

}

void ULineDrawer::init() {
    cv::namedWindow("Lines", cv::WINDOW_AUTOSIZE);

    onChangeFps(10); // default FPS
}

int ULineDrawer::update() {
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
                    //cerr << drawTask.dataLine.begin << ", " << drawTask.dataLine.end << ", " << drawTask.dataLine.color << ", " << drawTask.dataLine.width << endl;
                    cv::line(src, drawTask.dataLine.begin, drawTask.dataLine.end, drawTask.dataLine.color, drawTask.dataLine.width);
                    break;
                case TASK_CIRCLE:
                    cv::circle(src, drawTask.dataCircle.center, drawTask.dataCircle.radius, drawTask.dataCircle.color, 3);
                    break;
            }
        }
        drawTasks_.clear();
    }

    cv::imshow("Lines", src);
    cv::waitKey(waitDelay); // re-render image windows

    return 0; // Urbi undocumented, return value probably doesn't matter
}

void ULineDrawer::onChangeVideo(::urbi::UVar& uvar) {
    lastFrame_ = uvar;
    hasFrame_ = true;
    lineUtils_.setDimensions(lastFrame_.width, lastFrame_.height);
    lineUtils_.updateReferencePoint(theta, phi, cameraParam);
}

void ULineDrawer::onChangeFps(int value) {
    if (value <= 0) {
        USetUpdate(-1); // disabled timer
    } else {
        USetUpdate(1000 / value);
    }
}

void ULineDrawer::drawLine(cv::Point begin, cv::Point end, cv::Scalar color, size_t width) {
    lock_guard<mutex> lock(drawTasksMtx_);
    DrawTask task;
    task.dataLine.begin = begin;
    task.dataLine.end = end;
    task.dataLine.color = color;
    task.dataLine.width = width;
    task.type = TASK_LINE;
    drawTasks_.push_back(task);
}

void ULineDrawer::drawCircle(cv::Point center, cv::Scalar color, size_t radius) {
    lock_guard<mutex> lock(drawTasksMtx_);
    DrawTask task;
    task.dataCircle.center = center;
    task.dataCircle.color = color;
    task.dataCircle.radius = radius;
    task.type = TASK_CIRCLE;
    drawTasks_.push_back(task);
}

/*!
 * Very ugly geometry.
 */
void ULineDrawer::drawFullLine(double distance, double deviation, cv::Scalar color, size_t width, bool withCircle) {
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

void ULineDrawer::drawFullLineU(double distance, double deviation, size_t color, size_t width, bool withCircle) {
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

void ULineDrawer::drawLine(xcs::urbi::line_finder::LineUtils::RawLineType line, cv::Scalar color, size_t width) {
    drawLine(cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), color, width);
}


UStart(ULineDrawer);
