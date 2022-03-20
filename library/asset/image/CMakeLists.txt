add_library(pulchritude-asset-image SHARED)

find_package(ZLIB REQUIRED)

target_include_directories(
  pulchritude-asset-image
  PUBLIC "../include/"
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/libspng/spng"
    ${ZLIB_INCLUDE_DIRS}
)

target_sources(
  pulchritude-asset-image
  PRIVATE
    src/asset-image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/libspng/spng/spng.c
)

set_target_properties(
  pulchritude-asset-image
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic \
         -Wundef"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  pulchritude-asset-image
    pulchritude-error
    pulchritude-log
    pulchritude-stream
    ${ZLIB_LIBRARIES}
)

install(
  TARGETS pulchritude-asset-image
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
