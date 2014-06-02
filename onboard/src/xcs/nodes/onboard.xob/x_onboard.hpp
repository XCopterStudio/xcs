#ifndef ONBOARD_HPP
#define ONBOARD_HPP

#include <map>
#include <string>
#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_var.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>

namespace xcs {
namespace nodes {

class XOnboard : public xcs::nodes::XObject {
public:

    enum Mode {
        UNDEFINED = 0,
        FREE_FLIGHT = 1,
        ASSISTED_FLIGHT = 2,
        SCRIPTING = 3
    };

    enum Connection {
        DISCONNECTED = 0,
        CONNECTED = 1
    };

    XInputPort<std::string> changeMode;
    XVar<std::string> mode;

    XOnboard(const std::string &name);

    void init();

private:
    void onChangeMode(const std::string &oldMode);

    std::map<std::string, Mode> modes_;
    std::string mode_;
    int connection_;
};

}
}

#endif // ONBOARD_HPP
