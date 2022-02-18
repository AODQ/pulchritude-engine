add_library(bulkher-log SHARED)

target_include_directories(bulkher-log PUBLIC "include/")
target_sources(
  bulkher-log
  PRIVATE
    src/bulkher-log/log.c
)

set_target_properties(
  bulkher-log
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic \
         -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

#target_link_libraries(bulkher-log)

install(
  TARGETS bulkher-log
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
