std::string cmakeContents = PULE_multilineString(
add_library(%name SHARED)                                                     \n
                                                                              \n
target_include_directories(%name PUBLIC "../include/")                        \n
target_sources(                                                               \n
  %name                                                                       \n
  PRIVATE                                                                     \n
    src/source.cpp                                                            \n
)                                                                             \n
                                                                              \n
set_target_properties(                                                        \n
  %name                                                                       \n
    PROPERTIES                                                                \n
      COMPILE_FLAGS                                                           \n
      "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef"          \n
    POSITION_INDEPENDENT_CODE ON                                              \n
)                                                                             \n
                                                                              \n
target_link_libraries(                                                        \n
  pulchritude-editor-core                                                     \n
  pulchritude-allocator                                                       \n
  pulchritude-array                                                           \n
  pulchritude-asset-font                                                      \n
  pulchritude-asset-image                                                     \n
  pulchritude-asset-model                                                     \n
  pulchritude-asset-pds                                                       \n
  pulchritude-asset-tiled                                                     \n
  pulchritude-core                                                            \n
  pulchritude-data-serializer                                                 \n
  pulchritude-ecs-serializer                                                  \n
  pulchritude-ecs                                                             \n
  pulchritude-error                                                           \n
  pulchritude-file                                                            \n
  pulchritude-gfx-debug                                                       \n
  pulchritude-gfx                                                             \n
  pulchritude-imgui-engine                                                    \n
  pulchritude-imgui                                                           \n
  pulchritude-log                                                             \n
  pulchritude-math                                                            \n
  pulchritude-platform                                                        \n
  pulchritude-plugin                                                          \n
  pulchritude-renderer-3d                                                     \n
  pulchritude-script                                                          \n
  pulchritude-stream                                                          \n
  pulchritude-string                                                          \n
  pulchritude-time                                                            \n
  pulchritude-tui                                                             \n
)                                                                             \n
                                                                              \n
install(                                                                      \n
  TARGETS %name                                                               \n
  LIBRARY NAMELINK_SKIP                                                       \n
  LIBRARY                                                                     \n
    DESTINATION bin/plugins/                                                  \n
    COMPONENT plugin                                                          \n
  RUNTIME                                                                     \n
    DESTINATION bin/plugins/                                                  \n
    COMPONENT plugin                                                          \n
)                                                                             \n
);
