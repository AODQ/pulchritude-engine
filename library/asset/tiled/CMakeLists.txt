add_library(pulchritude-asset-tiled SHARED)

target_include_directories(pulchritude-asset-tiled PUBLIC "../include/")
target_sources(
  pulchritude-asset-tiled
  PRIVATE
    src/asset-tiled-read.cpp
)

set_target_properties(
  pulchritude-asset-tiled
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  pulchritude-asset-tiled
    pulchritude-allocator
    pulchritude-asset-pds
    pulchritude-data-serializer
    pulchritude-error
    pulchritude-file
    pulchritude-stream
)

install(
  TARGETS pulchritude-asset-tiled
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
