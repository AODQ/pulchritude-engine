add_library(pulchritude-becs SHARED)

target_include_directories(pulchritude-becs PUBLIC "../include/")
target_sources(
  pulchritude-becs
  PRIVATE
    src/becs.cpp
)

set_target_properties(
  pulchritude-becs
    PROPERTIES
      COMPILE_FLAGS "-Wshadow -Wdouble-promotion -Wformat=2 -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-becs
    pulchritude-error
    pulchritude-data-serializer
    pulchritude-log
)

install(
  TARGETS pulchritude-becs
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
