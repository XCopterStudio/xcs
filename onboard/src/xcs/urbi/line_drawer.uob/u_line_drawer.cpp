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

const size_t ULineDrawer::REFRESH_PERIOD = 100; // ms

ULineDrawer::ULineDrawer(const string& name) :
  ::urbi::UObject(name),
  hasFrame_(false) {
    UBindFunction(ULineDrawer, init);
    UBindFunctionRename(ULineDrawer, drawFullLineU, "drawFullLine"); //TODO better casting of cv::Scalar
    

    UBindVar(ULineDrawer, video);
    UNotifyChange(video, &ULineDrawer::onChangeVideo);

    UBindVar(ULineDrawer, theta);
    UBindVar(ULineDrawer, phi);
    UBindVar(ULineDrawer, cameraParam);

}

void ULineDrawer::init() {
    cv::namedWindow("Lines", cv::WINDOW_AUTOSIZE);

    USetUpdate(REFRESH_PERIOD);
}

int ULineDrawer::update() {
    const static size_t waitDelay = 10;
    if (!hasFrame_) {
        return 0;
    }

    cv::Mat src(lastFrame_.height, lastFrame_.width, CV_8UC3, lastFrame_.data);
    {
        lock_guard<mutex> lock(drawTasksMtx_);
        for (auto drawTask : drawTasks_) {
            switch (drawTask.type) {
                case TASK_LINE:
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
void ULineDrawer::drawFullLine(double distance, double deviation, cv::Scalar color, size_t width) {
    cv::Point deltaPoint = lineUtils_.getDeltaPoint(distance, deviation);
    cv::Point bottomPoint(deltaPoint.x - tan(deviation) * (0.5 * lineUtils_.height - distance * sin(deviation) * lineUtils_.distanceUnit), lineUtils_.height);
    cv::Point topPoint(deltaPoint.x + tan(deviation) * (0.5 * lineUtils_.height + distance * sin(deviation) * lineUtils_.distanceUnit), 0);

    drawLine(bottomPoint, topPoint, color, width);
    drawCircle(deltaPoint, color, width + 3);

    //    if (abs(deviation) > M_PI / 2) {
    //        cv::circle(image, bottomPoint, width + 2, color, width, CV_AA);
    //    } else {
    //        cv::circle(image, topPoint, width + 2, color, width, CV_AA);
    //    }
}

void ULineDrawer::drawFullLineU(double distance, double deviation, size_t color, size_t width) {
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
    drawFullLine(distance, deviation, cvColor, width);
}

void ULineDrawer::drawLine(xcs::urbi::line_finder::LineUtils::RawLineType line, cv::Scalar color, size_t width) {
    drawLine(cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), color, width);
}


UStart(ULineDrawer);
