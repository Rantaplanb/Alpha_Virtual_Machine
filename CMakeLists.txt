cmake_minimum_required(VERSION 3.19)
project(avm C)

set(CMAKE_C_STANDARD 11)

add_executable(avm src/avm.c src/loader.c src/utilities.c src/state.c src/memory/memory.c src/memory/table.c src/memory/stack.c src/libs/hash_map.c src/ALU/dispatcher.c src/ALU/exec_arithmetic.c src/ALU/exec_branches.c src/ALU/exec_functions.c src/ALU/exec_tables.c)
