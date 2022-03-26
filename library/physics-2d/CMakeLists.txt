add_library(pulchritude-physics-2d SHARED)

target_include_directories(pulchritude-physics-2d PUBLIC "../include/")
target_sources(
  pulchritude-physics-2d
  PRIVATE src/physics-2d.cpp
)

set_target_properties(
  pulchritude-physics-2d
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic \
         -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  pulchritude-physics-2d
  pulchritude-error pulchritude-log pulchritude-array
  box2d
)

install(
  TARGETS pulchritude-physics-2d
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
