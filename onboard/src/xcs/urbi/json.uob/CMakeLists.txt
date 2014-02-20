set(PROJECT_NAME ujson)
project(${PROJECT_NAME})

include_directories(${Urbi_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${rapidjson_INCLUDE_DIRS})

file(GLOB SOURCES *.cpp)

add_library(${PROJECT_NAME} MODULE ${SOURCES})

target_link_libraries(${PROJECT_NAME} port_d boost_system urbi)