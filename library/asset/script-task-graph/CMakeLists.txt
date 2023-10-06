add_library(pulchritude-asset-script-task-graph SHARED)

target_include_directories(
  pulchritude-asset-script-task-graph PUBLIC "../include/"
)
target_sources(
  pulchritude-asset-script-task-graph
  PRIVATE
    src/asset-script-task-graph.cpp
)

set_target_properties(
  pulchritude-asset-script-task-graph
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  pulchritude-asset-script-task-graph
    pulchritude-allocator
    pulchritude-asset-pds
    pulchritude-data-serializer
    pulchritude-error
    pulchritude-file
    pulchritude-script
    pulchritude-stream
)

install(
  TARGETS pulchritude-asset-script-task-graph
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
