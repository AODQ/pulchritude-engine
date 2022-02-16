cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
cmake_policy(VERSION 3.0)

if (PULZOAD_PLATFORM STREQUAL "Win64")
  message("*-- compiling for Windows 64-bit platform")
  include(CMakeModules/win64.cmake)
elseif (PULZOAD_PLATFORM STREQUAL "Linux")
  message("*-- compiling for Linux 64-bit platform")
elseif (PULZOAD_PLATFORM STREQUAL "macOS")
  message("*-- compiling for macOS 64-bit platform")
else()
  message(WARNING "defaulting to compile for Linux 64-bit platform")
  set(PULZOAD_PLATFORM "Linux")
endif()

# use rpath so we can install .so files locally to lib (such as libstdc++.so.6
#   etc) to allow pulzoad to be portable across various distros
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
if(PULZOAD_PLATFORM STREQUAL "Linux")
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib/")
elseif(PULZOAD_PLATFORM STREQUAL "macOS")
  set(CMAKE_INSTALL_RPATH "@loader_path/../lib/")
endif()

# silence OpenGL legacy warning
cmake_policy(SET CMP0072 NEW)

# add ccache if found
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK    ccache)
  message("*-- CCACHE found, will use")
endif()

## add include what you use if found
find_program(iwyu_path "/usr/local/bin/" NAMES include-what-you-use iwyu)
if(iwyu_path)
  message("*-- include-what-you-use found, will use")
  set_property(GLOBAL PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
endif()

## TODO probably not the best way to handle debug mode
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  message("[debug enabled]")
else()
  add_compile_definitions(NDEBUG)
endif()

project(
  pulzoad
  VERSION 0.0
  LANGUAGES C CXX
  DESCRIPTION "pulzoad | puzzled engine | github.com/aodq/pulzoad-engine"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (PULZOAD_PLATFORM STREQUAL "Linux" OR PULZOAD_PLATFORM STREQUAL "macOS")
  set(CMAKE_CXX_FLAGS "-march=x86-64")
elseif (PULZOAD_PLATFORM STREQUAL "Win64" OR PULZOAD_PLATFORM STREQUAL "Win32")
  set(CMAKE_CXX_FLAGS "-march=x86-64 -gstabs")
endif()

# requires out of source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if (EXISTS "${LOC_PATH}")
  message(FATAL_ERROR "build *must* be out of source")
endif()

# adds dependencies in correct order
add_subdirectory(engine)
add_subdirectory(library)
