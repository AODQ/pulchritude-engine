add_library(pulchritude-data-serializer SHARED)

target_include_directories(pulchritude-data-serializer PUBLIC "../include/")
target_sources(
  pulchritude-data-serializer
  PRIVATE src/data-serializer.cpp
)

set_target_properties(
  pulchritude-data-serializer
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-data-serializer
    pulchritude-asset-image
    pulchritude-error
    pulchritude-log
    pulchritude-stream
    pulchritude-string
)

install(
  TARGETS pulchritude-data-serializer
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
