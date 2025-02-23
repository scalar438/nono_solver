
add_executable(puzzle_solver_cli main.cpp puzzle_reader.cpp puzzle_data.cpp)
target_link_libraries(puzzle_solver_cli PRIVATE line_solver)