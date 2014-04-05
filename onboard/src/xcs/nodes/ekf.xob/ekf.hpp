#ifndef EKF_H
#define EKF_H

#include <queue>
#include <utility>

#include <xcs/nodes/xci.xob/structs/fly_param.hpp>
#include <xcs/nodes/xci.xob/structs/cartesian_vector.hpp>
#include <xcs/nodes/xci.xob/structs/eulerian_vector.hpp>

namespace xcs{
namespace nodes{
namespace ekf{

    struct DroneState{
        xcs::nodes::xci::CartesianVector position;
        xcs::nodes::xci::CartesianVector velocity;
        xcs::nodes::xci::EulerianVector angles;
        double angularRotationPsi;
    };

    struct DroneStateMeasurement{
        xcs::nodes::xci::CartesianVector velocity;
        xcs::nodes::xci::EulerianVector angles;
        double altitude;
    };

    typedef std::pair<DroneState, DroneState> DroneStateDistribution;
    typedef std::pair<DroneStateMeasurement, long int> MeasurementChronologic;
    typedef std::pair<DroneStateDistribution, long int> DroneStateDistributionChronologic;
    typedef std::pair<xcs::nodes::xci::FlyParam, long int> FlyParamChronologic;

    // Measurement consist from measurement of the drone state and timestamp. Timestamp is the time in milliseconds when measurement was created.
    typedef std::queue< MeasurementChronologic > Measurements;
    // Drone state consist from the drone state and timestamp. Timestamp is the time in milliseconds when drone state was created.
    typedef std::queue< DroneStateDistributionChronologic > DroneStates;
    // FlyParam consist from send fly parameters and timestamp. Timestamp is the time in milliseconds when drone received fly parameters.
    typedef std::queue< FlyParamChronologic > FlyParams;



    class Ekf{
        DroneStates droneStates_;
        DroneStates droneStateDeviation_;
        Measurements measurements_;
        FlyParams flyParams_;

        double parameters[9];

        DroneStateDistribution predict(const DroneStateDistribution &state, const xcs::nodes::xci::FlyParam &flyparam, const double &delta);
    public:

    };

}}}

#endif