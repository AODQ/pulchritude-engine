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
        "-Wshadow -Wall -Wformat=2 -Wextra -Wundef"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-imgui
  pulchritude-gpu pulchritude-platform pulchritude-error
  pulchritude-asset-render-graph
  pulchritude-asset-shader-module
  pulchritude-task-graph
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
