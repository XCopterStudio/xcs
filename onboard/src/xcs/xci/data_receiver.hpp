#ifndef DATA_RECEIVER_H
#define DATA_RECEIVER_H

#include <map>
#include <memory>
#include <iostream>
#include <mutex>
#include <atomic>

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

    nodes::SimpleXVar &getSensorXVar(const std::string& sensorName) const {
        auto it = outputs_.find(sensorName);
        if (it == outputs_.end()) {
            throw std::runtime_error("Unregistered sensor '" + sensorName + "'."); // TODO is it necessary to link with libxcs, therefore std::runtime_error?
        }
        return *(it->second);
    }

    std::atomic<bool> enabled_;


public:

    DataReceiver() : enabled_(false) {
    }

    ~DataReceiver() {
    }

    void enabled(const bool value) {
        enabled_ = value;
    }

    bool enabled() const {
        return enabled_;
    }

    template<typename T>
    void notify(const std::string& sensorName, T value) {
        if (!enabled_) {
            return;
        }
        nodes::SimpleXVar &xvar = getSensorXVar(sensorName);
        xvar = value;
    }

    /*!
     * This specialization is needed because of special memory managment.
     */
    void notify(const std::string& sensorName, xcs::BitmapType value) {
        if (!enabled_) {
            return;
        }
        nodes::SimpleXVar &xvar = getSensorXVar(sensorName);

        // Prepare the binary
        urbi::UBinary bin;
        bin.type = urbi::BINARY_IMAGE;
        bin.image.width = value.width;
        bin.image.height = value.height;
        bin.image.size = value.width * value.height * 3;
        bin.image.imageFormat = urbi::IMAGE_RGB;
        bin.image.data = value.data;

        xvar = bin; // this will do deep copy of the image buffer
        bin.image.data = nullptr; // not to deallocate in dtor
    }

    /*!
     * Creates SimpleXVar identified by the name with the specified type.
     * DataReceiver is (memory) owner of the SimpleXVar and returns reference
     * valid until data receiver is alive.
     */
    nodes::SimpleXVar &registerOutput(const std::string& name, const nodes::XType &type) {
        outputs_[name] = std::unique_ptr<nodes::SimpleXVar>(new nodes::SimpleXVar(type));
        return *(outputs_.at(name).get());
    }
};

}
}

#endif