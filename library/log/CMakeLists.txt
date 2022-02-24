add_library(pulchritude-log SHARED)

target_include_directories(pulchritude-log PUBLIC "../include/")
target_sources(
  pulchritude-log
  PRIVATE
    src/log.c
)

set_target_properties(
  pulchritude-log
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic \
         -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

#target_link_libraries(pulchritude-log)

install(
  TARGETS pulchritude-log
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
