cmake_minimum_required(VERSION 3.10)
project(NonogramSolver)

add_executable(main main.cpp row_solver.cpp cell.cpp row_solver_bf.cpp)
set_property(TARGET main PROPERTY CXX_STANDARD 17)