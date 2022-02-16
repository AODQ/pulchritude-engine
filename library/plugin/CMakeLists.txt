add_library(pulzoad-plugin SHARED)

target_include_directories(pulzoad-plugin PUBLIC "include/")
target_sources(
  pulzoad-plugin
  PRIVATE
    src/pulzoad-plugin/plugin.cpp
)

set_target_properties(
  pulzoad-plugin
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic \
         -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

#target_link_libraries(pulzoad-plugin)

install(
  TARGETS pulzoad-plugin
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION lib/
    COMPONENT plugin
  RUNTIME
    DESTINATION lib/
    COMPONENT plugin
)
