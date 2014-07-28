#ifndef XCONTROL_H
#define XCONTROL_H

#include <atomic>

#include "control.hpp"

#include <xcs/types/eulerian_vector.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/fly_control.hpp>
#include <xcs/types/speed_control.hpp>

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>
#include <xcs/nodes/xobject/x_var.hpp>

namespace xcs{
namespace nodes{
namespace control{

    class XControl : public XObject{
        Control control_;

        std::atomic<bool> stoped;

        void onChangeVelocity(xcs::CartesianVector velocity);
        void onChangeRotation(xcs::EulerianVector rotation);
        void onChangeDesireVelocity(xcs::SpeedControl desireVelocity);

        void loadParameters(const std::string &file);
    public:
        // Incoming data
        xcs::nodes::XInputPort<xcs::CartesianVector> velocity;
        xcs::nodes::XInputPort<xcs::EulerianVector> rotation;
        xcs::nodes::XInputPort<xcs::SpeedControl> desireVelocity;
        // Output data
        xcs::nodes::XVar<xcs::FlyControl> flyControl;

        XControl(const std::string& name);
        void init(const std::string &file);
        inline void start(){ stoped = false; };
        inline void stop(){ stoped = true; };
    };

}}}

#endif