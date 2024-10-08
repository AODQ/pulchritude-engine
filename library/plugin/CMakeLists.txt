add_library(pulchritude-plugin SHARED)

target_include_directories(pulchritude-plugin PUBLIC "../include/")
target_sources(
  pulchritude-plugin
  PRIVATE
    src/plugin.cpp
    src/plugin-payload.cpp
)

set_target_properties(
  pulchritude-plugin
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-plugin
    pulchritude-log
    pulchritude-string
)

install(
  TARGETS pulchritude-plugin
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
