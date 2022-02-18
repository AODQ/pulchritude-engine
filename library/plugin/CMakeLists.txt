add_library(bulkher-plugin SHARED)

target_include_directories(bulkher-plugin PUBLIC "include/")
target_sources(
  bulkher-plugin
  PRIVATE
    src/bulkher-plugin/plugin.cpp
)

set_target_properties(
  bulkher-plugin
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic \
         -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

#target_link_libraries(bulkher-plugin)

install(
  TARGETS bulkher-plugin
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
