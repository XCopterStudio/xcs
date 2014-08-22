#ifndef CHECKPOINT_MOVEMENT_H
#define CHECKPOINT_MOVEMENT_H

#include <atomic>
#include <functional>

#include <xcs/tsqueue.hpp>
#include <xcs/types/checkpoint.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>
#include <xcs/types/velocity_control.hpp>

namespace xcs{
namespace nodes{
namespace checkpoint{

    typedef xcs::Tsqueue<xcs::Checkpoint> CheckpointQueue;

    /*! \brief Create quadricopter FlyControl so that quadricopter will may reach all desired checkpoints.

        \warning Programmer have to ensure calling next four function in proper order dronePosition(), droneVelocity(), droneRotation() and flyOnCheckpoint() !!!
    */
    class CheckpointMovement{
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
        /*! Initialize private variables and set callback function which will be called when quadricopter arrive on checkpoint.
        
            \param reachedCallback It will be called every time when quadricopter arrive on checkpoint with true.
                                    With false when quadricopter go on another checkpoint.
        */
        CheckpointMovement(std::function<void(bool)> reachedCallback = nullptr);

        /*! Set private drone position variable. 
            \param dronePosition actual position an quadricopter 
        */
        void dronePosition(const xcs::CartesianVector &dronePosition);
        /*! Set private drone velocity variable.
        \param dronePosition actual velocity an quadricopter
        */
        void droneVelocity(const xcs::CartesianVector &droneVelocity);
        /*! Set private drone rotation variable.
        \param droneRotation actual position an quadricopter
        */
        void droneRotation(const xcs::EulerianVector &droneRotation);

        /*! Compute VelocityControl for an quadricopter base on private variables describing quadricopters actual state and first checkpoint in queue.
            
            \param speed quadricopter will go on checkpoints with this speed as maximal speed
            \sa addCheckpoint()
        */
        xcs::VelocityControl flyOnCheckpoint(const double &speed = MAX_SPEED);

        /*! Add checkpoint in checkpoint queue.
            
            From this queue are taken checkpoints for quadricopter speed control.
        */
        void addCheckpoint(const xcs::Checkpoint &checkpoint);
        /*!  Clear checkpoint queue.
        
            Stop actual fly on checkpoint and delete all checkpoints from checkpoint queue.
        */
        void deleteCheckpoints();
    };

}}}
#endif