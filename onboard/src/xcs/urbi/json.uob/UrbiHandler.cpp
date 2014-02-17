/* 
 * File:   UrbiHandler.cpp
 * Author: michal
 * 
 * Created on February 17, 2014, 10:14 PM
 */

#include "UrbiHandler.hpp"
#include <string>

using namespace xcs::urbi::json;
using namespace urbi;
using namespace std;

UrbiHandler::UrbiHandler() {
    currentType_.push(TYPE_SCALAR);
}

UrbiHandler& UrbiHandler::Null() {
    pushUValue(UValue());
    return *this;
}

UrbiHandler& UrbiHandler::Bool(bool b) {
    pushUValue(UValue(b));
    return *this;
}

UrbiHandler& UrbiHandler::Int(int i) {
    pushUValue(UValue(i));
    return *this;
}

UrbiHandler& UrbiHandler::Uint(unsigned u) {
    pushUValue(UValue(u));
    return *this;
}

UrbiHandler& UrbiHandler::Int64(int64_t i64) {
    pushUValue(UValue(i64));
    return *this;
}

UrbiHandler& UrbiHandler::Uint64(uint64_t u64) {
    pushUValue(UValue(u64));
    return *this;
}

UrbiHandler& UrbiHandler::Double(double d) {
    pushUValue(UValue(d));
    return *this;
}

UrbiHandler& UrbiHandler::String(const char* str, unsigned int length, bool copy) {
    pushUValue(UValue(std::string(str)));
    return *this;
}

UrbiHandler& UrbiHandler::StartObject() {
    UValue object;
    object.type = DATA_DICTIONARY;
    object.dictionary = new UDictionary(); //hopefully Urbi deallocates this

    currentUValue_.push(object);
    currentType_.push(TYPE_OBJECT);
    objectKey_.push(OptionalKey());

    return *this;
}

UrbiHandler& UrbiHandler::EndObject(unsigned int memberCount) {
    currentType_.pop();
    objectKey_.pop();

    auto top = currentUValue_.top();
    currentUValue_.pop();
    pushUValue(top);
    
    return *this;
}

UrbiHandler& UrbiHandler::StartArray() {
    UValue list;
    list.type = DATA_LIST;
    list.list = new UList(); //hopefully Urbi deallocates this

    currentUValue_.push(list);
    currentType_.push(TYPE_ARRAY);

    return *this;
}

UrbiHandler& UrbiHandler::EndArray(unsigned int elementCount) {
    currentType_.pop();

    auto top = currentUValue_.top();
    currentUValue_.pop();
    pushUValue(top);    

    return *this;
}
//@}

void UrbiHandler::pushUValue(const UValue &uvalue) {
    switch (currentType_.top()) {
        case TYPE_SCALAR:
            result_ = uvalue;
            break;
        case TYPE_ARRAY:
            currentUValue_.top().list->push_back(uvalue);
            break;
        case TYPE_OBJECT:
            if (objectKey_.top().is_initialized()) {
                (*currentUValue_.top().dictionary)[objectKey_.top().get()] = uvalue;
                objectKey_.top().reset();
            } else {
                assert(uvalue.type == DATA_STRING);
                objectKey_.top() = *uvalue.stringValue;
            }
            break;
    }
}

UValue UrbiHandler::getResult() {
    return result_;
}

