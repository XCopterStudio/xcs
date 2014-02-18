/* 
 * File:   UJson.hpp
 * Author: michal
 *
 * Created on February, 2014, 13:01 PM
 */

#ifndef UJSON_HPP
#define	UJSON_HPP

#include <urbi/uobject.hh>
#include <boost/unordered_map.hpp>
#include <rapidjson/document.h>
#include <memory>


namespace xcs {
namespace urbi {

class UJson : public ::urbi::UObject {
public:

    UJson(const std::string &name);


    ::urbi::UValue decode(const std::string &json);

    std::string encode(const ::urbi::UValue &data);

    virtual ~UJson();
private:
    std::unique_ptr<rapidjson::Value> fromUValue(const ::urbi::UValue & uvalue, rapidjson::Document::AllocatorType &allocator);
};

}
}

#endif	/* UXCI_HPP */

