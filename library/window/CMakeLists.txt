add_library(bulkher-window SHARED)

target_include_directories(bulkher-window PUBLIC "include/")
target_sources(
  bulkher-window
  PRIVATE
    src/bulkher-window/window.cpp
)

set_target_properties(
  bulkher-window
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic \
         -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(bulkher-window glfw)

install(
  TARGETS bulkher-window
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
