add_library(pulchritude-net SHARED)

target_include_directories(
  pulchritude-net
    PUBLIC "../include/"
)
target_sources(
  pulchritude-net
  PRIVATE
    src/net.cpp
)

set_target_properties(
  pulchritude-net
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef \
        -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-net
  pulchritude-log
  pulchritude-error
  pulchritude-string
  GameNetworkingSockets::static
)

install(
  TARGETS pulchritude-net
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
