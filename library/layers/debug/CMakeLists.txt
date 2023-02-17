add_library(pulchritude-layer-debug SHARED)

target_include_directories(
  pulchritude-layer-debug
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/glfw/include/"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../gfx/include/"
)
target_sources(
  pulchritude-layer-debug
  PRIVATE
    src/pulchritude.cpp
    src/debug.cpp

    "../../gfx/src/glad/glad.c"
)

set_target_properties(
  pulchritude-layer-debug
    PROPERTIES
      COMPILE_FLAGS "-Wshadow -Wdouble-promotion -Wformat=2 -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  pulchritude-layer-debug
    pulchritude-log
    pulchritude-log pulchritude-string pulchritude-allocator
    pulchritude-plugin
    glfw
)

install(
  TARGETS pulchritude-layer-debug
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
