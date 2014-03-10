set(PROJECT_NAME udummy)
project(${PROJECT_NAME})


file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

add_library(${PROJECT_NAME} MODULE ${SOURCES} ${HEADERS})

x_target_link_libraries(${PROJECT_NAME} xcs) # experiment for better linking on Windows, but don't know where the symbols could be referenced for

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})
