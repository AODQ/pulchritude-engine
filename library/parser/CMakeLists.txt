add_library(pulchritude-parser SHARED)

target_include_directories(pulchritude-parser PUBLIC "../include/")
target_sources(
  pulchritude-parser
  PRIVATE
    src/parser.cpp
    src/parser-expression-grammar-parser.cpp
)

set_target_properties(
  pulchritude-parser
    PROPERTIES
      COMPILE_FLAGS
        "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces"
    POSITION_INDEPENDENT_CODE ON
    ENABLE_EXPORTS ON
)

target_link_libraries(
  pulchritude-parser
  pulchritude-allocator
  pulchritude-string
  pulchritude-error
)

install(
  TARGETS pulchritude-parser
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
