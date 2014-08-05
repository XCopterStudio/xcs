set(PROJECT_NAME netadapter)
project(${PROJECT_NAME})

#set(CMAKE_VERBOSE_MAKEFILE ON)

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

# WebRTC Facade static lib
#add_subdirectory(webrtc_facade)

add_x_object(${PROJECT_NAME} ${SOURCES} ${HEADERS})
x_target_link_libraries(${PROJECT_NAME} xobject)

use_logging(${PROJECT_NAME})

# Urbi
find_package(UrbiAll)
use_urbi(${PROJECT_NAME})

# OpenCV TODO: temporary, only for video display
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )

# LibAV
find_package(LibAV)
include_directories(${LIBAV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${LIBAV_LIBRARIES})

#target_link_libraries(${PROJECT_NAME} webrtc_facade)
#target_link_libraries(${PROJECT_NAME} -L${CMAKE_LIBRARY_OUTPUT_DIRECTORY} -lwebrtc_facade)
#target_link_libraries(${PROJECT_NAME} /home/maros/projects/drones/xcs/onboard/build/lib/libwebrtc_facade.so)
