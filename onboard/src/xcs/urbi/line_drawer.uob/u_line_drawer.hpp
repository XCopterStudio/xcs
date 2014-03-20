#ifndef U_LINE_DRAWER_HPP
#define U_LINE_DRAWER_HPP

#include <vector>
#include <cstdint>
#include <mutex>
#include <urbi/uobject.hh>
#include <opencv2/opencv.hpp>
#include <xcs/urbi/line_finder.uob/line_utils.hpp>


namespace xcs {
namespace urbi {

class ULineDrawer : public ::urbi::UObject {
public:
    /*!
     * Inputs
     */
    ::urbi::InputPort video;
    ::urbi::UVar theta; // intentionally UVar
    ::urbi::UVar phi; // intentionally UVar

    /*
     * Params
     */
    ::urbi::UVar cameraParam;


    ULineDrawer(const std::string &);
    void init();

    /*!
     * Urbi period handler
     */
    virtual int update();


    void drawLine(cv::Point begin, cv::Point end, cv::Scalar color, size_t width);

    /*!
     * Very ugly geometry.
     */
    void drawFullLine(double distance, double deviation, cv::Scalar color, size_t width = 3);

    void drawLine(xcs::urbi::line_finder::LineUtils::RawLineType line, cv::Scalar color, size_t width = 2);

    void drawCircle(cv::Point center, cv::Scalar color, size_t radius = 3);

    void beginDrawTransaction();
    void endDrawTransaction();

private:

    struct TaskCircle {
        cv::Point center;
        cv::Scalar color;
        size_t radius;
    };

    struct TaskLine {
        cv::Point begin;
        cv::Point end;
        cv::Scalar color;
        size_t width;
    };

    enum DrawTaskType {
        TASK_VOID,
        TASK_CIRCLE,
        TASK_LINE
    };

    struct DrawTask {
        DrawTask();
        DrawTask(const DrawTask &drawTask);

        DrawTaskType type;

        union {
            TaskCircle dataCircle;
            TaskLine dataLine;
        };
    };

    static const size_t REFRESH_PERIOD;
    xcs::urbi::line_finder::LineUtils lineUtils_;
    void onChangeVideo(::urbi::UVar &uvar);
    ::urbi::UImage lastFrame_;
    bool hasFrame_;
    cv::Mat canvas_;

    std::vector<DrawTask> drawTasks_;
    std::mutex drawTasksMtx_;
    bool inTransaction_;

};

}
}

#endif // U_LINE_DRAWER_HPP
