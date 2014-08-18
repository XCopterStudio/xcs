#
# Global configuration
#

# Paths
set(URBI_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../)

set(BOOST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../bin)

# Flags
set(Boost_USE_STATIC_LIBS false)
add_definitions(-DBOOST_ALL_NO_LIB) # disable autolinking (causing problems in MS VS)
