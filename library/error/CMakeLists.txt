add_library(bulkher-error SHARED)

target_include_directories(bulkher-error PUBLIC "include/")
target_sources(
  bulkher-error
  PRIVATE
    src/bulkher-error/error.c
)

set_target_properties(
  bulkher-error
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic \
         -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(bulkher-error bulkher-log)

install(
  TARGETS bulkher-error
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
