
add_executable(puzzle_solver_cli main.cpp puzzle_reader.cpp puzzle_data.cpp)
target_link_libraries(puzzle_solver_cli PRIVATE line_solver)
set_property(TARGET puzzle_solver_cli PROPERTY CXX_STANDARD 20)