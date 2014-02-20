#
# Urbi SDK dependency
#

# Paths
set(CMAKE_LIBRARY_PATH ${URBI_ROOT}/lib)

set(Urbi_DIR ${URBI_ROOT}/share/cmake/urbi)
find_package(Urbi)

set(Port_DIR ${URBI_ROOT}/share/cmake/port)
find_package(Port)

find_package(Boost 1.55 REQUIRED COMPONENTS system)

set(Xurbi_INCLUDE_DIRS ${URBI_INCLUDE_DIRS};${PORT_INCLUDE_DIRS};${Boost_INCLUDE_DIRS})
set(Xurbi_LIBRARIES ${URBI_LIBRARIES};${PORT_LIBRARIES};${Boost_LIBRARIES})

function(use_urbi TARGET_NAME)
include_directories(${Xurbi_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} ${Xurbi_LIBRARIES})
endfunction(use_urbi)