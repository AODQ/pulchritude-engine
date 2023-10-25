std::string cmakeContents = PULE_multilineString(
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)                               \n
cmake_policy(VERSION 3.0)                                                     \n
find_program(CCACHE_FOUND ccache)                                             \n
if(CCACHE_FOUND)                                                              \n
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)                    \n
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK    ccache)                    \n
  message("*-- CCACHE found, will use")                                       \n
endif()                                                                       \n
                                                                              \n
project(                                                                      \n
  %project-name                                                               \n
  VERSION 1.0                                                                 \n
  LANGUAGES %supported-languages                                              \n
  DESCRIPTION "CMake husk generated by pulchritude"                           \n
)                                                                             \n
                                                                              \n
set(CMAKE_C_STANDARD 11)                                                      \n
set(CMAKE_CXX_STANDARD 20)                                                    \n
set(CMAKE_CXX_STANDARD_REQUIRED ON)                                           \n
set(CMAKE_CXX_EXTENSIONS OFF)                                                 \n
set(CMAKE_CXX_FLAGS "-march=x86-64")                                          \n
                                                                              \n
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")                       \n
                                                                              \n
%add_subdirs                                                                  \n
);
