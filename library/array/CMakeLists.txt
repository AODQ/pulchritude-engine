add_library(pulchritude-array SHARED)

target_include_directories(pulchritude-array PUBLIC "../include/")
target_sources(
  pulchritude-array
  PRIVATE
    src/array.cpp
)

set_target_properties(
  pulchritude-array
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic \
         -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(pulchritude-array pulchritude-allocator)

install(
  TARGETS pulchritude-array
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
