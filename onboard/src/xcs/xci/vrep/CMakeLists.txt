set(PROJECT_NAME xci_vrep)
project(${PROJECT_NAME})


include_directories(${VREP_REMOTE_ROOT})

file(GLOB SOURCES *.cpp ${VREP_REMOTE_ROOT}/vrep/*.c)
file(GLOB HEADERS *.hpp ${VREP_REMOTE_ROOT}/vrep/*.h)

add_definitions(-DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

x_target_link_libraries(${PROJECT_NAME} xcs)
x_target_link_libraries(${PROJECT_NAME} xobject)

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})

use_logging(${PROJECT_NAME})