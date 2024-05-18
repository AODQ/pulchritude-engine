add_library(pulchritude-vhd SHARED)

target_include_directories(pulchritude-vhd PUBLIC "../include/")
target_sources(
  pulchritude-vhd
  PRIVATE
    src/vhd.cpp
)

set_target_properties(
  pulchritude-vhd
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-vhd
  pulchritude-log
  pulchritude-error
  pulchritude-string
  pulchritude-array
)

install(
  TARGETS pulchritude-vhd
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
