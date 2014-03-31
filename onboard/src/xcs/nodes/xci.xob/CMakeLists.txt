set(PROJECT_NAME xxci)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp struct/*.cpp)
file(GLOB HEADERS *.hpp struct/*.hpp)

add_library(${PROJECT_NAME} MODULE ${SOURCES} ${HEADERS})

x_target_link_libraries(${PROJECT_NAME} xcs)
x_target_link_libraries(${PROJECT_NAME} xobject)

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})





