cmake_minimum_required(VERSION 3.23)

set(CMAKE_C_COMPILER gcc)

project(signalscope C)

file(GLOB DIR_SRC_FILES CONFIGURE_DEPENDS
    "utils/*.c"
)

add_executable(signalscope
    main.c
    spectrum_analyzer.c
    audio_decoder.c
    renderer.c
    lib/miniaudio.c
    ${DIR_SRC_FILES}
)

target_include_directories(signalscope PRIVATE
    .
    utils
    libs
)

target_compile_options(signalscope PRIVATE
    -Wall
    -Wextra
)

# TODO only for debug
target_link_options(signalscope PRIVATE
    -fsanitize=address
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Curses REQUIRED)
target_link_libraries(signalscope PRIVATE
    ${CURSES_LIBRARIES}
)

