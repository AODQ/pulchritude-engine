add_library(pulchritude-file SHARED)

target_include_directories(pulchritude-file PUBLIC "../include/")
target_sources(
  pulchritude-file
  PRIVATE src/file.cpp
)

set_target_properties(
  pulchritude-file
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-file
  pulchritude-error pulchritude-log pulchritude-array
)

install(
  TARGETS pulchritude-file
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
