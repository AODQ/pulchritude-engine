add_library(pulchritude-gfx SHARED)

target_include_directories(pulchritude-gfx PUBLIC "../include/")
target_sources(
  pulchritude-gfx
  PRIVATE
    src/gfx.cpp
)

set_target_properties(
  pulchritude-gfx
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic \
         -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(pulchritude-gfx glfw)

install(
  TARGETS pulchritude-gfx
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
