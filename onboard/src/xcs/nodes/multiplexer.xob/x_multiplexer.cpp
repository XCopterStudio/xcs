#include "x_multiplexer.hpp"

#include <xcs/types/eulerian_vector.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/fly_control.hpp>
#include <xcs/types/speed_control.hpp>

using namespace xcs::nodes::multiplexer;
/// New name for XMultiplexer specialization
typedef XMultiplexer<xcs::FlyControl> XMultiplexerFlyControl;
/// Add new XMultiplexer specialization for use in urbiscript 
XStart(XMultiplexerFlyControl);