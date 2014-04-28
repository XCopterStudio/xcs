#ifndef HERMIT_MOVEMENT_H
#define HERMIT_MOVEMENT_H

#include <mutex>
#include <atomic>
#include <thread>

#include <xcs/tsqueue.hpp>
#include <xcs/types/checkpoint.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>

namespace xcs{
namespace nodes{
namespace hermit{

    typedef xcs::Tsqueue<xcs::Checkpoint> CheckpointQueue;

    class HermitMovement{
        // constants
        static const unsigned int POINTS_ON_METER;
        static const double EPSILON;
        static const double MAX_SPEED;

        CheckpointQueue checkpointQueue_;

        xcs::CartesianVector dronePosition_;
        xcs::CartesianVector droneVelocity_;
        xcs::EulerianVector droneRotation_;

        //locking primitive
        std::mutex mtxPosition_;
        std::mutex mtxVelocity_;
        std::mutex mtxRotation_;

        //end variable
        std::atomic_bool endAll_;
        std::atomic_bool clear_;

        //thread
        std::thread flyOnCheckpointsThread;

        // support functions
        double computeDistance(const xcs::Checkpoint &targetCheckpoint, const xcs::CartesianVector &actualPosition);
        xcs::Checkpoint computeHermitPoint(const xcs::Checkpoint &start, const xcs::Checkpoint &end, double step);

        // movementFunction
        void flyOnCheckpoint(const xcs::Checkpoint &targetCheckpoint);
        void flyOnCheckpoints();
    public:
        HermitMovement();

        void dronePosition(const xcs::CartesianVector &dronePosition);
        void droneVelocity(const xcs::CartesianVector &droneVelocity);
        void droneRotation(const xcs::EulerianVector &droneRotation);

        void addCheckpoint(const xcs::Checkpoint &checkpoint);
        void deleteCheckpoints();
    };

}}}
#endif