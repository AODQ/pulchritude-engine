add_library(pulchritude-stream SHARED)

target_include_directories(pulchritude-stream PUBLIC "../include/")
target_sources(
  pulchritude-stream
  PRIVATE
    src/stream.c
)

set_target_properties(
  pulchritude-stream
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(pulchritude-stream pulchritude-string pulchritude-array)

install(
  TARGETS pulchritude-stream
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
