# -- library
add_library(pulchritude-gpu-ir SHARED)

target_include_directories(pulchritude-gpu-ir PRIVATE "../include/")
target_sources(
  pulchritude-gpu-ir
  PRIVATE
    src/gpu-ir.cpp
)

set_target_properties(
  pulchritude-gpu-ir
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-gpu-ir
    pulchritude-error
    pulchritude-log
    pulchritude-gpu
    pulchritude-array
    pulchritude-math
    SPIRV-Tools-opt
)

install(
  TARGETS pulchritude-gpu-ir
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
