set(PROJECT_NAME ujson)
project(${PROJECT_NAME})

include_directories(${rapidjson_INCLUDE_DIRS})

file(GLOB SOURCES *.cpp)

add_library(${PROJECT_NAME} MODULE ${SOURCES})


find_package(Xurbi)
use_urbi(${PROJECT_NAME})
