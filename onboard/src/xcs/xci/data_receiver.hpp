#ifndef DATA_RECEIVER_H
#define DATA_RECEIVER_H

#include <map>
#include <memory>
#include <iostream>
#include <mutex>

#include <urbi/uobject.hh>

#include <xcs/nodes/xobject/simple_x_var.hpp>
#include <xcs/nodes/xobject/x_type.hpp>
#include <xcs/types/bitmap_type.hpp>

namespace xcs {
namespace xci {

/*!
 * If you don't need/want to include uobject.hh only use forward declaration
 * of this class and only in implementation dependent files include this
 * whole file.
 */
class DataReceiver {
private:
    typedef std::map<std::string, std::unique_ptr<nodes::SimpleXVar> > OutputsType;

    /*! We are owners of the UVars, kept in the name indexed map. */
    OutputsType outputs_;

    /*! See hack notify() overload for xcs::BitmapTypeChronologic */
    std::mutex bitmapLock_;

    OutputsType::const_iterator getSensorUVar(const std::string& sensorName) const {
        auto it = outputs_.find(sensorName);
        if (it == outputs_.end()) {
            throw std::runtime_error("Unregistered sensor '" + sensorName + "'."); // TODO is it necessary to link with libxcs, therefore std::runtime_error?
        }
        return it; //TODO what about returning reference to the held UVar?
    }

    urbi::UBinary toUBinary(const xcs::BitmapType &value) const {
        urbi::UBinary bin;
        bin.type = urbi::BINARY_IMAGE;
        bin.image.width = value.width;
        bin.image.height = value.height;
        bin.image.size = value.width * value.height * 3;
        bin.image.imageFormat = urbi::IMAGE_RGB;
        bin.image.data = value.data;
        return bin;
    }

public:

    DataReceiver() {
    }

    ~DataReceiver() {
    }

    template<typename T>
    void notify(const std::string& sensorName, T value) {
        auto it = getSensorUVar(sensorName);
        *(it->second) = value;
    }

    /*!
     * This specialization is needed because of special memory managment.
     */
    void notify(const std::string& sensorName, xcs::BitmapType value) {
        auto it = getSensorUVar(sensorName);
        auto bin = toUBinary(value);

        *(it->second) = bin; // this will do deep copy of the image buffer
        bin.image.data = nullptr;
    }

    /*!
     * This overload is a hack because of memory managment of binary data and timestamp notification
     * (cannot encapsulate pointer and timestamp in one struct correctly).
     * 
     * We send the timestamp in advance to the same UVar/InputPort and
     * it's receiver's responsibility to handle it properly.
     * 
     * The lock may be needed when multiple notifies at the moment
     * (e.g. *_BIND_THREADED macro was used and processing takes longer than "creation").         * 
     */
    void notify(const std::string& sensorName, xcs::BitmapTypeChronologic value) {
        auto it = getSensorUVar(sensorName);
        auto bin = toUBinary(value);

        /*
         * 
         */
        std::lock_guard<std::mutex> lock(bitmapLock_);
        *(it->second) = value.time;
        *(it->second) = bin; // this will do deep copy of the image buffer
        bin.image.data = nullptr;
    }

    /*!
     * \param sensorName
     * \param uvar  DataReceiver becomes owner of the pointed UVar (will free memory)
     */
    nodes::SimpleXVar &registerOutput(const std::string& sensorName, const nodes::XType &type) {
        outputs_[sensorName] = std::unique_ptr<nodes::SimpleXVar>(new nodes::SimpleXVar(type));
        return (*outputs_.at(sensorName).get());
    }
};

}
}

#endif