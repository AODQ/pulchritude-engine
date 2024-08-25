add_library(pulchritude-allocator SHARED)

target_include_directories(pulchritude-allocator PUBLIC "../include/")
target_sources(
  pulchritude-allocator
  PRIVATE
    src/allocator.cpp
)

set_target_properties(
  pulchritude-allocator
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(pulchritude-allocator pulchritude-log)

install(
  TARGETS pulchritude-allocator
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
