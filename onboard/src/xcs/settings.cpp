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