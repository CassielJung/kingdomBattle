cmake_minimum_required(VERSION 3.21)
project(Project_C C)

set(CMAKE_C_STANDARD 99)

add_executable(Project_C main.c User_Input.c Card_Function.c ultimate_function.c ultimate_function.h input_test_out.c input_test_out.h)
