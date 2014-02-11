set(PROJECT_NAME uxci)
project(${PROJECT_NAME})

message("Building UXci...")
include_directories(${Urbi_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})

file(GLOB SOURCES *.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} xci-parrot) # TODO is this static or dynamic linking?

