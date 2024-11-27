add_library(pulchritude-pecs SHARED)

target_include_directories(pulchritude-pecs PUBLIC "../include/")
target_sources(
  pulchritude-pecs
  PRIVATE
    src/pecs.cpp
)

set_target_properties(
  pulchritude-pecs
    PROPERTIES
      COMPILE_FLAGS "-Wshadow -Wdouble-promotion -Wformat=2 -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-pecs
    pulchritude-allocator
    pulchritude-array
    pulchritude-data-serializer
    pulchritude-error
    pulchritude-log
    pulchritude-string
)

install(
  TARGETS pulchritude-pecs
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
