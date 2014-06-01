set(PROJECT_NAME xlinekeeper)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

add_library(${PROJECT_NAME} MODULE ${SOURCES} ${HEADERS})

x_target_link_libraries(${PROJECT_NAME} xcs)
x_target_link_libraries(${PROJECT_NAME} xlinedrawer)

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})

# opencv (only headers)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )


