set(PROJECT_NAME xcs)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

x_add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

if(X_LIN)
    target_link_libraries(${PROJECT_NAME} dl)
endif(X_LIN)

find_package(Boost 1.55 REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS}) 
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})