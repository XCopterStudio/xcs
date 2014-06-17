#include "x_json.hpp"
#include <xcs/nodes/xobject/x.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "urbi_handler.hpp"

using namespace std;
using namespace rapidjson;

namespace xcs {
namespace nodes {

XJson::XJson(const std::string& name) : 
    xcs::nodes::XObject(name),
    outJson("JSON"),
    outRest("REST") {
    
    XBindVar(outJson);
    XBindVar(outRest);

    XBindFunction(XJson, encode);
    XBindFunction(XJson, decode);
    XBindFunction(XJson, tryDecode);
}

XJson::~XJson() {
}

string XJson::encode(const ::urbi::UValue& data) {
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

::urbi::UValue XJson::decode(const std::string &json) {
    Document doc;
    doc.Parse<0>(json.c_str());
    if (doc.HasParseError()) {
        send("throw \"Cannot parse JSON.\";");
        // would return nil UValue
    }

    UrbiHandler handler;
    doc.Accept(handler);
    return handler.getResult();
}

/**
Attempts to decode JSON from the given data./
(Try cut end of given json)

\param [in]     json    The JSON.
\param [out]    outJson The out JSON.
\param [out]    outRest The out rest.

\return    true if it succeeds, false if it fails.
*/
bool XJson::tryDecode(const std::string &json) {
    // init output value
    outRest = "";

    string jsonText = json;
    vector<string> rests;
    Document doc;
    string text, rest;
    unsigned foundPosition;

    // try find valid part of json
    do {
        foundPosition = jsonText.rfind('}');
        if (foundPosition == string::npos) {
            text = jsonText;
            jsonText = "";
        }
        else {
            text = jsonText.substr(0, foundPosition + 1);
            rest = trimRestJson(jsonText.substr(foundPosition + 1, jsonText.length()));
            if (rest != "") {
                rests.push_back(rest);
                jsonText = text;
            }
            else {
                jsonText = "";
            }
        }

        text = "[" + text + "]";

        doc.Parse<0>(text.c_str());
    } while (doc.HasParseError() && jsonText != ""); // try it while not succeed

    // validate result
    if (doc.HasParseError()) {
        return false;
    }

    // create out rest
    stringstream ssRest;
    for (vector<string>::const_iterator it = rests.cend(); it != rests.cbegin(); --it) {
        ssRest << *it;
    }
    outRest = ssRest.str();

    // create out json
    UrbiHandler handler;
    doc.Accept(handler);
    outJson = handler.getResult();

    return true;
}

string& XJson::trimRestJson(string &restJson) const {
    restJson.erase(restJson.begin(), std::find_if(restJson.begin(), restJson.end(), bind1st(mem_fun(&XJson::isNotSpaceOrComma), this)));
    restJson.erase(std::find_if(restJson.rbegin(), restJson.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), restJson.end());
    return restJson;
}

bool XJson::isNotSpaceOrComma(char c) const {
    return !(std::isspace(c) || c == ',');
}

unique_ptr<Value> XJson::fromUValue(const ::urbi::UValue & uvalue, rapidjson::Document::AllocatorType &allocator) {
    unique_ptr<Value> result(new Value());
    switch (uvalue.type) {
        case ::urbi::DATA_DOUBLE:
            (*result).SetDouble(uvalue.val);
            break;
        case ::urbi::DATA_STRING:
            (*result).SetString(uvalue.stringValue->c_str(), uvalue.stringValue->size(), allocator);
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
                (*result).AddMember(Value().SetString(pair.first.c_str(), pair.first.size(), allocator), *mappedItemPtr, allocator);
            }
            break;
        default:
            throw runtime_error("Unsupported type of UValue for JSON serialization.");
            break;
    }
    return result;
}

XStart(XJson);

}
}