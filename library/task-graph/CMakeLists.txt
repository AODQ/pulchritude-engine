add_library(pulchritude-task-graph SHARED)

target_include_directories(pulchritude-task-graph PUBLIC "../include/")
target_sources(
  pulchritude-task-graph
  PRIVATE
    src/task-graph.cpp
)

set_target_properties(
  pulchritude-task-graph
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-task-graph
    pulchritude-allocator
    pulchritude-string
)

install(
  TARGETS pulchritude-task-graph
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
