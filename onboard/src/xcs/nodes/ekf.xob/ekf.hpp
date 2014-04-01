#ifndef EKF_H
#define EKF_H

#include <queue>
#include <utility>

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

    struct DroneStateMeasurement{
        ::xcs::nodes::xci::CartesianVector speed;
        ::xcs::nodes::xci::EulerianVector angles;
        double altitude;
    };

    // Measurement consist from measurement of the drone state and timestamp. Timestamp is the time when measurement was created.
    typedef std::queue< std::pair<DroneStateMeasurement, long int> > Measurements; 
    typedef std::queue< std::pair<DroneState, long int> > DroneStates;

    class Ekf{
        DroneStates droneStates_;
        DroneStates droneStateDeviation_;
        Measurements measurements_;
    public:

    };

}}}

#endif