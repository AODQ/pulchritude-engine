add_executable(pulzoad-application)

target_sources(
  pulzoad-application
  PRIVATE
    src/source.cpp
)

#target_compile_features(pulzoad-application PRIVATE cxx_std_20)

set_target_properties(
  pulzoad-application
  PROPERTIES
    COMPILE_FLAGS
      "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic -Wundef"
)

target_link_libraries(pulzoad-application pulzoad-plugin)

install(
  TARGETS pulzoad-application
  RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT core
)
