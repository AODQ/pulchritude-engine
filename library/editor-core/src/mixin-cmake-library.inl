std::string cmakeContents = PULE_multilineString(
add_library(%lib-name SHARED)                                                 \n
                                                                              \n
target_include_directories(                                                   \n
  %lib-name                                                                   \n
  PRIVATE                                                                     \n
    "${%project-path}/engine-include/"                                        \n
  PUBLIC                                                                      \n
    "${%project-path}/library/include/"                                       \n
)                                                                             \n
                                                                              \n
target_link_directories(                                                      \n
  %lib-name                                                                   \n
  PUBLIC "${%project-path}/engine-include/../plugins"                         \n
)                                                                             \n
                                                                              \n
target_sources(                                                               \n
  %lib-name                                                                   \n
  PRIVATE                                                                     \n
    %source-files                                                             \n
)                                                                             \n
                                                                              \n
set_target_properties(                                                        \n
  %lib-name                                                                   \n
  PROPERTIES                                                                  \n
    COMPILE_FLAGS                                                             \n
    "-Wshadow -Wdouble-promotion -Wall -Wformat=2 -Wextra -Wundef -Wno-missing-braces -Wno-misleading-indentation"\n
  POSITION_INDEPENDENT_CODE ON                                                \n
)                                                                             \n
                                                                              \n
target_link_libraries(                                                        \n
  %lib-name                                                                   \n
    %linked-libraries                                                         \n
)                                                                             \n
                                                                              \n
install(                                                                      \n
  TARGETS %lib-name                                                           \n
  LIBRARY NAMELINK_SKIP                                                       \n
  LIBRARY                                                                     \n
    DESTINATION bin/plugins/                                                  \n
    COMPONENT plugin                                                          \n
  RUNTIME                                                                     \n
    DESTINATION bin/plugins/                                                  \n
    COMPONENT plugin                                                          \n
)                                                                             \n
);
