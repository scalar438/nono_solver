cmake_minimum_required(VERSION 3.10)
project(NonogramSolver)

add_library(one_row_solver row_solver/impl/row_solver.cpp row_solver/impl/cell.cpp)
target_include_directories(one_row_solver PUBLIC "./row_solver")
set_property(TARGET one_row_solver PROPERTY CXX_STANDARD 17)

add_subdirectory(tests)