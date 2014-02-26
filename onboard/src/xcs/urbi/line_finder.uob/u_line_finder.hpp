#ifndef U_LINE_FINDER_HPP
#define U_LINE_FINDER_HPP

#include <urbi/uobject.hh>

namespace xcs {
namespace urbi {

class ULineFinder : public ::urbi::UObject {
public:

    ::urbi::InputPort video;

    ULineFinder(const std::string &);
    void init();

private:
    void onChangeVideo(::urbi::UVar &uvar);
};

}
}

#endif // U_LINE_FINDER_HPP
