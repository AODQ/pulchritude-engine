add_library(pulchritude-render-graph SHARED)

target_include_directories(pulchritude-render-graph PUBLIC "../include/")
target_sources(
  pulchritude-render-graph
  PRIVATE
    src/render-graph.cpp
)

set_target_properties(
  pulchritude-render-graph
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-render-graph
    pulchritude-allocator
    pulchritude-file
    pulchritude-gpu
)

install(
  TARGETS pulchritude-render-graph
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
