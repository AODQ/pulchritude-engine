add_library(pulchritude-math SHARED)

target_include_directories(pulchritude-math PUBLIC "../include/")
target_sources(
  pulchritude-math
  PRIVATE
    src/math.cpp
)

set_target_properties(
  pulchritude-math
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic \
         -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(pulchritude-math)

install(
  TARGETS pulchritude-math
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
