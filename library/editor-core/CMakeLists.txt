add_library(pulchritude-editor-core SHARED)

target_include_directories(pulchritude-editor-core PUBLIC "../include/")
target_sources(
  pulchritude-editor-core
  PRIVATE
    src/build.cpp
    src/function-registry.cpp
    src/plugin.cpp
    src/run.cpp
    src/undotree.cpp
    src/json-conv.cpp
    src/cJSON.c
)

set_target_properties(
  pulchritude-editor-core
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-editor-core
    pulchritude-asset-pds
    pulchritude-data-serializer
    pulchritude-error
    pulchritude-log
    pulchritude-plugin
    pulchritude-stream
    pulchritude-string
    pulchritude-time
)

install(
  TARGETS pulchritude-editor-core
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
