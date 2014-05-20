#include <boost/property_tree/info_parser.hpp>
#include "x_settings.hpp"

using namespace std;
using namespace xcs;

XSettings::XSettings(const string& filename) :
    filename_(filename) {
    reset();
}

XSettings::~XSettings() {
}

void XSettings::reset() {
    if (isInit()) {
        boost::property_tree::info_parser::read_info(filename_, settings_);
    }
}

void XSettings::reset(const string& filename) {
    filename_ = filename;
    reset();
}

bool XSettings::save() {
    if (isInit()) {
        boost::property_tree::info_parser::write_info(filename_, settings_);
        return true;
    }

    return false;            
}

boost::property_tree::ptree& XSettings::getTree() {
    return settings_;
}

bool XSettings::isInit() const {
    return !filename_.empty();
}