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
        int lowerH_;
        int upperH_;
        int lowerS_;
        int upperS_;
        int lowerV_;
        int upperV_;

        void getMask(const cv::Mat &image, cv::Mat &redMask);
    public:
        void lowerH(const int value){ if(value < upperH_) lowerH_ = value; };
        void upperH(const int value){ if(value > lowerH_) upperH_ = value; };
        void lowerS(const int value){ if (value < upperS_) lowerS_ = value; };
        void upperS(const int value){ if (value > lowerS_) upperS_ = value; };
        void lowerV(const int value){ if (value < upperV_) lowerV_ = value; };
        void upperV(const int value){ if (value > lowerS_) upperV_ = value; };
        bool findRedDot(const cv::Mat &image, cv::Mat &out);
        ImagePosition lastPosition(){ return lastPosition_; };
    };

}}}

#endif