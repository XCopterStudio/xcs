set(PROJECT_NAME xci_dodo)
project(${PROJECT_NAME})

include_directories(${Urbi_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})

file(GLOB SOURCES *.cpp)

x_add_library(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} port_d boost_system urbi)
