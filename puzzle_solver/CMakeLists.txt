add_library(puzzle_solver impl/solver.cpp impl/puzzle_data.cpp)

target_include_directories(puzzle_solver PUBLIC .)