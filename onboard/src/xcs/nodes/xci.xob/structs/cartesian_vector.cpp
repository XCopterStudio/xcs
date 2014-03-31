#include "cartesian_vector.hpp"

using namespace xcs::nodes::xci;

CartesianVector::CartesianVector() :
    x(0),
    y(0),
    z(0){
}

CartesianVector::CartesianVector(double x, double y, double z) :
x(x),
y(y),
z(z){
}

X_GENERATE_STRUCT(CartesianVector, x, y, z);