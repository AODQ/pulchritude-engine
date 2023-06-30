add_library(pulchritude-raycast SHARED)

target_include_directories(pulchritude-raycast PUBLIC "../include/")
target_sources(
  pulchritude-raycast
  PRIVATE
    src/raycast.cpp
)

set_target_properties(
  pulchritude-raycast
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-raycast
    pulchritude-array
    pulchritude-math
    pulchritude-shape
)

install(
  TARGETS pulchritude-raycast
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
