#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <string>
#include <xcs/xcs_export.h>

namespace xcs {

class XCS_EXPORT Settings {
public:
    Settings(const std::string& filename = "");
    ~Settings();
    
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
    
    bool contains(const std::string& path) const;
private:
    boost::property_tree::ptree settings_;
    std::string filename_;
};

template<class Type>
Type Settings::get(const std::string& path, const Type& defaultValue) const {
    return settings_.get<Type>(path, defaultValue);
}

template<class Type>
Type Settings::get(const std::string& path) const {
    return settings_.get<Type>(path);
}

template<class Type>
void Settings::set(const std::string& path, const Type& value) {
    settings_.put(path, value);
}
}

#endif // SETTINGS_HPP