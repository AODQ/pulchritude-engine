add_library(pulchritude-asset-pds SHARED)

target_include_directories(pulchritude-asset-pds PUBLIC "../include/")
target_sources(
  pulchritude-asset-pds
  PRIVATE
    src/asset-pds-read.cpp
    src/asset-pds-write.cpp
)

set_target_properties(
  pulchritude-asset-pds
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic \
         -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  pulchritude-asset-pds
    pulchritude-allocator
    pulchritude-data-serializer
    pulchritude-error
    pulchritude-file
    pulchritude-stream
    pulchritude-string
    pulchritude-time
)

install(
  TARGETS pulchritude-asset-pds
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
