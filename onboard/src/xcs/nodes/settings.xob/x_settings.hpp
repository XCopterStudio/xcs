#ifndef X_SETTINGS_HPP
#define X_SETTINGS_HPP

#include <string>
#include <boost/unordered_map.hpp>
#include <xcs/settings.hpp>
#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_var.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>

namespace xcs {
namespace nodes {

class XSettings : public xcs::nodes::XObject {
public:
    XSettings(const std::string &name);
    void init(const std::string& filepath, bool create);
    std::string get(const std::string& key);
    std::string getOrDefault(const std::string& key, const std::string& defaultValue);
    boost::unordered_map<std::string, std::string> getMap(const std::string& key) const;
    void set(const std::string& key, const std::string& value);
    void setAndSave(const std::string& key, const std::string& value);
    bool save();
    bool contains(const std::string& key);
private:
    xcs::Settings settings_;
};

}
}

#endif // X_SETTINGS_HPP
