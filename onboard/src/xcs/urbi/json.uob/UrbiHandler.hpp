/* 
 * File:   UrbiHandler.hpp
 * Author: michal
 *
 * Created on February 17, 2014, 10:14 PM
 */

#ifndef URBIHANDLER_HPP
#define	URBIHANDLER_HPP

#include <urbi/uvalue.hh>
#include <rapidjson/rapidjson.h>
#include <stack>
#include <cinttypes>

namespace xcs {
namespace urbi {
namespace json {

class UrbiHandler {
public:
    UrbiHandler();

    //@name Implementation of Handler (see rapidjson library)
    //@{

    UrbiHandler& Null();

    UrbiHandler& Bool(bool b);

    UrbiHandler& Int(int i);

    UrbiHandler& Uint(unsigned u);

    UrbiHandler& Int64(int64_t i64);

    UrbiHandler& Uint64(uint64_t u64);

    UrbiHandler& Double(double d);

    UrbiHandler& String(const char* str, unsigned int length, bool copy = false);

    UrbiHandler& StartObject();

    UrbiHandler& EndObject(unsigned int memberCount = 0);

    UrbiHandler& StartArray();

    UrbiHandler& EndArray(unsigned int elementCount = 0);
    //@}


    ::urbi::UValue getResult();

private:

    enum TypeEnum {
        TYPE_SCALAR,
        TYPE_ARRAY,
        TYPE_OBJECT
    };
    
    typedef boost::optional<std::string> OptionalKey;

    void pushUValue(const ::urbi::UValue &uvalue);

    std::stack<TypeEnum> currentType_;

    std::stack<OptionalKey> objectKey_;
    
    std::stack<::urbi::UValue> currentUValue_;

    ::urbi::UValue result_;
};

}
}
}

#endif	/* URBIHANDLER_HPP */

