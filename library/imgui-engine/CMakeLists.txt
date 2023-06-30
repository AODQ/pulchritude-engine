# then add library
add_library(pulchritude-imgui-engine SHARED)

target_include_directories(pulchritude-imgui-engine PUBLIC "../include/")

target_sources(
  pulchritude-imgui-engine
  PRIVATE
    src/imgui-engine.cpp
)

set_target_properties(
  pulchritude-imgui-engine
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wall -Wformat=2 -Wextra -Wundef"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-imgui-engine
  pulchritude-imgui
  pulchritude-gpu
  pulchritude-ecs
  pulchritude-platform
  pulchritude-error
)

install(
  TARGETS pulchritude-imgui-engine
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
