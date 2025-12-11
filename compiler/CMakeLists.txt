cmake_minimum_required(VERSION 3.16)
project(stm32_vm_host C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Inc
)

add_executable(compiler
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Src/opcodes.c
    main.cpp
)

# Link C opcodes.c with the C++ binary correctly
set_property(TARGET compiler PROPERTY LINKER_LANGUAGE CXX)
