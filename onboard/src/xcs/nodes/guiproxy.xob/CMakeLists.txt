set(PROJECT_NAME xguiproxy)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

add_x_object(${PROJECT_NAME} ${SOURCES} ${HEADERS})
x_target_link_libraries(${PROJECT_NAME} xobject)

use_logging(${PROJECT_NAME})

# Urbi
find_package(UrbiAll)
use_urbi(${PROJECT_NAME})

# LibAV
find_package(LibAV)
include_directories(${LIBAV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${LIBAV_LIBRARIES})

# uncomment for video display test
# OpenCV
#find_package( OpenCV REQUIRED )
#include_directories( ${OpenCV_INCLUDE_DIRS} )
#target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )



