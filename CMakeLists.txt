cmake_minimum_required(VERSION 4.2)
project(MusicPlayer)

option(LTO "Enable Link Time Optimization (LTO/IPO)" OFF)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT error)

if(LTO AND ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

find_library(PULSEAUDIO_LIBRARY NAMES pulse DOC "PulseAudio Library is required in backend.")
find_path(PULSEAUDIO_INCLUDE_DIR NAMES pulse/simple.h DOC "PulseAudio head files.")
include_directories(${PULSEAUDIO_INCLUDE_DIR})

add_executable(main main.c strapi.c parser.c backend.c wave.c dataapi.c note.c math.c)

if (NOT MSVC)
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

set_source_files_properties(wave.c math.c
    PROPERTIES
    COMPILE_OPTIONS
        "$<$<NOT:$<CONFIG:Debug>>:${FLOAT_OPTI_CFLAGS}>;$<$<CONFIG:Debug>:-Og>"
)

endif()

target_link_libraries(main pulse-simple m)

add_compile_options(-g -Wall)
