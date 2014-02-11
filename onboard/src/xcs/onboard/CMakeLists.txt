set(PROJECT_NAME onboard)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)

include_directories(${Boost_INCLUDE_DIR})

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} xcs)

if(X_LIN)
    target_link_libraries(${PROJECT_NAME} dl)
endif(X_LIN)

