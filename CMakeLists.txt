cmake_minimum_required(VERSION 3.10)
project(NonogramSolver)

add_library(one_row_solver row_solver/row_solver.cpp row_solver/cell.cpp)
target_include_directories(one_row_solver PUBLIC "./row_solver")

add_executable(testing main.cpp row_solver_bf.cpp)
target_link_libraries(testing PRIVATE one_row_solver)

set_property(TARGET testing PROPERTY CXX_STANDARD 17)
set_property(TARGET one_row_solver PROPERTY CXX_STANDARD 17)