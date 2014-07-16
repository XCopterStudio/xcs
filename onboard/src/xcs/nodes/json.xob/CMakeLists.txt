set(PROJECT_NAME xjson)
project(${PROJECT_NAME})

include_directories(${rapidjson_INCLUDE_DIRS})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

add_x_object(${PROJECT_NAME} ${SOURCES} ${HEADERS})
