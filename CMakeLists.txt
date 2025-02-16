cmake_minimum_required(VERSION 3.10)

project(cadical-interface)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CONFIGURE_COMMAND configure --debug)
else()
    set(CONFIGURE_COMMAND configure)
endif()

include(ExternalProject)
ExternalProject_Add(cadical
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cadical
    CONFIGURE_COMMAND CXXFLAGS=-fPIC ${CMAKE_CURRENT_SOURCE_DIR}/cadical/${CONFIGURE_COMMAND}
    BUILD_COMMAND make
    INSTALL_COMMAND ""
    BUILD_IN_SOURCE 1
)

# Set the C++ Standard to C++20.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(interrupt src/interrupt.cpp include/interrupt.hpp)
target_include_directories(interrupt PUBLIC include)

add_library(cadical_solver src/cadical_solver.cpp include/cadical_solver.hpp)
target_include_directories(cadical_solver PUBLIC include ${CMAKE_CURRENT_SOURCE_DIR}/cadical/src/)
add_dependencies(cadical_solver cadical)
target_link_libraries(cadical_solver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/cadical/build/libcadical.a interrupt)
