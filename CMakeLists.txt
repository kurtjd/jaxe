cmake_minimum_required(VERSION 3.1)

project(JAXE
    VERSION 1.0
    DESCRIPTION "Just Another Chip-8 Emulator."
    LANGUAGES C)

add_executable("jaxe"
    src/main.c
    src/chip8.c)

target_include_directories("jaxe" PUBLIC include)
target_compile_options("jaxe" PRIVATE -Wall -Wextra -Wpedantic)

if (WIN32)
    target_link_libraries("jaxe" -lmingw32 -lSDL2main -lSDL2 SDL2_ttf m)
else (UNIX)
    target_link_libraries("jaxe" -lSDL2 SDL2_ttf m)
endif (WIN32)

add_executable("test"
    tests/test_opcodes.c
    src/chip8.c)

target_include_directories("test" PUBLIC include)
target_compile_options("test" PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries("test" -lm)