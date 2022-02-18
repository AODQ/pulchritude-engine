add_executable(bulkher-application)

target_sources(
  bulkher-application
  PRIVATE
    src/source.cpp
)

#target_compile_features(bulkher-application PRIVATE cxx_std_20)

set_target_properties(
  bulkher-application
  PROPERTIES
    COMPILE_FLAGS
      "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wpedantic -Wundef"
)

target_link_libraries(bulkher-application bulkher-plugin)

install(
  TARGETS bulkher-application
  RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT core
)
