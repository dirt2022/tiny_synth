cmake_minimum_required(VERSION 4.2)
project(MusicPlayer)

add_executable(main main.c strapi.c parser.c backend.c wave.c dataapi.c note.c math.c)

option(LTO "Enable Link Time Optimization (LTO/IPO)" OFF)
option(MULTI-THREAD "Enable multithread support. (OpenMP required)" OFF)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT error)

if(ipo_supported)
    message(STATUS "Link-Time optimization is supported.")
    if (LTO)
        message(STATUS "Link-Time optimization is enabled.")
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()

find_package(OpenMP)
if(OpenMP_C_FOUND)
    message(STATUS "Multithread function is supported on current platform.")
    if(MULTI-THREAD)
        message(STATUS "Multithread function is enabled.")
        target_link_libraries(main OpenMP::OpenMP_C)
    endif()
endif()

find_library(PULSEAUDIO_LIBRARY NAMES pulse DOC "PulseAudio Library is required in backend.")
find_path(PULSEAUDIO_INCLUDE_DIR NAMES pulse/simple.h DOC "PulseAudio head files.")
include_directories(${PULSEAUDIO_INCLUDE_DIR})

if (NOT MSVC)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(FLOAT_OPTI_CFLAGS
        -ffast-math
        -fno-unsafe-math-optimizations
    )
else()
    set(FLOAT_OPTI_CFLAGS
        -fbuiltin
        -ffp-contract=fast
        -funsafe-math-optimizations
        -fno-honor-nans
        -fno-honor-infinities
        -fno-signed-zeros
        -fno-trapping-math
        -fno-rounding-math
        -fcx-limited-range
        -fno-math-errno
        -fno-approx-func
    )
endif()

set_source_files_properties(wave.c math.c
    PROPERTIES
    COMPILE_OPTIONS
        "$<$<NOT:$<CONFIG:Debug>>:${FLOAT_OPTI_CFLAGS}>;$<$<CONFIG:Debug>:-Og>"
)
endif()

target_link_libraries(main pulse-simple m)

add_compile_options(-g -Wall)
