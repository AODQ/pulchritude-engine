add_library(pulchritude-asset-font SHARED)

target_include_directories(pulchritude-asset-font PUBLIC "../include/")
target_sources(
  pulchritude-asset-font
  PRIVATE
    src/asset-font.cpp
)

set_target_properties(
  pulchritude-asset-font
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  pulchritude-asset-font
    pulchritude-data-serializer
    pulchritude-error
    pulchritude-array
    pulchritude-string

    schrift
)

install(
  TARGETS pulchritude-asset-font
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
