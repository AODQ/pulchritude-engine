# then add library
add_library(pulchritude-imgui SHARED)

target_include_directories(
  pulchritude-imgui
  PUBLIC "../include/"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/imgui"
)

FILE(
  GLOB imgui_source_files
  "${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/imgui/imgui/*.cpp"
)

target_sources(
  pulchritude-imgui
  PRIVATE
    src/imgui.cpp
    ${imgui_source_files}
)

set_target_properties(
  pulchritude-imgui
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wall -Wformat=2 -Wextra -Wpedantic -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  pulchritude-imgui
  pulchritude-gfx pulchritude-platform pulchritude-error
)

install(
  TARGETS pulchritude-imgui
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
