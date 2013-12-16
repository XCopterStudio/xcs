cmake_minimum_required(VERSION 2.8)
project(onboard)

set (CMAKE_CXX_FLAGS "-std=c++0x")

# pthreads
find_package (Threads)

# Boost
add_definitions(-DBOOST_ALL_NO_LIB)
set(Boost_USE_STATIC_LIBS true)
find_package(Boost REQUIRED COMPONENTS system thread) 

include_directories(${Boost_INCLUDE_DIRS}) 
add_executable(main src/XCI_Parrot.cpp src/AT_Command.cpp)

# Linked libs
target_link_libraries(main ${Boost_LIBRARIES})                                                                                                                                                       
target_link_libraries (main ${CMAKE_THREAD_LIBS_INIT})


