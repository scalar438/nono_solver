add_library(line_solver_bf line_solver_bf/line_solver_bf.cpp)
target_include_directories(line_solver_bf INTERFACE ./line_solver_bf)
target_link_libraries(line_solver_bf PRIVATE line_solver)

add_executable(timus_sol timus/solution/main.cpp)
target_compile_definitions(timus_sol PRIVATE MY_LOCAL_ONLINE_JUDGE)

add_executable(test_timus timus/testing/main.cpp)
target_include_directories(test_timus PRIVATE timus)
target_link_libraries(test_timus PRIVATE line_solver)
target_link_libraries(test_timus PRIVATE line_solver_bf)
target_link_libraries(test_timus PRIVATE -lpthread)
target_compile_definitions(test_timus PRIVATE TESTING_ONLY)

add_executable(line_solver_testing line_solver_testing/line_solver_testing.cpp line_solver_testing/iterators.cpp)
target_link_libraries(line_solver_testing PRIVATE line_solver_bf)
target_link_libraries(line_solver_testing PRIVATE line_solver)
target_link_libraries(line_solver_testing PRIVATE -lpthread)

add_executable(spoj_solution spoj/main.cpp)
target_compile_definitions(spoj_solution PRIVATE MY_LOCAL_ONLINE_JUDGE)
# Sphere online judge accepts only C++14
set_property(TARGET spoj_solution PROPERTY CXX_STANDARD 14)