cmake_minimum_required(VERSION 3.24 FATAL_ERROR) # 3.24 for vk components
cmake_policy(VERSION 3.24)

#### DEVELOPMENT FEATURES ######################################################
# unless turned off here, features should only be here (and enabled) under
#   puldev or below

option(PULCHRITUDE_FEATURE_COMPILER "pulchritude-compiler" ON)
if (PULCHRITUDE_FEATURE_COMPILER)
  add_compile_definitions(PULCHRITUDE_FEATURE_COMPILER)
endif()

option(PULCHRITUDE_FEATURE_TEXT "pulchritude-text" ON)
if (PULCHRITUDE_FEATURE_TEXT)
  add_compile_definitions(PULCHRITUDE_FEATURE_TEXT)
endif()

option(PULCHRITUDE_FEATURE_NET "pulchritude-net" ON)
if (PULCHRITUDE_FEATURE_NET)
  add_compile_definitions(PULCHRITUDE_FEATURE_NET)
endif()

################################################################################

if (PULCHRITUDE_PLATFORM STREQUAL "Win64")
  message("*-- compiling for Windows 64-bit platform")
  include(CMakeModules/win64.cmake)
elseif (PULCHRITUDE_PLATFORM STREQUAL "Linux")
  message("*-- compiling for Linux 64-bit platform")
elseif (PULCHRITUDE_PLATFORM STREQUAL "osx")
  message("*-- compiling for macOS 64-bit platform")
else()
  # error out if platform not found
  message(FATAL_ERROR "platform not found")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# use rpath so we can install .so files locally to lib (such as libstdc++.so.6
#   etc) to allow pulchritude to be portable across various distros
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
if(PULCHRITUDE_PLATFORM STREQUAL "Linux")
  set(CMAKE_INSTALL_RPATH "$ORIGIN/plugins/:$ORIGIN/")
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,--disable-new-dtags,-z,origin")
elseif(PULCHRITUDE_PLATFORM STREQUAL "osx")
  set(CMAKE_INSTALL_RPATH "@loader_path/plugins/;@loader_path")
  set(CMAKE_THREAD_LIBS_INIT "-lpthread")
  set(CMAKE_HAVE_THREADS_LIBRARY 1)
  set(CMAKE_USE_WIN32_THREADS_INIT 0)
  set(CMAKE_USE_PTHREADS_INIT 1)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
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
  pulchritude-engine
  VERSION 0.0
  LANGUAGES C CXX
  DESCRIPTION "pulchritude engine | github.com/aodq/pulchritude-engine"
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (
     PULCHRITUDE_PLATFORM STREQUAL "Linux"
)
  set(CMAKE_CXX_FLAGS "-march=x86-64 -rdynamic")
elseif (PULCHRITUDE_PLATFORM STREQUAL "osx")
elseif (PULCHRITUDE_PLATFORM STREQUAL "Win64" OR PULCHRITUDE_PLATFORM STREQUAL "Win32")
  set(CMAKE_CXX_FLAGS "-march=x86-64 -gstabs")
else()
  message(FATAL_ERROR "platform not found")
endif()

# requires out of source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if (EXISTS "${LOC_PATH}")
  message(FATAL_ERROR "build *must* be out of source")
endif()

# adds dependencies in correct order
add_subdirectory(third-party)
add_subdirectory(application)
add_subdirectory(editor)
add_subdirectory(library)
add_subdirectory(puledit)

# install engine headers
install(
  DIRECTORY library/include/
  DESTINATION ${CMAKE_INSTALL_BINDIR}/engine-include
)

function(watchfile)
  set_property(
    DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${ARGV}
  )
endfunction()

execute_process(
  COMMAND
    python3
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/compile-engine-shaders.py"
    "--dir"
    "${CMAKE_CURRENT_SOURCE_DIR}"
  RESULT_VARIABLE processFinished
)
if (NOT processFinished STREQUAL "0")
  message(FATAL_ERROR "could not compile shaders")
endif()

watchfile("${CMAKE_CURRENT_SOURCE_DIR}/shaders/text-bitmap.frag)
watchfile("${CMAKE_CURRENT_SOURCE_DIR}/shaders/text-bitmap.vert)
watchfile("${CMAKE_CURRENT_SOURCE_DIR}/shaders/debug-gfx-line.frag)
watchfile("${CMAKE_CURRENT_SOURCE_DIR}/shaders/debug-gfx-line.vert)
