#ifndef HERMIT_MOVEMENT_H
#define HERMIT_MOVEMENT_H

#include <atomic>
#include <functional>

#include <xcs/tsqueue.hpp>
#include <xcs/types/checkpoint.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>
#include <xcs/types/speed_control.hpp>

namespace xcs{
namespace nodes{
namespace hermit{

    typedef xcs::Tsqueue<xcs::Checkpoint> CheckpointQueue;

    class HermitMovement{
        // constants
        static const unsigned int POINTS_ON_METER;
        static const double EPSILON;
        static const double MAX_SPEED;

        xcs::Checkpoint targetCheckpoint_;
        CheckpointQueue checkpointQueue_;

        xcs::CartesianVector dronePosition_;
        xcs::CartesianVector droneVelocity_;
        xcs::EulerianVector droneRotation_;

        // callback function when checkpoint is reached
        std::function<void(bool)> reachedCallback_;

        // command variable
        std::atomic<bool> clear_;
        bool newCheckpoint_;
        bool empty_;

        // support functions
        double computeDistance(const xcs::Checkpoint &targetCheckpoint, const xcs::CartesianVector &actualPosition);
        xcs::Checkpoint computeHermitPoint(const xcs::Checkpoint &start, const xcs::Checkpoint &end, double step);

    public:
        HermitMovement(std::function<void(bool)> reachedCallback = nullptr);

        // Programmer have to ensure calling next four function in proper order (position,velocity,rotation,flyOnCheckpoint) and together!!!
        void dronePosition(const xcs::CartesianVector &dronePosition);
        void droneVelocity(const xcs::CartesianVector &droneVelocity);
        void droneRotation(const xcs::EulerianVector &droneRotation);

        xcs::SpeedControl flyOnCheckpoint(const double &speed = MAX_SPEED);

        void addCheckpoint(const xcs::Checkpoint &checkpoint);
        void deleteCheckpoints();
    };

}}}
#endif