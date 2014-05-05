set(PROJECT_NAME xci_vrep)
project(${PROJECT_NAME})


file(GLOB SOURCES *.cpp ./vrep/*.cpp)
file(GLOB HEADERS *.hpp ./vrep/*.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

x_target_link_libraries(${PROJECT_NAME} xcs)

find_package(UrbiAll)
use_urbi(${PROJECT_NAME})