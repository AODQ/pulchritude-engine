add_executable(puledit)

target_sources(
  puledit
  PRIVATE
    src/source.cpp
)

set_target_properties(
  puledit
  PROPERTIES
    COMPILE_FLAGS
      "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    ENABLE_EXPORTS ON
)

target_link_libraries(
  puledit
  pulchritude-allocator
  pulchritude-array
  pulchritude-asset-pds
  pulchritude-data-serializer
  pulchritude-error
  pulchritude-file
  pulchritude-log
  pulchritude-math
  pulchritude-plugin
  pulchritude-stream
  pulchritude-string
  pulchritude-time
  pulchritude-tui
)

install(
  TARGETS puledit
  RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT core
)
