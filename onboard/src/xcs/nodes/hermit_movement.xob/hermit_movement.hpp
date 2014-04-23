#ifndef HERMIT_MOVEMENT_H
#define HERMIT_MOVEMENT_H

#include <xcs/tsqueue.hpp>
#include <xcs/types/checkpoint.hpp>

namespace xcs{
namespace nodes{
namespace hermit{

    typedef xcs::Tsqueue<Checkpoint> CheckpointQueue;

    class HermitMovement{
        CheckpointQueue checkpointQueue_;

        xcs::Checkpoint computeHermitPoint(const xcs::Checkpoint &start, const xcs::Checkpoint &end, double step);
    public:
        
    };

}}}
#endif