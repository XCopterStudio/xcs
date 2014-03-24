set(PROJECT_NAME ulinekeeper)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

x_target_link_libraries(${PROJECT_NAME} xcs)
x_target_link_libraries(${PROJECT_NAME} ulinedrawer)

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})

# opencv (only headers)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )


