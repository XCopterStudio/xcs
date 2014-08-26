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

        ImagePosition(const int x=0, const int y=0) : x(x), y(y) {};
    };

    class RedDot{
        ImagePosition lastPosition_;
        void getRedMask(const cv::Mat &image, cv::Mat &redMask);
    public:
        bool findRedDot(const cv::Mat &image, cv::Mat &out);
        ImagePosition lastPosition(){ return lastPosition_; };
    };

}}}

#endif