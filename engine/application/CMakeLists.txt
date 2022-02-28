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
      "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic -Wundef"
)

target_link_libraries(
  pulchritude-application
  pulchritude-plugin pulchritude-log
)

install(
  TARGETS pulchritude-application
  RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT core
)
