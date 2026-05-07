cmake_minimum_required(VERSION 3.23)

set(CMAKE_C_COMPILER clang)

project(signalscope C OBJC)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

file(GLOB DIR_SRC_FILES CONFIGURE_DEPENDS
    "utils/*.c"
)

add_executable(signalscope MACOSX_BUNDLE
    main.c
    spectrum_analyzer.c
    audio_decoder.c
    renderer.c
    lib/miniaudio.c
    coreaudio/coreaudio_tap.c
    coreaudio/coreaudio_tap.m
    ${DIR_SRC_FILES}
)

target_include_directories(signalscope PRIVATE
    .
    utils
    lib
    coreaudio
)

target_compile_options(signalscope PRIVATE
    -Wall
    -Wextra
)

set_source_files_properties(coreaudio/coreaudio_tap.m PROPERTIES
    LANGUAGE OBJC
)

# TODO only for debug
target_link_options(signalscope PRIVATE
    -fsanitize=address
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Curses REQUIRED)

target_link_libraries(signalscope PRIVATE
    ${CURSES_LIBRARIES}
    "-framework Foundation"
    "-framework CoreAudio"
)

set_target_properties(signalscope PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Info.plist"
    MACOSX_BUNDLE_BUNDLE_NAME "SignalScope"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.pakut2.signalscope"
    MACOSX_BUNDLE_BUNDLE_VERSION "1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
)
