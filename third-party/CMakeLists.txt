# build dependencies that can't be easily built by adding source files in
#   target library

# glfw
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set(GLFW_INSTALL ON CACHE INTERNAL "")
add_subdirectory(glfw)

# flecs
set(FLECS_PIC ON CACHE INTERNAL "")
set(FLECS_STATIC_LIBS ON CACHE INTERNAL "")
set(FLECS_SHARED_LIBS OFF CACHE INTERNAL "")
set(FLECS_DEVELOPER_WARNINGS OFF CACHE INTERNAL "")
add_subdirectory(flecs)
