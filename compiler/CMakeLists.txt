cmake_minimum_required(VERSION 3.16)
project(stm32_vm_host C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Inc
)

# Old C compiler 
add_executable(compiler_c
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Src/opcodes.c
    main.c
)

target_compile_options(compiler_c PRIVATE -Wall -Wextra -O0 -g)


# New C++ compiler 
add_executable(compiler_cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Src/opcodes.c
    main.cpp
)

target_compile_options(compiler_cpp PRIVATE -Wall -Wextra -O0 -g)

# Link C opcodes.c with the C++ binary correctly
set_property(TARGET compiler_cpp PROPERTY LINKER_LANGUAGE CXX)
