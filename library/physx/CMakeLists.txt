add_library(pulchritude-physx SHARED)

option(PULCHRITUDE_PHYSX_2D_SUPPORT "Enable 2D physics support" ON)
option(PULCHRITUDE_PHYSX_3D_SUPPORT "Enable 3D physics support" ON)

target_include_directories(pulchritude-physx PUBLIC "../include/")
target_sources(
  pulchritude-physx
  PRIVATE
    src/collision.cpp
    src/physx2d.cpp
)

set_target_properties(
  pulchritude-physx
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef \
        -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-physx
  pulchritude-log
)

if (PULCHRITUDE_PHYSX_2D_SUPPORT)
  target_link_libraries(pulchritude-physx box2d)
endif()
if (PULCHRITUDE_PHYSX_3D_SUPPORT)
  #target_compile_definitions(pulchritude-physx PUBLIC bullet)
endif()

install(
  TARGETS pulchritude-physx
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
