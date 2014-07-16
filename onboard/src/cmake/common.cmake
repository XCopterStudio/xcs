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

function(add_x_object TARGET_NAME)
    add_library(${TARGET_NAME} MODULE ${ARGN})
    set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "")

    x_target_link_libraries(${TARGET_NAME} xobject)

    find_package(UrbiAll)
    use_urbi(${TARGET_NAME})
endfunction(add_x_object)