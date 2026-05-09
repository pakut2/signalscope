if(NOT CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

add_library(audio_decoder SHARED
    audio_decoder.c
    coreaudio/coreaudio_tap.c
    coreaudio/coreaudio_tap.m
    miniaudio/miniaudio.c
)

target_include_directories(audio_decoder PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(audio_decoder PRIVATE
    -fvisibility=hidden
    $<$<CONFIG:Debug>:-Wall -Wextra -fsanitize=address>
)

target_link_options(audio_decoder PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=address>
)

set_source_files_properties(coreaudio/coreaudio_tap.m PROPERTIES
    LANGUAGE OBJC
)

target_link_libraries(audio_decoder PRIVATE
    "-framework Foundation"
    "-framework CoreAudio"
)

