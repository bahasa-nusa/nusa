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

# install
install(
    TARGETS nusa
    DESTINATION "."
)

install(
    DIRECTORY "kode/"
    DESTINATION "kode"
)

file(GLOB SO_WINDOWS_INTEL_32_PUSTAKA_FILES "so/windows/intel_32/pustaka/*.a")
install(
    FILES ${SO_WINDOWS_INTEL_32_PUSTAKA_FILES}
    DESTINATION "so/windows/intel_32/pustaka"
)

file(GLOB SO_WINDOWS_INTEL_64_PUSTAKA_FILES "so/windows/intel_64/pustaka/*.a")
install(
    FILES ${SO_WINDOWS_INTEL_64_PUSTAKA_FILES}
    DESTINATION "so/windows/intel_64/pustaka"
)

file(GLOB SO_WINDOWS_INTEL_32_FILES "so/windows/intel_32/*.exe" "so/windows/intel_32/*.dll")
install(
    FILES ${SO_WINDOWS_INTEL_32_FILES}
    DESTINATION "so/windows/intel_32"
)

file(GLOB SO_WINDOWS_INTEL_64_FILES "so/windows/intel_64/*.exe" "so/windows/intel_64/*.dll")
install(
    FILES ${SO_WINDOWS_INTEL_64_FILES}
    DESTINATION "so/windows/intel_64"
)

install(
    FILES "LICENSE"
    DESTINATION "."
)