#ifndef XCONTROL_H
#define XCONTROL_H

#include <atomic>

#include "control.hpp"

#include <xcs/types/eulerian_vector.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/fly_control.hpp>
#include <xcs/types/velocity_control.hpp>

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>
#include <xcs/nodes/xobject/x_var.hpp>

namespace xcs{
namespace nodes{
namespace control{

    /*! \brief XObject wrapper on Control.

    Desire and actual quadricopter velocity have to be in m/s and global frame.
    Quadricopter rotation have to be in radians.
    \warning Xobject user have to assure that change first velocity and then rotation.
    */
    class XControl : public XObject{
        Control control_;

        std::atomic<bool> stopped_;

        void onChangeVelocity(xcs::CartesianVector velocity);
        void onChangeRotation(xcs::EulerianVector rotation);
        void onChangeDesireVelocity(xcs::VelocityControl desireVelocity);

        void loadParameters(const std::string &file);
    protected:
        virtual void stateChanged(XObject::State state);
    public:
        // Incoming data
        /// Quadricopter actual velocity
        xcs::nodes::XInputPort<xcs::CartesianVector> velocity;
        /// Quadricopter actual rotation
        xcs::nodes::XInputPort<xcs::EulerianVector> rotation;
        /// Desire quadricopter velocity
        xcs::nodes::XInputPort<xcs::VelocityControl> desireVelocity;
        // Output data
        /// FlyControl for the quadricopter
        xcs::nodes::XVar<xcs::FlyControl> flyControl;

        /// Initialize private variables and get name from Urbi
        XControl(const std::string& name);
        /*! Load Control PID parameters from file.
            
            \param file absolute path of XControl setting file
        */
        void init(const std::string &file);
    };

}}}

#endif