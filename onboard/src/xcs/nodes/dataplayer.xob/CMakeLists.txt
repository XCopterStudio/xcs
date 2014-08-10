set(PROJECT_NAME xdataplayer)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

add_x_object(${PROJECT_NAME} ${SOURCES} ${HEADERS})

find_package(LibAV)
include_directories(${LIBAV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${LIBAV_LIBRARIES})

find_package(Boost REQUIRED COMPONENTS filesystem regex)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})


use_logging(${PROJECT_NAME})
