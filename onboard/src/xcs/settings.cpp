#include <boost/property_tree/info_parser.hpp>
#include "settings.hpp"

using namespace std;
using namespace xcs;

Settings::Settings(const string& filename) :
    filename_(filename) {
    reset();
}

Settings::~Settings() {
}

void Settings::reset() {
    if (isInit()) {
        boost::property_tree::info_parser::read_info(filename_, settings_);
    }
}

void Settings::reset(const string& filename) {
    filename_ = filename;
    reset();
}

bool Settings::save() {
    if (isInit()) {
        boost::property_tree::info_parser::write_info(filename_, settings_);
        return true;
    }

    return false;            
}

boost::property_tree::ptree& Settings::getTree() {
    return settings_;
}

bool Settings::isInit() const {
    return !filename_.empty();
}

bool Settings::contains(const string& key) const {
    //return settings_.count(key) > 0;
    //return settings_.find(key) != settings_.not_found();
    
    //TODO: use somethig faster then exception?
    try  {
        settings_.get_child(key);
        return true;
    }
    catch (boost::property_tree::ptree_bad_path ex) {
        return false;
    }
}