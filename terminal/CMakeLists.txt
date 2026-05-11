cmake_minimum_required(VERSION 3.23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_C_COMPILER clang)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(signalscope C OBJC)

file(GLOB DIR_SRC_FILES CONFIGURE_DEPENDS
    "utils/*.c"
)

add_executable(signalscope MACOSX_BUNDLE
    main.c
    renderer.c
    ${DIR_SRC_FILES}
)

add_subdirectory(../audio_decoder ${CMAKE_BINARY_DIR}/audio_decoder)
add_subdirectory(../spectrum_analyzer ${CMAKE_BINARY_DIR}/spectrum_analyzer)

target_include_directories(signalscope PRIVATE
    .
    utils
)

target_compile_options(signalscope PRIVATE
    $<$<CONFIG:Debug>:-Wall -Wextra -fsanitize=address>
)

target_link_options(signalscope PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=address>
)

find_package(Curses REQUIRED)

target_link_libraries(signalscope PRIVATE
    audio_decoder
    spectrum_analyzer
    ${CURSES_LIBRARIES}
)

set_target_properties(signalscope PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/../Info.plist"
)

