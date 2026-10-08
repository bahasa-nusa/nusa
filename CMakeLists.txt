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

# llvm
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

llvm_map_components_to_libnames(llvm_libs support core)

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

target_include_directories(
    nusa
    PRIVATE
    "include"
    ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(
    nusa
    PRIVATE
    ${llvm_libs}
)

# install
install(
    TARGETS nusa
    DESTINATION "."
)

install(
    DIRECTORY "kode/"
    DESTINATION "kode"
)

install(
    DIRECTORY so/wins/32/
    DESTINATION so/wins/32
)

install(
    DIRECTORY so/wins/64/
    DESTINATION so/wins/64
)

install(
    FILES "LICENSE"
    DESTINATION "."
)

# std.a dari bahasa rakitan
add_subdirectory(src/std)