add_library(pulchritude-text SHARED)

target_include_directories(pulchritude-text PUBLIC "../include/")
target_sources(
  pulchritude-text
  PRIVATE
    src/text.cpp
)

set_target_properties(
  pulchritude-text
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-text

  pulchritude-allocator
  pulchritude-asset-font
  pulchritude-error
  pulchritude-gpu
  pulchritude-log
)

install(
  TARGETS pulchritude-text
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
