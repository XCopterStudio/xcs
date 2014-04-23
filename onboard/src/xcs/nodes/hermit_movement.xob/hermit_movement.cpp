#include "hermit_movement.hpp"

#include <armadillo>

using namespace arma;
using namespace xcs;
using namespace xcs::nodes::hermit;

xcs::Checkpoint HermitMovement::computeHermitPoint(const xcs::Checkpoint &start, const xcs::Checkpoint &end, double step){
    mat steps;
    steps << 1 << endr;
    steps << step << endr;
    steps << step*step << endr;
    steps << step*step*step << endr;

    mat hermitPolynomials;
    hermitPolynomials <<  2.0 << -2.0 <<  1.0 <<  1.0 << endr;
    hermitPolynomials << -3.0 << 3.0 << -2.0 << -1.0 << endr;
    hermitPolynomials << 0.0 << 0.0 << 1.0 << 0.0 << endr;
    hermitPolynomials << 1.0 << 0.0 << 0.0 << 0.0 << endr;
};
   
}
