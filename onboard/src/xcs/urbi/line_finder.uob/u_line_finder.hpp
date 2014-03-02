#ifndef U_LINE_FINDER_HPP
#define U_LINE_FINDER_HPP

#include <urbi/uobject.hh>

namespace xcs {
namespace urbi {

class ULineFinder : public ::urbi::UObject {
public:

    ::urbi::InputPort video;

    ::urbi::UVar blurRange;
    ::urbi::UVar hsvValueRange;
    ::urbi::UVar cannyT1;
    ::urbi::UVar cannyT2;
    ::urbi::UVar cannyApertureSize;
    ::urbi::UVar cannyL2Gradient;
    ::urbi::UVar houghRho;
    ::urbi::UVar houghTheta;
    ::urbi::UVar houghT;
    ::urbi::UVar houghMinLength;
    ::urbi::UVar houghMaxGap;

    ULineFinder(const std::string &);
    void init();

private:
    void onChangeVideo(::urbi::UVar &uvar);
    void onChangeBlurRange(int range);
    void onChangeHsvValueRange(double range);
    void onChangeCannyT1(double treshhold);
    void onChangeCannyT2(double treshhold);
    void onChangeCannyApertureSize(int size);
    void onChangeCannyL2Gradient(bool hasGradient);
    void onChangeHoughRho(double rho);
    void onChangeHoughTheta(double theta);
    void onChangeHoughT(int treshhold);
    void onChangeHoughMinLength(double length);
    void onChangeHoughMaxGap(double length);

    int blurRange_;
    double hsvValueRange_;
    double cannyT1_;
    double cannyT2_;
    int cannyApertureSize_;
    bool cannyL2Gradient_;
    double houghRho_;
    double houghTheta_;
    int houghT_;
    double houghMinLength_;
    double houghMaxGap_;
};

}
}

#endif // U_LINE_FINDER_HPP
