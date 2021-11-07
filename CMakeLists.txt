cmake_minimum_required(VERSION 3.1)

project(JACE
    VERSION 1.0
    DESCRIPTION "Just Another Chip-8 Emulator."
    LANGUAGES C)

add_executable("jace"
    src/main.c
    src/chip8.c)

target_include_directories("jace" PUBLIC include)
target_compile_options("jace" PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries("jace" -lSDL2 SDL2_ttf m)

add_executable("test"
    tests/test_opcodes.c
    src/chip8.c)

target_include_directories("test" PUBLIC include)
target_compile_options("test" PRIVATE -Wall -Wextra -Wpedantic)