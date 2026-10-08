# Gubah src/std/wins/[32|64]/*.s -> build/wins/[32|64]/std.a
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    return()
endif()

find_program(AS_32 "as.exe" PATHS "${CMAKE_SOURCE_DIR}/so/wins/32" NO_DEFAULT_PATH)
find_program(AR_32 "ar.exe" PATHS "${CMAKE_SOURCE_DIR}/so/wins/32" NO_DEFAULT_PATH)
find_program(AS_64 "as.exe" PATHS "${CMAKE_SOURCE_DIR}/so/wins/64" NO_DEFAULT_PATH)
find_program(AR_64 "ar.exe" PATHS "${CMAKE_SOURCE_DIR}/so/wins/64" NO_DEFAULT_PATH)

function(std_a_arsip arsat as_prog ar_prog)
    file(GLOB sumber "${CMAKE_CURRENT_SOURCE_DIR}/${arsat}/*.s")
    set(obj "")
    foreach(s ${sumber})
        get_filename_component(nama "${s}" NAME_WE)
        set(o "${CMAKE_BINARY_DIR}/wins/${arsat}/${nama}.o")
        add_custom_command(OUTPUT "${o}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/wins/${arsat}"
            COMMAND ${as_prog} -c "${s}" -o "${o}"
            DEPENDS "${s}"
            COMMENT "Gubah ${nama}.s (${arsat})")
        list(APPEND obj "${o}")
    endforeach()
    set(stda "${CMAKE_BINARY_DIR}/wins/${arsat}/std.a")
    add_custom_command(OUTPUT "${stda}"
        COMMAND ${ar_prog} rcs "${stda}" ${obj}
        DEPENDS ${obj}
        COMMENT "Gubah std.a (Windows ${arsat})")
    add_custom_target(std_a_${arsat} ALL DEPENDS "${stda}")
    install(FILES "${stda}" DESTINATION "so/wins/${arsat}/pus")
endfunction()

if(AS_32 AND AR_32)
    std_a_arsip(32 ${AS_32} ${AR_32})
endif()
if(AS_64 AND AR_64)
    std_a_arsip(64 ${AS_64} ${AR_64})
endif()