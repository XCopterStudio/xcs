set(PROJECT_NAME uxci)
project(${PROJECT_NAME})

include_directories(${Urbi_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})

file(GLOB SOURCES *.cpp)

add_library(${PROJECT_NAME} MODULE ${SOURCES})

x_target_link_libraries(${PROJECT_NAME} xcs)

target_link_libraries(${PROJECT_NAME} boost_system)
target_link_libraries(${PROJECT_NAME} general port_d general urbi_d)
target_link_libraries(${PROJECT_NAME} debug port_d debug urbi_d)

