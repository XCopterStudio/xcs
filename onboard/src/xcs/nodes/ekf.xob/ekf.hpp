#ifndef EKF_H
#define EKF_H

#include <xcs/nodes/xci.xob/structs/cartesian_vector.hpp>
#include <xcs/nodes/xci.xob/structs/eulerian_vector.hpp>

namespace xcs{
    namespace nodes{
        namespace ekf{

struct DroneState{
    ::xcs::nodes::xci::CartesianVector position;
    ::xcs::nodes::xci::CartesianVector speed;
    ::xcs::nodes::xci::EulerianVector angles;
    double angularRotationTheta;
};

class Ekf{
    DroneState droneState;
    DroneState droneStateDeviation;
public:

};

        }
    }
}

#endif