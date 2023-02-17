add_library(pulchritude-gfx-debug SHARED)

target_include_directories(
  pulchritude-gfx-debug
  PUBLIC "../include/"
  PRIVATE "include/"
)
target_sources(
  pulchritude-gfx-debug
  PRIVATE
    src/gfx-debug.cpp
)

set_target_properties(
  pulchritude-gfx-debug
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  pulchritude-gfx-debug
    pulchritude-error
    pulchritude-log
    pulchritude-platform
)

install(
  TARGETS pulchritude-gfx-debug
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
