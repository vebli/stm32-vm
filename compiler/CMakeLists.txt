cmake_minimum_required(VERSION 3.16)
project(stm32_vm_host C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Inc
)

add_executable(compiler
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Src/opcodes.c
    main.c   
)

target_compile_options(compiler PRIVATE -Wall -Wextra -O0 -g)
