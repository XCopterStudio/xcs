#ifndef XHERMIT_MOVEMENT_H
#define XHERMIT_MOVEMENT_H

#include "hermit_movement.hpp"

#include <xcs/types/checkpoint.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>
#include <xcs/nodes/xobject/x_object.hpp>

namespace xcs {
namespace nodes {

    class XHermitMovement : public xcs::nodes::XObject{
        xcs::nodes::hermit::HermitMovement hermitMovement_;

        void onChangeCommand(std::string command);
        void onChangeAddCheckpoint(xcs::Checkpoint checkpoint);
        void onChangeDronePosition(xcs::CartesianVector position);
        void onChangeDroneRotation(xcs::EulerianVector rotation);
    public:
        xcs::nodes::XVar<std::string> command;
        xcs::nodes::XVar<xcs::Checkpoint> checkpoint;
        xcs::nodes::XVar<xcs::CartesianVector> dronePosition;
        xcs::nodes::XVar<xcs::EulerianVector> droneRotation;

        XHermitMovement(const std::string& name);

    };

}
}

#endif