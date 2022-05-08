add_subdirectory(library)

add_executable(pulchritude-editor)

target_sources(
  pulchritude-editor
  PRIVATE
    src/source.cpp
)

#target_compile_features(pulchritude-application PRIVATE cxx_std_20)

set_target_properties(
  pulchritude-editor
  PROPERTIES
    COMPILE_FLAGS
      "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic -Wundef"
)

target_link_libraries(
  pulchritude-editor
    pulchritude-asset-pds
    pulchritude-data-serializer
    pulchritude-error
    pulchritude-gfx
    pulchritude-log
    pulchritude-platform
    pulchritude-plugin
    pulchritude-stream
    pulchritude-string
    pulchritude-time
)

install(
  TARGETS pulchritude-editor
  RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT core
)
