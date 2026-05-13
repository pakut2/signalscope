cmake_minimum_required(VERSION 3.23)

add_library(spectrum_analyzer SHARED
  spectrum_analyzer.c
  ring_buffer.c
)

target_include_directories(spectrum_analyzer PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(spectrum_analyzer PRIVATE
  -fvisibility=hidden
  $<$<CONFIG:Debug>:-Wall -Wextra -fsanitize=address>
)

target_link_options(spectrum_analyzer PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=address>
)

