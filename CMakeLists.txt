cmake_minimum_required(VERSION 3.14)
project(finalProject C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -Wall -Wextra -Werror -pedantic-errors")
add_executable(finalProject main.c List.c List.h IOHandler.c IOHandler.h Game.c Game.h Solver.h Solver.c Stack.h Stack.c MainAux.c MainAux.h Parser.h Parser.c)