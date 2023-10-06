add_library(pulchritude-script SHARED)

option(PULCHRITUDE_SCRIPT_BIND_ALLOCATOR "bind allocator" ON)
option(PULCHRITUDE_SCRIPT_BIND_ARRAY "bind array" ON)
option(PULCHRITUDE_SCRIPT_BIND_ASSET_FONT "bind asset font" ON)
option(PULCHRITUDE_SCRIPT_BIND_ASSET_IMAGE "bind asset image" ON)
option(PULCHRITUDE_SCRIPT_BIND_ASSET_PDS "bind asset pds" ON)
option(PULCHRITUDE_SCRIPT_BIND_ASSET_TILED "bind asset tiled" ON)
option(PULCHRITUDE_SCRIPT_BIND_DATA_SERIALIZER "bind DATA-serializer" ON)
option(PULCHRITUDE_SCRIPT_BIND_ECS_SERIALIZER "bind ECS-serializer" ON)
option(PULCHRITUDE_SCRIPT_BIND_ECS "bind ecs" ON)
option(PULCHRITUDE_SCRIPT_BIND_FILE "bind file" ON)
option(PULCHRITUDE_SCRIPT_BIND_GFX_DEBUG "bind GFX-debug" ON)
option(PULCHRITUDE_SCRIPT_BIND_GPU "bind gpu" ON)
option(PULCHRITUDE_SCRIPT_BIND_IMGUI_ENGINE "bind IMGUI-engine" ON)
option(PULCHRITUDE_SCRIPT_BIND_IMGUI "bind imgui" ON)
option(PULCHRITUDE_SCRIPT_BIND_LOG "bind log" ON)
option(PULCHRITUDE_SCRIPT_BIND_MATH "bind math" ON)
option(PULCHRITUDE_SCRIPT_BIND_PLATFORM "bind platform" ON)
option(PULCHRITUDE_SCRIPT_BIND_PLUGIN "bind plugin" ON)
option(PULCHRITUDE_SCRIPT_BIND_STREAM "bind stream" ON)
option(PULCHRITUDE_SCRIPT_BIND_SCRIPT "bind script" ON)
option(PULCHRITUDE_SCRIPT_BIND_TIME "bind time" ON)

find_package(Lua 5.3 REQUIRED)
target_include_directories(
  pulchritude-script
  PUBLIC
    "../include/"
    "include"
    ${LUA_INCLUDE_DIR}
)

target_sources(
  pulchritude-script
  PRIVATE
    src/script.cpp
    src/setup.c
)

set_target_properties(
  pulchritude-script
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-script
  ${LUA_LIBRARIES}
)

if (PULCHRITUDE_SCRIPT_BIND_ALLOCATOR)
  target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_ALLOCATOR")
  target_link_libraries(pulchritude-script pulchritude-allocator)
endif()
if (PULCHRITUDE_SCRIPT_BIND_ASSET_FONT)
  target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_ASSET_FONT")
  target_link_libraries(pulchritude-script pulchritude-asset-font)
endif()
if (PULCHRITUDE_SCRIPT_BIND_ASSET_IMAGE)
  target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_ASSET_IMAGE")
  target_link_libraries(pulchritude-script pulchritude-asset-image)
endif()
if (PULCHRITUDE_SCRIPT_BIND_ASSET_PDS)
  target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_ASSET_PDS")
  target_link_libraries(pulchritude-script pulchritude-asset-pds)
endif()
if (PULCHRITUDE_SCRIPT_BIND_ASSET_TILED)
  target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_ASSET_TILED")
  target_link_libraries(pulchritude-script pulchritude-asset-tiled)
endif()
if (PULCHRITUDE_SCRIPT_BIND_DATA_SERIALIZER)
  target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_DATA_SERIALIZER")
  target_link_libraries(pulchritude-script pulchritude-data-serializer)
endif()
if (PULCHRITUDE_SCRIPT_BIND_ECS_SERIALIZER)
  target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_ECS_SERIALIZER")
  target_link_libraries(pulchritude-script pulchritude-ecs-serializer)
endif()
if (PULCHRITUDE_SCRIPT_BIND_ECS)
  target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_ECS")
  target_link_libraries(pulchritude-script pulchritude-ecs)
endif()
if (PULCHRITUDE_SCRIPT_BIND_FILE)
  target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_FILE")
  target_link_libraries(pulchritude-script pulchritude-file)
endif()
if (PULCHRITUDE_SCRIPT_BIND_GFX_DEBUG)
  target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_GFX_DEBUG")
  target_link_libraries(pulchritude-script pulchritude-gfx-debug)
endif()
if (PULCHRITUDE_SCRIPT_BIND_gpu)
  target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_GPU")
  target_link_libraries(pulchritude-script pulchritude-gpu)
endif()
if (PULCHRITUDE_SCRIPT_BIND_IMGUI)
  target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_IMGUI")
  target_link_libraries(pulchritude-script pulchritude-imgui)
endif()
if (PULCHRITUDE_SCRIPT_BIND_MATH)
  target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_MATH")
  target_link_libraries(pulchritude-script pulchritude-math)
endif()
if (PULCHRITUDE_SCRIPT_BIND_PLATFORM)
  target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_PLATFORM")
  target_link_libraries(pulchritude-script pulchritude-platform)
endif()
if (PULCHRITUDE_SCRIPT_BIND_PLUGIN)
  target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_PLUGIN")
  target_link_libraries(pulchritude-script pulchritude-plugin)
endif()
if (PULCHRITUDE_SCRIPT_BIND_SCRIPT)
  target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_SCRIPT")
endif()
if (PULCHRITUDE_SCRIPT_BIND_STREAM)
  target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_STREAM")
  target_link_libraries(pulchritude-script pulchritude-stream)
endif()
if (PULCHRITUDE_SCRIPT_BIND_TIME)
  target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_TIME")
  target_link_libraries(pulchritude-script pulchritude-time)
endif()

# log, array, string and error are included no matter what
target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_LOG")
target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_ERROR")
target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_ARRAY")
target_compile_definitions(pulchritude-script PUBLIC "PULCHRITUDE_SCRIPT_BIND_STRING")
target_link_libraries(
  pulchritude-script
  pulchritude-log pulchritude-error pulchritude-array pulchritude-string
)

install(
  TARGETS pulchritude-script
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
