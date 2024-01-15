std::string cmakeContents = PULE_multilineString(
add_library(%plugin-name SHARED)                                              \n
                                                                              \n
target_include_directories(                                                   \n
  %plugin-name                                                                \n
  PRIVATE                                                                     \n
    "${CMAKE_SOURCE_DIR}/engine-include/"                                     \n
    "${CMAKE_SOURCE_DIR}/library/include/"                                    \n
)                                                                             \n
                                                                              \n
target_link_directories(                                                      \n
  %plugin-name                                                                \n
  PUBLIC "${CMAKE_SOURCE_DIR}/engine-include/../plugins"                      \n
)                                                                             \n
                                                                              \n
target_sources(                                                               \n
  %plugin-name                                                                \n
  PRIVATE                                                                     \n
    %source-files                                                             \n
)                                                                             \n
                                                                              \n
set_target_properties(                                                        \n
  %plugin-name                                                                \n
  PROPERTIES                                                                  \n
    COMPILE_FLAGS                                                             \n
    "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces -Wno-misleading-indentation"\n
  POSITION_INDEPENDENT_CODE ON                                                \n
)                                                                             \n
                                                                              \n
target_link_libraries(                                                        \n
  %plugin-name                                                                \n
    %linked-libraries                                                         \n
)                                                                             \n
                                                                              \n
install(                                                                      \n
  TARGETS %plugin-name                                                        \n
  LIBRARY NAMELINK_SKIP                                                       \n
  LIBRARY                                                                     \n
    DESTINATION bin/plugins/                                                  \n
    COMPONENT plugin                                                          \n
  RUNTIME                                                                     \n
    DESTINATION bin/plugins/                                                  \n
    COMPONENT plugin                                                          \n
)                                                                             \n
);
