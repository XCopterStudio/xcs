#ifndef XCI_VREP_H
#define XCI_VREP_H

#include <xcs/xci/xci.hpp>
#include <thread>
#include <atomic>
#include <cmath>
#include <string>

extern "C" {
#include "vrep/extApi.h"
}

namespace xcs {
namespace xci {
namespace vrep{

    class XciVrep : public virtual XCI {
        static const float MAX_ANGLE;

        int clientID_;
        std::string name_;
        std::string address_;
        int portNumber_;        
        int droneHandler_;

        std::atomic_bool endAll_;

        std::thread updateThread_;

        void updateSensors();
    public:

        XciVrep(DataReceiver& dataReceiver, std::string name = "drone#", std::string address = "127.0.0.1", int portNumber = 19997);
        ~XciVrep();

        std::string name() override;

        xcs::xci::SensorList sensorList() override;

        xcs::xci::ParameterValueType parameter(xcs::xci::ParameterNameType name) override;

        std::string configuration(const std::string &key) override;

        xcs::xci::InformationMap configuration() override;

        xcs::xci::SpecialCMDList specialCMD() override;

        void configuration(const std::string &key, const std::string &value) override;

        void configuration(const xcs::xci::InformationMap &configuration) override;

        void command(const std::string &command) override;

        void flyControl(float roll, float pitch, float yaw, float gaz) override;

        void init() override;

        void reset() override;

        void start() override;

        void stop() override;
    };

}}}


#endif