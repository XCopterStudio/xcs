set(PROJECT_NAME ulinefinder)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)

add_library(${PROJECT_NAME} MODULE ${SOURCES})

x_target_link_libraries(${PROJECT_NAME} xcs)

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})

# opencv
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
target_link_libraries( ulinefinder ${OpenCV_LIBS} )
