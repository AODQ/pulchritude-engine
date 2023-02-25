add_library(pulchritude-mesh SHARED)

target_include_directories(pulchritude-mesh PUBLIC "../include/")
target_sources(
  pulchritude-mesh
  PRIVATE
    src/mesh.cpp
)

set_target_properties(
  pulchritude-mesh
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(pulchritude-mesh)

install(
  TARGETS pulchritude-mesh
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
