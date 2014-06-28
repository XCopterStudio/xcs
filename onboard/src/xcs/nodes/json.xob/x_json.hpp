#ifndef XJSON_HPP
#define	XJSON_HPP

#include <boost/unordered_map.hpp>
#include <rapidjson/document.h>
#include <memory>
#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_var.hpp>

namespace xcs {
namespace nodes {

class XJson : public xcs::nodes::XObject {
public:
    XJson(const std::string &name);
    virtual ~XJson();
    ::urbi::UValue decode(const std::string &json);
    bool tryDecode(const std::string &json);
    std::string encode(const ::urbi::UValue &data);

    //TODO: code 2 one xvar
    XVar<::urbi::UValue> outJson;
    XVar<std::string> outRest;    
private:
    std::unique_ptr<rapidjson::Value> fromUValue(const ::urbi::UValue & uvalue, rapidjson::Document::AllocatorType &allocator);
    void trimRestJson(std::string &restJson) const;
    bool isNotSpaceOrComma(char c) const;
};

}
}

#endif	/* XXCI_HPP */