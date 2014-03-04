set(PROJECT_NAME test)
project(${PROJECT_NAME})


file(GLOB SOURCES *.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

x_target_link_libraries(${PROJECT_NAME} xcs) # experiment for better linking on Windows, but don't know where the symbols could be referenced for


