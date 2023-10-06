add_library(pulchritude-layer-debug SHARED)

find_package(Vulkan REQUIRED FATAL_ERROR)

target_include_directories(
  pulchritude-layer-debug
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/glfw/include/"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../gpu/include/"
    ${VULKAN_INCLUDE_DIRS}
)
target_sources(
  pulchritude-layer-debug
  PRIVATE
    src/pulchritude.cpp
    src/debug.cpp
)

set_target_properties(
  pulchritude-layer-debug
    PROPERTIES
      COMPILE_FLAGS "-Wshadow -Wdouble-promotion -Wformat=2 -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-layer-debug
    pulchritude-log
    pulchritude-log pulchritude-string pulchritude-allocator
    pulchritude-plugin
    ${VULKAN_LIB_LIST}
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
