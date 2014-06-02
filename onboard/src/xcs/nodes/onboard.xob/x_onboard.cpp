#include "x_onboard.hpp"
#include <xcs/nodes/xobject/x.h>
#include <boost/log/trivial.hpp>

using namespace xcs::nodes;
using namespace std;

namespace xcs {
namespace nodes {


XOnboard::XOnboard(const std::string &name) :
  xcs::nodes::XObject(name),
  changeMode("MODE"),
  mode("MODE"),
  connection_(0) {

    mode_ = "scripting";

    // TODO: unused so far
    

    XBindVar(mode);

    XBindFunction(XOnboard, init);
    XBindVarF(changeMode, &XOnboard::onChangeMode);
}

void XOnboard::init() {
    // default mode so far, TODO: change to free-flight and run urbiscripts from web GUI
    mode = "scriptin";
}

void XOnboard::onChangeMode(const string &newMode) {
    string oldMode = mode_;
    cerr << "mode_ = " << mode_ << endl;
    this->send("Onboard.stop_" + oldMode + "();");
    this->send("Onboard.start_" + newMode + "();");
    cerr << "[onboard] Mode changed from " << oldMode << " to " << newMode << endl;
    this->mode = newMode;
    mode_ = newMode;
}

}
}

XStart(XOnboard);