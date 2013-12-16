cmake_minimum_required(VERSION 2.8)
project(onboard)

set (CMAKE_CXX_FLAGS "-std=c++0x")

find_package(Boost REQUIRED COMPONENTS system thread) 

include_directories(${Boost_INCLUDE_DIRS}) 
add_executable(main src/XCI_Parrot.cpp src/AT_Command.cpp)
target_link_libraries(main ${Boost_LIBRARIES})                                                                                                                                                       


