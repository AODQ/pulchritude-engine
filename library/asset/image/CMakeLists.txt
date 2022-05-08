# pulchritude asset image depends on SPNG, which we can build simply here
add_library(pulchritude-asset-image-spng STATIC)
find_package(ZLIB REQUIRED)
target_include_directories(
  pulchritude-asset-image-spng
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/libspng/spng"
    ${ZLIB_INCLUDE_DIRS}
)

target_sources(
  pulchritude-asset-image-spng
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/libspng/spng/spng.c
)

set_target_properties(
  pulchritude-asset-image-spng
    PROPERTIES COMPILE_FLAGS ""
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(pulchritude-asset-image-spng  ${ZLIB_LIBRARIES})

# build asset image
add_library(pulchritude-asset-image SHARED)

target_include_directories(
  pulchritude-asset-image
  PUBLIC "../include/"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/libspng/spng"
)

target_sources(
  pulchritude-asset-image
  PRIVATE
    src/asset-image.cpp
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
    pulchritude-asset-image-spng
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
