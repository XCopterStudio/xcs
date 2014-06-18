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

void XSettings::init(const string& filepath, bool create) {
    settings_.reset(filepath, create);
}

string XSettings::get(const string& key) {
    return settings_.get(key);
}

string XSettings::getOrDefault(const string& key, const string& defaultValue) {
    return settings_.get(key, defaultValue);
}

boost::unordered_map<string, string> XSettings::getMap(const string& key) const {
    // get map
    map<string, string> m = settings_.getMap(key);
    
    // convert map 2 boost unordered map, which can be bind to urbiscript dictionary
    boost::unordered_map<string, string> um;
    for (const auto& kvp : m) {
        um.emplace(kvp.first, kvp.second);
    }

    return um;
}


void XSettings::set(const std::string& key, const std::string& value) {
    settings_.set(key, value);
}

void XSettings::setAndSave(const std::string& key, const std::string& value) {
    settings_.set(key, value);
    save();
}

bool XSettings::save() {
    return settings_.save();
}

bool XSettings::contains(const string& key) {
    return settings_.contains(key);
}

XStart(XSettings);
