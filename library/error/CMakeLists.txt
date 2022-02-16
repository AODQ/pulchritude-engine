add_library(pulzoad-error SHARED)

target_include_directories(pulzoad-error PUBLIC "include/")
target_sources(
  pulzoad-error
  PRIVATE
    src/pulzoad-error/error.c
)

set_target_properties(
  pulzoad-error
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic \
         -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(pulzoad-error pulzoad-log)

install(
  TARGETS pulzoad-error
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
