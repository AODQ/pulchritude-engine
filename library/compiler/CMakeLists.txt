add_library(pulchritude-compiler SHARED)

target_include_directories(pulchritude-compiler PUBLIC "../include/")
target_sources(
  pulchritude-compiler
  PRIVATE
    src/compiler.cpp
)

set_target_properties(
  pulchritude-compiler
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-compiler
  pulchritude-allocator
  pulchritude-string

  LLVM
)

install(
  TARGETS pulchritude-compiler
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
