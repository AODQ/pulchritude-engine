add_library(pulchritude-error SHARED)

target_include_directories(pulchritude-error PUBLIC "../include/")
target_sources(
  pulchritude-error
  PRIVATE
    src/error.cpp
)

set_target_properties(
  pulchritude-error
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(pulchritude-error pulchritude-log pulchritude-string)

install(
  TARGETS pulchritude-error
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
