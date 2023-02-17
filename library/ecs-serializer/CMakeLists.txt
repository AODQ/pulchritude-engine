add_library(pulchritude-ecs-serializer SHARED)

target_include_directories(pulchritude-ecs-serializer PUBLIC "../include/")
target_sources(
  pulchritude-ecs-serializer
  PRIVATE src/ecs-serializer.cpp
)

set_target_properties(
  pulchritude-ecs-serializer
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  pulchritude-ecs-serializer
    pulchritude-ecs
    pulchritude-error
    pulchritude-log
    pulchritude-data-serializer
    pulchritude-string
)

install(
  TARGETS pulchritude-ecs-serializer
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
