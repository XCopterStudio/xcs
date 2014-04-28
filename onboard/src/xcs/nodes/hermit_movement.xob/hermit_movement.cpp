#include "hermit_movement.hpp"

#include <armadillo>

using namespace arma;
using namespace xcs;
using namespace xcs::nodes::hermit;

xcs::Checkpoint HermitMovement::computeHermitPoint(const xcs::Checkpoint &start, const xcs::Checkpoint &end, double step){
    mat steps;
    steps << step*step*step
          << step*step
          << step
          << 1 << endr;

    mat derSteps;
    derSteps << 3*step*step;
    derSteps << 2*step;
    derSteps << 1;
    derSteps << 0 << endr;

    mat hermitPolynomials;
    hermitPolynomials <<  2.0 << -2.0 <<  1.0 <<  1.0 << endr
                      << -3.0 << 3.0 << -2.0 << -1.0 << endr
                      << 0.0 << 0.0 << 1.0 << 0.0 << endr
                      << 1.0 << 0.0 << 0.0 << 0.0 << endr;

    mat points;
    points << start.x << start.y << start.z << endr
           << end.x << end.y << end.z << endr
           << start.xOut << start.yOut << start.zOut << endr
           << end.xOut << end.yOut << end.zOut << endr;

    mat interPoint = steps*hermitPolynomials*points;
    mat tangent = derSteps*hermitPolynomials*points;

    return Checkpoint(interPoint[1,1],interPoint[1,2],interPoint[1,3],
        tangent[1,1],tangent[1,2],tangent[1,3]);
}
   
