#include "cartesian_vector.hpp"
#include <xcs/nodes/xobject/x_impl.hpp>

#undef X_STRUCT_NAMESPACE
#define X_STRUCT_NAMESPACE "xcs::"
X_GENERATE_STRUCT(CartesianVector, x, y, z);
X_GENERATE_STRUCT(CartesianVectorChronologic, x, y, z, time);
#undef X_STRUCT_NAMESPACE