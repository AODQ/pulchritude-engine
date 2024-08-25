add_library(pulchritude-scene SHARED)

target_include_directories(pulchritude-scene PUBLIC "../include/")
target_sources(
  pulchritude-scene
  PRIVATE
    src/scene.cpp
)

set_target_properties(
  pulchritude-scene
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-scene

  pulchritude-camera
  pulchritude-ecs
  pulchritude-error
  pulchritude-gpu
  pulchritude-log
  pulchritude-math
  pulchritude-physx
  pulchritude-platform
  pulchritude-render-graph
)

install(
  TARGETS pulchritude-scene
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
