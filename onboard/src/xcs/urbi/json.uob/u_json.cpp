#include "u_json.hpp"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "urbi_handler.hpp"

using namespace xcs::urbi;
using namespace xcs::urbi::json;
using namespace std;
using namespace rapidjson;

UJson::UJson(const std::string& name) : ::urbi::UObject(name) {
    UBindFunction(UJson, encode);
    UBindFunction(UJson, decode);    
}

UJson::~UJson() {

}

string UJson::encode(const ::urbi::UValue& data) {
    if (data.type != ::urbi::DATA_DICTIONARY && data.type != ::urbi::DATA_LIST) {
        send("throw \"Only UList or UDictionary are supported by UJson.\";");
        return "";
    }
    Document doc;
    StringBuffer stream;

    Writer<StringBuffer> writer(stream);

    unique_ptr<Value> valPtr(fromUValue(data, doc.GetAllocator()));
    (*valPtr).Accept(writer);

    string result(stream.GetString());
    stream.Clear();
    return result;
}

::urbi::UValue UJson::decode(const std::string &json) {
    Document doc;
    doc.Parse<0>(json.c_str());
    if(doc.HasParseError()) {
        send("throw \"Cannot parse JSON.\";");
        // would return nil UValue
    }

    UrbiHandler handler;
    doc.Accept(handler);
    return handler.getResult();
}

unique_ptr<Value> UJson::fromUValue(const ::urbi::UValue & uvalue, rapidjson::Document::AllocatorType &allocator) {
    unique_ptr<Value> result(new Value());
    switch (uvalue.type) {
        case ::urbi::DATA_DOUBLE:
            (*result).SetDouble(uvalue.val);
            break;
        case ::urbi::DATA_STRING:
            (*result).SetString(uvalue.stringValue->c_str(), uvalue.stringValue->size());
            break;
        case ::urbi::DATA_LIST:
            (*result).SetArray();
            for (auto itemPtr : *uvalue.list) {
                unique_ptr<Value> valuePtr(fromUValue(*itemPtr, allocator));
                (*result).PushBack(*valuePtr, allocator);
            }
            break;
        case ::urbi::DATA_DICTIONARY:
            (*result).SetObject();
            for (auto pair : *uvalue.dictionary) {
                unique_ptr<Value> mappedItemPtr(fromUValue(pair.second, allocator));
                (*result).AddMember(pair.first.c_str(), *mappedItemPtr, allocator);
            }
            break;
        default:
            throw runtime_error("Unsupported type of UValue for JSON serialization.");
            break;
    }
    return result;
}


UStart(UJson);