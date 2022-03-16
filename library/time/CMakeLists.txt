add_library(pulchritude-time SHARED)

target_include_directories(pulchritude-time PUBLIC "../include/")
target_sources(
  pulchritude-time
  PRIVATE
    src/time.c
)

set_target_properties(
  pulchritude-time
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic \
         -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(pulchritude-time)

install(
  TARGETS pulchritude-time
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
