add_library(pulchritude-platform SHARED)

target_include_directories(
  pulchritude-platform
  PUBLIC "../include/"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/glfw/include/"
)
target_sources(
  pulchritude-platform
  PRIVATE
    src/platform.cpp
)

set_target_properties(
  pulchritude-platform
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-platform
    pulchritude-error pulchritude-math

    glfw
)

install(
  TARGETS pulchritude-platform
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
