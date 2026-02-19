cmake_minimum_required(VERSION 4.2)
project(MusicPlayer)

find_library(PULSEAUDIO_LIBRARY NAMES pulse DOC "PulseAudio Library is required in backend.")
find_path(PULSEAUDIO_INCLUDE_DIR NAMES pulse/simple.h DOC "PulseAudio head files.")
include_directories(${PULSEAUDIO_INCLUDE_DIR})

add_executable(main main.c strapi.c parser.c backend.c wave.c dataapi.c note.c)
target_link_libraries(main pulse-simple m)

add_compile_options(-g -Wall)