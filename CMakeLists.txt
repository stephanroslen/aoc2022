cmake_minimum_required(VERSION 3.21)
project(aoc2022)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(aoclib)


function(create_executable)
    add_executable(${ARGV})
    target_link_libraries(${ARGV0} aoclib)
    target_compile_options(${ARGV0} PRIVATE -Werror -Wall -Wextra -Wpedantic -Wshadow)
endfunction(create_executable)

create_executable(day01 day01/main.cpp)
create_executable(day02 day02/main.cpp)
create_executable(day03 day03/main.cpp)
create_executable(day04 day04/main.cpp)
create_executable(day05 day05/main.cpp)
create_executable(day06 day06/main.cpp)
create_executable(day07 day07/main.cpp)
create_executable(day08 day08/main.cpp)
create_executable(day09 day09/main.cpp)
create_executable(day10 day10/main.cpp)
create_executable(day11 day11/main.cpp)
create_executable(day12 day12/main.cpp)
create_executable(day13 day13/main.cpp)
create_executable(day14 day14/main.cpp)
create_executable(day15 day15/main.cpp)
create_executable(day16 day16/main.cpp)
create_executable(day17 day17/main.cpp)
create_executable(day18 day18/main.cpp)
create_executable(day19 day19/main.cpp)
create_executable(day20 day20/main.cpp)
create_executable(day21 day21/main.cpp)
create_executable(day22 day22/main.cpp)
create_executable(day23 day23/main.cpp)
create_executable(day24 day24/main.cpp)
create_executable(day25 day25/main.cpp)

