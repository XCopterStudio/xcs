set(PROJECT_NAME xmultiplexer)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

add_x_object(${PROJECT_NAME} ${SOURCES} ${HEADERS})

use_logging(${PROJECT_NAME})