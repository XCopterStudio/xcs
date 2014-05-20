#ifndef X_SETTINGS_HPP
#define X_SETTINGS_HPP

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <string>
#include <xcs/xcs_export.h>
//#include <xcs/nodes/xobject/xobject_export.h>

namespace xcs {

class /*XOBJECT_EXPORT*/ XCS_EXPORT XSettings {
public:
    XSettings(const std::string& filename = "");
    ~XSettings();
    void reset();
    void reset(const std::string& filename);
    template<class Type = std::string>
    Type get(const std::string& path, const Type& defaultValue) const;
    template<class Type = std::string>
    Type get(const std::string& path) const;
    template<class Type = std::string>
    void set(const std::string& path, const Type& value);
    bool save();
    boost::property_tree::ptree& getTree();
    bool isInit() const;
private:
    boost::property_tree::ptree settings_;
    std::string filename_;
};

template<class Type>
Type XSettings::get(const std::string& path, const Type& defaultValue) const {
    cerr << "SOUBOR:::::" << filename_ << endl;
    return settings_.get<Type>(path, defaultValue);
}

template<class Type>
Type XSettings::get(const std::string& path) const {
    return settings_.get<Type>(path);
}

template<class Type = std::string>
void XSettings::set(const std::string& path, const Type& value) {
    settings_.put(path, value);
}


//inline XSettings::XSettings(const std::string& filename) :
//    filename_(filename) {
//    reset();
//}
//
//inline XSettings::~XSettings() {
//}
//
//inline void XSettings::reset() {
//    boost::property_tree::info_parser::read_info(filename_, settings_);
//}
//
//inline void XSettings::reset(const std::string& filename) {
//    filename_ = filename;
//    reset();
//}
//
//inline bool XSettings::save() {
//    if (isInit()) {
//        boost::property_tree::info_parser::write_info(filename_, settings_);
//        return true;
//    }
//
//    return false;
//}
//
//inline boost::property_tree::ptree& XSettings::getTree() {
//    return settings_;
//}
//
//inline bool XSettings::isInit() const {
//    return !settings_.empty();
//}

}

#endif // X_SETTINGS_HPP