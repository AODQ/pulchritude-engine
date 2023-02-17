add_library(pulchritude-string SHARED)

target_include_directories(pulchritude-string PUBLIC "../include/")
target_sources(
  pulchritude-string
  PRIVATE
    src/string.cpp
)

set_target_properties(
  pulchritude-string
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(pulchritude-string pulchritude-allocator)

install(
  TARGETS pulchritude-string
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
