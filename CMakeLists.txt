cmake_minimum_required(VERSION 3.10)
project(NonogramSolver)

add_library(line_solver line_solver/impl/line_solver.cpp line_solver/impl/cell.cpp)
target_include_directories(line_solver PUBLIC ".")
set_property(TARGET line_solver PROPERTY CXX_STANDARD 20)

add_executable(puzzle_solver_cli puzzle_solver_cli/main.cpp)
target_link_libraries(puzzle_solver_cli PRIVATE line_solver)
set_property(TARGET puzzle_solver_cli PROPERTY CXX_STANDARD 20)

add_subdirectory(tests)