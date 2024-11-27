add_library(pulchritude-asset-sprite SHARED)

target_include_directories(pulchritude-asset-sprite PUBLIC "../include/")
target_sources(
  pulchritude-asset-sprite
  PRIVATE
    src/asset-sprite.cpp
)

set_target_properties(
  pulchritude-asset-sprite
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-asset-sprite
    pulchritude-allocator
    pulchritude-array
    pulchritude-data-serializer
    pulchritude-error
    pulchritude-file
    pulchritude-gpu
    pulchritude-math
    pulchritude-stream
    pulchritude-time
)

install(
  TARGETS pulchritude-asset-sprite
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
