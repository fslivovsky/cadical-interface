cmake_minimum_required(VERSION 3.10)

project(cadical_bindings)

# Set the C++ Standard to C++20.
set(CMAKE_CXX_STANDARD 20)

# Find Pybind11
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG)

if(pybind11_FOUND)
  pybind11_add_module(cadical_py MODULE bindings.cpp)
  target_link_libraries(cadical_py PRIVATE cadical_solver pybind11::module)
  target_include_directories(cadical_py PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src/)
else()
    message(WARNING "Pybind11 not found - Python bindings will not be built.")
endif()
