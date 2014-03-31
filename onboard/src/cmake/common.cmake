#
# Common utility macros for subprojects
#

function(use_logging TARGET_NAME)

find_package(Boost 1.55 REQUIRED COMPONENTS log)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} ${Boost_LIBRARIES})
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)

find_package (Threads)
target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

endfunction(use_logging)
