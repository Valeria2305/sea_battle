cmake_minimum_required(VERSION 3.10.0)
add_executable(proj_test test.cpp game.h)
add_executable(proj main.cpp game.h)
target_link_libraries(proj_test PUBLIC doctest::doctest default sfml-graphics sfml-window sfml-system)
target_link_libraries(proj PUBLIC doctest::doctest default sfml-graphics sfml-window sfml-system)
doctest_discover_tests(proj_test)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Micra_Normal.ttf DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
