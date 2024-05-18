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
  pulchritude-log
  pulchritude-error pulchritude-math pulchritude-platform
  pulchritude-gpu
  pulchritude-camera
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
