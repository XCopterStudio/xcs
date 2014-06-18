#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <map>
#include <xcs/xcs_export.h>

namespace xcs {

class XCS_EXPORT Settings {
public:
    Settings(const std::string& filename = "", bool create = false);
    ~Settings();
    void reset(bool create = false);
    void reset(const std::string& filename, bool create = false);
    template<class Type = std::string>
    Type get(const std::string& key, const Type& defaultValue) const;
    template<class Type = std::string>
    Type get(const std::string& key) const;
    
    template<class Type = std::string>
    std::map<std::string, Type> getMap(const std::string& key) const;
    template<class Type>
    void set(const std::string& key, const Type& value);
    boost::property_tree::ptree& getTree();
    
    bool isInit() const;
    bool save();
    bool contains(const std::string& key) const;
private:
    boost::property_tree::ptree settings_;
    std::string filename_;
};

template<class Type>
Type Settings::get(const std::string& key, const Type& defaultValue) const {
    return settings_.get<Type>(key, defaultValue);
}

template<class Type>
Type Settings::get(const std::string& key) const {
    return settings_.get<Type>(key);
}

template<class Type>
std::map<std::string, Type> Settings::getMap(const std::string& key) const {
    std::map<std::string, Type> m;

    // read all settings and fill the map
    if (contains(key)) {
        for (const auto& kvp : settings_.get_child(key)) {
            m.emplace(kvp.first, kvp.second.data());
        }
    }

    return m;
}

template<class Type>
void Settings::set(const std::string& key, const Type& value) {
    settings_.put(key, value);
}
}

#endif // SETTINGS_HPP