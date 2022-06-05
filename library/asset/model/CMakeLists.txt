add_library(pulchritude-asset-model SHARED)

target_include_directories(pulchritude-asset-model PUBLIC "../include/")
target_sources(
  pulchritude-asset-model
  PRIVATE
    src/asset-model-read.cpp
)

set_target_properties(
  pulchritude-asset-model
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic \
         -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  pulchritude-asset-model
    pulchritude-allocator
    pulchritude-asset-pds
    pulchritude-data-serializer
    pulchritude-error
    pulchritude-file
    pulchritude-stream
)

install(
  TARGETS pulchritude-asset-model
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
