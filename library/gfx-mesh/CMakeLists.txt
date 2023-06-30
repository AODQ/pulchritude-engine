add_library(pulchritude-gfx-mesh SHARED)

target_include_directories(pulchritude-gfx-mesh PUBLIC "../include/")
target_sources(
  pulchritude-gfx-mesh
  PRIVATE
    src/mesh.cpp
)

set_target_properties(
  pulchritude-gfx-mesh
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(pulchritude-gfx-mesh)

install(
  TARGETS pulchritude-gfx-mesh
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
