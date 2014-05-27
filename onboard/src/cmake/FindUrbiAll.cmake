#
# Urbi SDK dependency
#

# Paths
set(CMAKE_LIBRARY_PATH ${URBI_D_ROOT}/lib;${URBI_ROOT}/lib;${CMAKE_LIBRARY_PATH})
#message(STATUS "Searching Urbi libraries in: ${CMAKE_LIBRARY_PATH}")

set(Port_DIR ${URBI_ROOT}/share/cmake/port)
find_package(Port)
mark_as_advanced(PORT_LIBRARIES)

set(Qijpeg_DIR ${URBI_ROOT}/share/cmake/qijpeg)
find_package(Qijpeg)
mark_as_advanced(QIJPEG_LIBRARIES)

set(Sched_DIR ${URBI_ROOT}/share/cmake/sched)
find_package(Sched)
mark_as_advanced(SCHED_LIBRARIES)

set(Serialize_DIR ${URBI_ROOT}/share/cmake/serialize)
find_package(Serialize)
mark_as_advanced(SERIALIZE_LIBRARIES)

set(Urbi_DIR ${URBI_ROOT}/share/cmake/urbi)
find_package(Urbi)
mark_as_advanced(URBI_LIBRARIES)

set(Uobject_DIR ${URBI_ROOT}/share/cmake/uobject)
find_package(Uobject)
mark_as_advanced(UOBJECT_LIBRARIES)



find_package(Boost 1.55 REQUIRED COMPONENTS system)

set(URBIALL_INCLUDE_DIRS ${PORT_INCLUDE_DIRS};${QIJPEG_INCLUDE_DIRS};${SCHED_INCLUDE_DIRS};${SERIALIZE_INCLUDE_DIRS};${UOBJECT_INCLUDE_DIRS};${Boost_INCLUDE_DIRS})
set(URBIALL_LIBRARIES ${PORT_LIBRARIES};${QIJPEG_LIBRARIES};${SCHED_LIBRARIES};${SERIALIZE_LIBRARIES};${URBI_LIBRARIES};${UOBJECT_LIBRARIES};${Boost_LIBRARIES})

function(use_urbi TARGET_NAME)
include_directories(${URBIALL_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} ${URBIALL_LIBRARIES})
# link_directories(${URBI_ROOT}/bin) # I think this is unnecessary. (Michal)
endfunction(use_urbi)