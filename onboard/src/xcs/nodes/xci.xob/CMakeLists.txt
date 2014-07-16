set(PROJECT_NAME xxci)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp structs/*.cpp)
file(GLOB HEADERS *.hpp structs/*.hpp)

add_x_object(${PROJECT_NAME} ${SOURCES} ${HEADERS})

x_target_link_libraries(${PROJECT_NAME} xcs)

use_logging(${PROJECT_NAME})





