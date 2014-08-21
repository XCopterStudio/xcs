set(PROJECT_NAME xcheckpoint_movement)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

add_x_object(${PROJECT_NAME} ${SOURCES} ${HEADERS})

find_package(Armadillo)
use_armadillo(${PROJECT_NAME})