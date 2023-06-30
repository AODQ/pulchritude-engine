add_library(pulchritude-asset-render-graph SHARED)

target_include_directories(
  pulchritude-asset-render-graph PUBLIC "../include/"
)
target_sources(
  pulchritude-asset-render-graph
  PRIVATE
    src/asset-render-graph.cpp
)

set_target_properties(
  pulchritude-asset-render-graph
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-asset-render-graph
    pulchritude-allocator
    pulchritude-asset-pds
    pulchritude-render-graph
    pulchritude-gfx pulchritude-platform
    pulchritude-data-serializer
    pulchritude-error
    pulchritude-file
    pulchritude-stream
)

install(
  TARGETS pulchritude-asset-render-graph
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
