# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

# set minimum cmake version
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# project name and language
project(Mso 
  HOMEPAGE_URL "https://github.com/microsoft/Mso"
  LANGUAGES CXX)

# include liblet.cmake with liblet-specific functions
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake")
include(liblet)

if(MSO_ENABLE_UNIT_TESTS)
  enable_testing()
endif()

add_subdirectory(external)
add_subdirectory(libs)

liblet_detect_dependency_cycles()