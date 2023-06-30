# -- third party
# TODO... dont do this
find_package(Vulkan REQUIRED FATAL_ERROR)
# vulkan memory allocator
#set(VMA_STATIC_VULKAN_FUNCTIONS ON CACHE INTERNAL "")
set(VMA_STATIC_VULKAN_FUNCTIONS OFF CACHE INTERNAL "")

#set(VMA_DEBUG_INITIALIZE_ALLOCATIONS ON CACHE INTERNAL "")
set(VMA_DEBUG_DONT_EXCEED_MAX_MEMORY_ALLOCATION_COUNT ON CACHE INTERNAL "")
add_subdirectory(
  "${CMAKE_SOURCE_DIR}/third-party/VulkanMemoryAllocator"
  third-party/VulkanMemoryAllocator
)

# -- library
add_library(pulchritude-gfx SHARED)


target_include_directories(
  pulchritude-gfx
  PUBLIC "../include/"
  PRIVATE
    "include/"
    ${VULKAN_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/glfw/include/"
    "${CMAKE_SOURCE_DIR}/third-party/VulkanMemoryAllocator/include"
)
target_sources(
  pulchritude-gfx
  PRIVATE
    src/barrier.cpp
    src/commands.cpp
    src/gfx.cpp
    src/image.cpp
    src/pipeline.cpp
    src/shader-module.cpp
    src/util.cpp
    src/vulkan-memory-allocator.cpp
)

set_target_properties(
  pulchritude-gfx
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-gfx
    pulchritude-error
    pulchritude-log
    volk
    glfw # this is needed for glfwCreateWindowSurface
    VulkanMemoryAllocator
)

install(
  TARGETS pulchritude-gfx
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
