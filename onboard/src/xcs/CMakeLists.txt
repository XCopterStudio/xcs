set(PROJECT_NAME xcs)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)

x_add_library(${PROJECT_NAME} ${SOURCES})

if(X_LIN)
    target_link_libraries(${PROJECT_NAME} dl)
endif(X_LIN)
