#
# Substitutions for standard cmake add_library and target_link_libraries
# for our own libraries.
# It's purpose is to correctly add __declspec attributes on Windows.
#
# See: http://stackoverflow.com/questions/12738096/generate-export-header-typical-usage
#

include(GenerateExportHeader)

function(x_add_library name type)
    if(NOT (${type} STREQUAL "SHARED"))
        message(SEND_ERROR "x_add_library is intended for SHARED libraries only (${type} given).")
    endif()
    add_library(${name} ${type} ${ARGN})
    GENERATE_EXPORT_HEADER(${name})
    include_directories(${CMAKE_BINARY_DIR})
endfunction(x_add_library)

function(x_target_link_libraries target)
    target_link_libraries(${target} ${ARGN})
    include_directories(${CMAKE_BINARY_DIR})
endfunction(x_target_link_libraries)