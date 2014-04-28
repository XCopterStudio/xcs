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

    typedef xcs::Tsqueue<Checkpoint> CheckpointQueue;

    class HermitMovement{
        // constants
        static const unsigned int pointsOnMeter;

        CheckpointQueue checkpointQueue_;

        xcs::CartesianVector dronePosition_;
        xcs::EulerianVector droneRotation_;
        xcs::Checkpoint targetCheckpoint_;

        //locking primitive
        std::mutex mtxPosition_;
        std::mutex mtxRotation_;

        //end variable
        std::atomic_bool endAll_;
        std::atomic_bool clear_;

        //thread
        std::thread flyOnCheckpointsThread;

        xcs::Checkpoint computeHermitPoint(const xcs::Checkpoint &start, const xcs::Checkpoint &end, double step);
        void flyOnCheckpoints();
    public:
        HermitMovement();

        void dronePosition(const xcs::CartesianVector &dronePosition);
        void droneRotation(const xcs::EulerianVector &droneRotation);

        void addCheckpoint(const Checkpoint &checkpoint);
        void deleteCheckpoints();
    };

}}}
#endif