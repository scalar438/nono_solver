cmake_minimum_required(VERSION 3.20)
project(NonogramSolver)

set(CMAKE_CXX_STANDARD 20)

add_library(line_solver line_solver/impl/line_solver.cpp line_solver/impl/cell.cpp)
target_include_directories(line_solver PUBLIC ".")

add_subdirectory(puzzle_solver_cli)

add_subdirectory(tests)