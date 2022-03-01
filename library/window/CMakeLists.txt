add_library(pulchritude-window SHARED)

target_include_directories(pulchritude-window PUBLIC "../include/")
target_sources(
  pulchritude-window
  PRIVATE
    src/window.cpp
)

set_target_properties(
  pulchritude-window
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic \
         -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(pulchritude-window glfw pulchritude-error)

install(
  TARGETS pulchritude-window
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
