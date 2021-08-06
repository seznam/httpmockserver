cmake_minimum_required(VERSION 3.0)
project(httpmockserver LANGUAGES CXX)

message(STATUS "Checking compiler C++11 support.")
if(CMAKE_VERSION VERSION_LESS "3.1")
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
    if(COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
    endif()
else()
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

add_compile_options(-O2 -pipe -Wall -Wextra -Wfatal-errors -Wformat-security -Wuninitialized -Wfloat-equal -g -fPIC)


# Setting options of command cmake. Copied from whoshuu/cpr
macro(mockserver_option OPTION_NAME OPTION_TEXT OPTION_DEFAULT)
    option(${OPTION_NAME} ${OPTION_TEXT} ${OPTION_DEFAULT})
    if(DEFINED ENV{${OPTION_NAME}})
        # Allow setting the option through an environment variable
        set(${OPTION_NAME} $ENV{${OPTION_NAME}})
    endif()
    if(${OPTION_NAME})
        add_definitions(-D${OPTION_NAME})
    endif()
    message(STATUS "  ${OPTION_NAME}: ${${OPTION_NAME}}")
endmacro()

message(STATUS "httpmockserver CMake Options")
message(STATUS "=======================================================")
mockserver_option(USE_ALL_SYSTEM_LIBS "If ON, all dependencies will be searched in system paths." OFF)
if(USE_ALL_SYSTEM_LIBS)
    set(USE_SYSTEM_CPR ON)
    set(USE_SYSTEM_GTEST ON)
else()
    mockserver_option(USE_SYSTEM_CPR "If ON, CPR library will be searched in system paths." OFF)
    mockserver_option(USE_SYSTEM_CURL "If ON, CURL for CPR library will be searched in system paths." OFF)
    mockserver_option(USE_SYSTEM_GTEST "If ON, GTest library will be searched in system paths." OFF)
endif()
message(STATUS "=======================================================")

###

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(MHD MODULE REQUIRED)

set(HTTPMOCKSERVER_LIBRARIES httpmockserver ${MHD_LIBRARIES} CACHE INTERNAL "")
set(HTTPMOCKSERVER_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "")

add_subdirectory(external)
add_subdirectory(src)

enable_testing()
add_subdirectory(test)

install(DIRECTORY ${HTTPMOCKSERVER_INCLUDE_DIRS}/httpmockserver
        DESTINATION include FILES_MATCHING PATTERN "*.h")

# set pkg-config file
set(prefix "${CMAKE_INSTALL_PREFIX}")
configure_file("libhttpmockserver.pc.in" "libhttpmockserver.pc" @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libhttpmockserver.pc DESTINATION lib/pkgconfig)

