cmake_minimum_required(VERSION 3.10)
project(NonogramSolver)

add_library(one_line_solver line_solver/impl/row_solver.cpp line_solver/impl/cell.cpp)
target_include_directories(one_line_solver PUBLIC "./line_solver")
set_property(TARGET one_line_solver PROPERTY CXX_STANDARD 17)

add_subdirectory(tests)