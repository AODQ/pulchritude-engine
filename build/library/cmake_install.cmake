# Install script for directory: C:/msys64/home/ic2000/pulchritude-engine/library

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/ic2000/pulchritude-engine/install/")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/msys64/mingw64/bin/llvm-objdump.exe")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/msys64/home/ic2000/pulchritude-engine/build/library/allocator/cmake_install.cmake")
  include("C:/msys64/home/ic2000/pulchritude-engine/build/library/array/cmake_install.cmake")
  include("C:/msys64/home/ic2000/pulchritude-engine/build/library/asset/cmake_install.cmake")
  include("C:/msys64/home/ic2000/pulchritude-engine/build/library/data-serializer/cmake_install.cmake")
  include("C:/msys64/home/ic2000/pulchritude-engine/build/library/ecs/cmake_install.cmake")
  include("C:/msys64/home/ic2000/pulchritude-engine/build/library/error/cmake_install.cmake")
  include("C:/msys64/home/ic2000/pulchritude-engine/build/library/file/cmake_install.cmake")
  include("C:/msys64/home/ic2000/pulchritude-engine/build/library/gfx/cmake_install.cmake")
  include("C:/msys64/home/ic2000/pulchritude-engine/build/library/imgui/cmake_install.cmake")
  include("C:/msys64/home/ic2000/pulchritude-engine/build/library/log/cmake_install.cmake")
  include("C:/msys64/home/ic2000/pulchritude-engine/build/library/math/cmake_install.cmake")
  include("C:/msys64/home/ic2000/pulchritude-engine/build/library/platform/cmake_install.cmake")
  include("C:/msys64/home/ic2000/pulchritude-engine/build/library/plugin/cmake_install.cmake")
  include("C:/msys64/home/ic2000/pulchritude-engine/build/library/stream/cmake_install.cmake")
  include("C:/msys64/home/ic2000/pulchritude-engine/build/library/string/cmake_install.cmake")
  include("C:/msys64/home/ic2000/pulchritude-engine/build/library/time/cmake_install.cmake")

endif()

