#
# Urbi SDK dependency
#

# Paths
set(CMAKE_LIBRARY_PATH ${URBI_ROOT}/lib)

set(Port_DIR ${URBI_ROOT}/share/cmake/port)
find_package(Port)

set(Qijpeg_DIR ${URBI_ROOT}/share/cmake/qijpeg)
find_package(Qijpeg)

set(Sched_DIR ${URBI_ROOT}/share/cmake/sched)
find_package(Sched)

set(Serialize_DIR ${URBI_ROOT}/share/cmake/serialize)
find_package(Serialize)

#set(Urbi_DIR ${URBI_ROOT}/share/cmake/urbi)
#find_package(Urbi)

set(Uobject_DIR ${URBI_ROOT}/share/cmake/uobject)
find_package(Uobject)



find_package(Boost 1.55 REQUIRED COMPONENTS system)

set(Xurbi_INCLUDE_DIRS ${URBI_INCLUDE_DIRS};${PORT_INCLUDE_DIRS};${Boost_INCLUDE_DIRS})
#set(Xurbi_LIBRARIES ${URBI_LIBRARIES};${PORT_LIBRARIES};${Boost_LIBRARIES})
set(Xurbi_LIBRARIES ${PORT_LIBRARIES};${QIJPEG_LIBRARIES};${SCHED_LIBRARIES};${SERIALIZE_LIBRARIES};${UOBJECT_LIBRARIES};${Boost_LIBRARIES})

message("Urbi libraries: ${Xurbi_LIBRARIES}")

function(use_urbi TARGET_NAME)
include_directories(${Xurbi_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} ${Xurbi_LIBRARIES})
endfunction(use_urbi)