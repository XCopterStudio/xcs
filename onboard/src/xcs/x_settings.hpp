#ifndef X_SETTINGS_HPP
#define X_SETTINGS_HPP

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <string>
#include <xcs/xcs_export.h>

namespace xcs {

class XCS_EXPORT XSettings {
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
    boost::property_tree::ptree& getTree();
    bool isInit() const;
    bool save();
private:
    boost::property_tree::ptree settings_;
    std::string filename_;
};

template<class Type>
Type XSettings::get(const std::string& path, const Type& defaultValue) const {
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
}

#endif // X_SETTINGS_HPP