set(PROJECT_NAME onboard)
project(${PROJECT_NAME})

file(GLOB SOURCES *.cpp)

include_directories(${Urbi_INCLUDE_DIRS}) # dependency in DataReceiver
include_directories(${Boost_INCLUDE_DIR})


add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
x_target_link_libraries(${PROJECT_NAME} xcs)
