#include "speed_control.hpp"
#include <xcs/nodes/xobject/x_impl.hpp>

#undef X_STRUCT_NAMESPACE
#define X_STRUCT_NAMESPACE "xcs::"
X_GENERATE_STRUCT(SpeedControl, vx, vy, vz, psi);
#undef X_STRUCT_NAMESPACE