add_executable(pulchritude-application)

target_sources(
  pulchritude-application
  PRIVATE
    src/source.cpp
)

#target_compile_features(pulchritude-application PRIVATE cxx_std_20)

set_target_properties(
  pulchritude-application
  PROPERTIES
    COMPILE_FLAGS
      "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-application
  pulchritude-plugin pulchritude-log
  pulchritude-string
  pulchritude-allocator
  pulchritude-error
  pulchritude-camera
  pulchritude-asset-script-task-graph
  pulchritude-script
  pulchritude-asset-pds
  pulchritude-asset-render-graph
  pulchritude-imgui-engine
  pulchritude-task-graph
)

install(
  TARGETS pulchritude-application
  RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT core
)
