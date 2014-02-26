set(PROJECT_NAME ujson)
project(${PROJECT_NAME})

include_directories(${rapidjson_INCLUDE_DIRS})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

add_library(${PROJECT_NAME} MODULE ${SOURCES} ${HEADERS})

x_target_link_libraries(${PROJECT_NAME} xcs) # experiment for better linking on Windows, but don't know where the symbols could be referenced for

find_package(Xurbi)
use_urbi(${PROJECT_NAME})
