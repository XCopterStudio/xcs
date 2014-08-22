#include "x_multiplexer.hpp"

#include <xcs/types/fly_control.hpp>

using namespace xcs::nodes::multiplexer;
/// New name for XMultiplexer specialization
typedef XMultiplexer<xcs::FlyControl> XMultiplexerFlyControl;
/// Add new XMultiplexer specialization for use in urbiscript 
XStart(XMultiplexerFlyControl);