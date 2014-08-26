#ifndef RED_DOT_H
#define RED_DOT_H

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace xcs{
namespace nodes{
namespace reddot{

    struct ImagePosition{
        int x;
        int y;

        ImagePosition(const int x, const int y) : x(x), y(y) {};
    };

    class RedDot{
        void getRedMask(const cv::Mat &image, cv::Mat &redMask);
    public:
        ImagePosition findRedDot(const cv::Mat &image, cv::Mat &out);
    };

}}}

#endif