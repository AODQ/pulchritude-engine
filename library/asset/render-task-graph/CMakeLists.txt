add_library(pulchritude-asset-render-task-graph SHARED)

target_include_directories(
  pulchritude-asset-render-task-graph PUBLIC "../include/"
)
target_sources(
  pulchritude-asset-render-task-graph
  PRIVATE
    src/asset-render-task-graph.cpp
)

set_target_properties(
  pulchritude-asset-render-task-graph
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  pulchritude-asset-render-task-graph
    pulchritude-allocator
    pulchritude-asset-pds
    pulchritude-gfx pulchritude-platform
    pulchritude-data-serializer
    pulchritude-error
    pulchritude-file
    pulchritude-stream
)

install(
  TARGETS pulchritude-asset-render-task-graph
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
