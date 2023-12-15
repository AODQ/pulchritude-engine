add_library(pulchritude-engine-language SHARED)

target_include_directories(pulchritude-engine-language PUBLIC "../include/")
target_sources(
  pulchritude-engine-language
  PRIVATE
    src/engine-language.cpp
)

set_target_properties(
  pulchritude-engine-language
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-engine-language
  pulchritude-string

  LLVM
)

install(
  TARGETS pulchritude-engine-language
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
