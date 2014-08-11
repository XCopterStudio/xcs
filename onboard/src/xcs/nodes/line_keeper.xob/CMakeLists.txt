set(PROJECT_NAME xline_keeper)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

add_x_object(${PROJECT_NAME} ${SOURCES} ${HEADERS})

x_target_link_libraries(${PROJECT_NAME} xlinedrawer)

# opencv (only headers)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )


