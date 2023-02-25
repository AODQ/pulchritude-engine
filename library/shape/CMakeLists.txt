add_library(pulchritude-shape SHARED)

target_include_directories(pulchritude-shape PUBLIC "../include/")
target_sources(
  pulchritude-shape
  PRIVATE
    src/shape.cpp
)

set_target_properties(
  pulchritude-shape
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(pulchritude-shape pulchritude-math)

install(
  TARGETS pulchritude-shpae
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
