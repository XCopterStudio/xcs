#ifndef DATA_RECEIVER_H
#define DATA_RECEIVER_H

#include <map>
#include <memory>

#include <urbi/uobject.hh>

namespace xcs {
namespace xci {

/*!
 * If you don't need/want to include uobject.hh only use forward declaration
 * of this class and only in implementation dependent files include this
 * whole file.
 */
class DataReceiver {
public:

    DataReceiver() {
    }

    ~DataReceiver() {
    }

    template<typename T>
    void notify(const std::string& sensorName, T value) {
        auto it = outputs_.find(sensorName);
        if (it == outputs_.end()) {
            throw std::runtime_error("Unregistered sensor '" + sensorName + "'."); // TODO is it necessary to link with libxcs, therefore std::runtime_error?
        }
        *(it->second) = value;
    }

    /*!
     * \param sensorName
     * \param uvar  DataReceiver becomes owner of the pointed UVar (will free memory)
     */
    void registerOutput(const std::string& sensorName, ::urbi::UVar* uvar) {
        outputs_[sensorName] = std::unique_ptr<::urbi::UVar>(uvar);
    }
private:
    /*!
     * We own the UVar so ensure deallocation in standard destructor.
     */
    typedef std::map<std::string, std::unique_ptr<::urbi::UVar> > OutputsType;
    OutputsType outputs_;
};

}
}

#endif