#include "x_settings.hpp"
#include <xcs/nodes/xobject/x.h>

using namespace xcs::nodes;
using namespace std;

XSettings::XSettings(const std::string &name) :
    xcs::nodes::XObject(name) {
    XBindFunction(XSettings, init);
    XBindFunction(XSettings, get);
    XBindFunction(XSettings, getOrDefault);
    XBindFunction(XSettings, getMap);
    XBindFunction(XSettings, set);
    XBindFunction(XSettings, setAndSave);
    XBindFunction(XSettings, save);
    XBindFunction(XSettings, contains);
}

void XSettings::init(const string& filepath) {
    settings_.reset(filepath);
}

string XSettings::get(const string& path) {
    return settings_.get(path);
}

string XSettings::getOrDefault(const string& path, const string& defaultValue) {
    return settings_.get(path, defaultValue);
}

boost::unordered_map<string, string> XSettings::getMap(const string& path) const {
    // get map
    map<string, string> m = settings_.getMap(path);
    
    // convert map 2 boost unordered map, which can be bind to urbiscript dictionary
    boost::unordered_map<string, string> um;
    for (const auto& kvp : m) {
        um.emplace(kvp.first, kvp.second);
    }

    return um;
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

bool XSettings::contains(const string& path) {
    return settings_.contains(path);
}

XStart(XSettings);
