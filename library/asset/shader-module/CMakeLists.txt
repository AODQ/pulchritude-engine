add_library(pulchritude-asset-shader-module SHARED)

target_include_directories(
  pulchritude-asset-shader-module PUBLIC "../include/"
)
target_sources(
  pulchritude-asset-shader-module
  PRIVATE
    src/asset-shader-module.cpp
)

set_target_properties(
  pulchritude-asset-shader-module
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-asset-shader-module
    pulchritude-data-serializer
    pulchritude-error
    pulchritude-array
    pulchritude-string
    pulchritude-file
    pulchritude-time

    schrift
)

install(
  TARGETS pulchritude-asset-shader-module
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
