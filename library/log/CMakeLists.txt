add_library(pulzoad-log SHARED)

target_include_directories(pulzoad-log PUBLIC "include/")
target_sources(
  pulzoad-log
  PRIVATE
    src/pulzoad-log/log.c
)

set_target_properties(
  pulzoad-log
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic \
         -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

#target_link_libraries(pulzoad-log)

install(
  TARGETS pulzoad-log
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
