#include "x_settings.hpp"
#include <xcs/nodes/xobject/x.h>

using namespace xcs::nodes;
using namespace std;

namespace xcs {
namespace nodes {

XSettings::XSettings(const std::string &name) :
    xcs::nodes::XObject(name) {
    XBindFunction(XSettings, init);
    XBindFunction(XSettings, get);
    XBindFunction(XSettings, getOrDefault);
    XBindFunction(XSettings, set);
    XBindFunction(XSettings, setAndSave);
    XBindFunction(XSettings, save);
}

void XSettings::init(const string& filepath) {
    settings_.reset(filepath);
}

string XSettings::get(const string& path) {
    return settings_.get(path);
}

std::string XSettings::getOrDefault(const std::string& path, const std::string& defaultValue) {
    return settings_.get(path, defaultValue);
}

void XSettings::set(const std::string& path, const std::string& value) {
    settings_.set(path, value);
}

void XSettings::setAndSave(const std::string& path, const std::string& value) {
    settings_.set(path, value);
    save();
}

bool XSettings::save() {
    return settings_.save();
}

}
}

XStart(XSettings);
