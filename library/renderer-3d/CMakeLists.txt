add_library(pulchritude-renderer-3d SHARED)

target_include_directories(pulchritude-renderer-3d PUBLIC "../include/")
target_sources(
  pulchritude-renderer-3d
  PRIVATE
    src/renderer-3d.cpp
)

set_target_properties(
  pulchritude-renderer-3d
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-renderer-3d
  pulchritude-log
  pulchritude-error pulchritude-math pulchritude-platform
  pulchritude-gpu
)

install(
  TARGETS pulchritude-renderer-3d
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
