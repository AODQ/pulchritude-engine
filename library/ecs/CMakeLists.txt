add_library(pulchritude-ecs SHARED)

target_include_directories(pulchritude-ecs PUBLIC "../include/")
target_sources(
  pulchritude-ecs
  PRIVATE
    src/ecs.cpp
)

set_target_properties(
  pulchritude-ecs
    PROPERTIES
      COMPILE_FLAGS "-Wshadow -Wdouble-promotion -Wformat=2 -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  pulchritude-ecs
    pulchritude-error
    pulchritude-log
    flecs_static
)

install(
  TARGETS pulchritude-ecs
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
