cmake_minimum_required(VERSION 3.1)

project(MyProject
    VERSION 1.0
    DESCRIPTION "My new project."
    LANGUAGES C)

set(TARGET_NAME "myproj")

add_executable(${TARGET_NAME}
    src/main.c)

target_include_directories(${TARGET_NAME} PUBLIC include)
target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Werror -Wpedantic)

