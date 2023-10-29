# build dependencies that can't be easily built by adding source files in
#   target library

# glfw
#find_package(glfw3)
#if(NOT glfw3_FOUND)
  set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
  set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
  set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
  set(GLFW_INSTALL ON CACHE INTERNAL "")
  add_subdirectory(glfw)
#endif()

# flecs
set(FLECS_PIC ON CACHE INTERNAL "")
set(FLECS_STATIC_LIBS ON CACHE INTERNAL "")
set(FLECS_SHARED_LIBS OFF CACHE INTERNAL "")
set(FLECS_DEVELOPER_WARNINGS OFF CACHE INTERNAL "")
add_subdirectory(flecs)

# schrift (font)
add_library(schrift STATIC)
target_include_directories(schrift PUBLIC "libschrift")
target_sources(
  schrift
  PRIVATE
    libschrift/schrift.c
)
target_link_libraries(schrift)

# volk loader
add_subdirectory(
  "${CMAKE_SOURCE_DIR}/third-party/volk"
)

# box2d
# we don't to build any tests or examples, specifically as they pull
# in third party dependencies like GLFW that conflict with our own
option(BOX2D_BUILD_UNIT_TESTS "Build the Box2D unit tests" OFF)
option(BOX2D_BUILD_TESTBED "Build the Box2D testbed" OFF)
option(BOX2D_BUILD_DOCS "Build the Box2D documentation" OFF)
option(BOX2D_USER_SETTINGS "Override Box2D settings with b2UserSettings.h" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
add_subdirectory(box2d)
set_target_properties(box2d PROPERTIES POSITION_INDEPENDENT_CODE ON)

if (PULCHRITUDE_FEATURE_NETWORK)
  set(ENET_STATIC ON CACHE INTERNAL "")
  add_subdirectory(enet)
endif()
