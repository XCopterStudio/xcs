#ifndef U_LINE_FINDER_HPP
#define U_LINE_FINDER_HPP

#include <vector>
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

    std::vector<int> getLine();
    std::vector<int> getViewCenter();
    int getImageWidth();
    int getImageHeight();

private:
    void onChangeVideo(::urbi::UVar &uvar);

    std::vector<int> line_;
    std::vector<int> center_;
    int imageHeight_;
    int imageWidth_;
    double prevDist_;
};

}
}

#endif // U_LINE_FINDER_HPP
