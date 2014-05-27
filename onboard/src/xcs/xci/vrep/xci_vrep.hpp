#ifndef XCI_VREP_H
#define XCI_VREP_H

#include <xcs/xci/xci.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>
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

    class XciVrep : public virtual Xci {
        static const float POS_MULTI;

        int clientID_;
        std::string droneName_;
        std::string targetName_;
        std::string address_;
        int portNumber_;        
        int droneHandler_;
        int targetHandler_;

        InformationMap configuration_;

        xcs::CartesianVector dronePosition_;
        xcs::EulerianVector droneRotation_;

        std::atomic_bool endAll_;

        std::thread updateThread_;

        void updateSensors();
    public:

        XciVrep(DataReceiver& dataReceiver, std::string droneName = "Quadricopter#", std::string targetName = "Quadricopter_target#", std::string address = "127.0.0.1", int portNumber = 19997);
        ~XciVrep();

        std::string name() override;

        xcs::xci::SensorList sensorList() override;

        std::string configuration(const std::string &key) override;

        xcs::xci::InformationMap configuration() override;

        xcs::xci::SpecialCMDList specialCMD() override;

        void configuration(const std::string &key, const std::string &value) override;

        void configuration(const xcs::xci::InformationMap &configuration) override;

        void command(const std::string &command) override;

        void flyControl(float roll, float pitch, float yaw, float gaz) override;

        void init() override;
    };

}}}


#endif