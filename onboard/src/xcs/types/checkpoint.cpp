#include "checkpoint.hpp"
#include <xcs/nodes/xobject/x_impl.hpp>

#undef X_STRUCT_NAMESPACE
#define X_STRUCT_NAMESPACE "xcs::"
X_GENERATE_STRUCT(Checkpoint, x, y, z, xOut, yOut, zOut);
#undef X_STRUCT_NAMESPACE