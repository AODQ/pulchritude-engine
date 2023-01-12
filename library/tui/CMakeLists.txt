add_library(pulchritude-tui SHARED)

find_package(Curses REQUIRED)

target_include_directories(pulchritude-tui PUBLIC "../include/" ${CURSES_INCLUDE_DIR})
target_sources(
  pulchritude-tui
  PRIVATE
    src/tui.cpp
)

set_target_properties(
  pulchritude-tui
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic \
         -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(pulchritude-tui ncurses ${CURSES_LIBRARIES})

install(
  TARGETS pulchritude-tui
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
