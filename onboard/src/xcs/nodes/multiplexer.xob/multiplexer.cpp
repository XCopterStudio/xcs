#include "multiplexer.hpp"

#include <xcs/types/eulerian_vector.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/fly_control.hpp>
#include <xcs/types/speed_control.hpp>

using namespace xcs::nodes::multiplexer;
typedef XMultiplexer<xcs::FlyControl> XMultiplexerFlyControl;

XStart(XMultiplexerFlyControl);