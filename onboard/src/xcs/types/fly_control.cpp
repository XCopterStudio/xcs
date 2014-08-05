#include "fly_control.hpp"
#include <xcs/nodes/xobject/x_impl.hpp>

#undef X_STRUCT_NAMESPACE
#define X_STRUCT_NAMESPACE "xcs::"
X_GENERATE_STRUCT(FlyControl, roll, pitch, yaw, gaz);
#undef X_STRUCT_NAMESPACE