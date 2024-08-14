add_library(pulchritude-mt SHARED)

target_include_directories(pulchritude-mt PUBLIC "../include/")
target_sources(
  pulchritude-mt
  PRIVATE
    src/mt.cpp
)

set_target_properties(
  pulchritude-mt
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-mt
  pulchritude-error pulchritude-allocator pulchritude-log
)

install(
  TARGETS pulchritude-mt
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
