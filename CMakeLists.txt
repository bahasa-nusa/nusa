cmake_minimum_required(VERSION 4.4)

project(
    Nusa
    VERSION 0.0.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ccache
find_program(CCACHE_PROGRAM "ccache")
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    message(STATUS "Nusa: ccache ditemukan - ${CCACHE_PROGRAM}")
else()
    message(STATUS "Nusa: ccache tidak ditemukan, kompilasi tanpa cache")
endif()

# nusa
file(
    GLOB_RECURSE
    SEMUA_FILE_SUMBER_NUSA
    "src/nusa/*.c"
)

add_executable(
    nusa
    ${SEMUA_FILE_SUMBER_NUSA}
)

target_include_directories(nusa PRIVATE include)