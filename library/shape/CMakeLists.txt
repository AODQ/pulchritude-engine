add_library(pulchritude-shape SHARED)

target_include_directories(
  pulchritude-shape
  PUBLIC "../include/"
)
target_sources(
  pulchritude-shape
  PRIVATE
    src/shape.cpp
    src/shape-surface-mesh.cpp
)

set_target_properties(
  pulchritude-shape
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-shape
  pulchritude-math
  pulchritude-allocator
  pulchritude-array

  pmp
)

install(
  TARGETS pulchritude-shape
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
