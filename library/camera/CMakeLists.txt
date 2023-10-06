add_library(pulchritude-camera SHARED)

target_include_directories(pulchritude-camera PUBLIC "../include/")
target_sources(
  pulchritude-camera
  PRIVATE
    src/camera.cpp
)

set_target_properties(
  pulchritude-camera
    PROPERTIES
      COMPILE_FLAGS "-Wshadow -Wdouble-promotion -Wformat=2 -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  pulchritude-camera
    pulchritude-math
    pulchritude-platform
    pulchritude-gpu
)

install(
  TARGETS pulchritude-camera
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
