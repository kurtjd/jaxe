cmake_minimum_required(VERSION 3.1)

project(JACE
    VERSION 1.0
    DESCRIPTION "Just Another Chip-8 Emulator."
    LANGUAGES C)

set(TARGET_NAME "jace")

add_executable(${TARGET_NAME}
    src/main.c
    src/chip8.c)

target_include_directories(${TARGET_NAME} PUBLIC include)
target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(${TARGET_NAME} -lSDL2)

add_executable("test"
    tests/test.c
    src/chip8.c)

target_include_directories("test" PUBLIC include)
target_compile_options("test" PRIVATE -Wall -Wextra -Wpedantic)