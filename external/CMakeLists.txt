# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

if(${MSO_LIBLET_TEST} STREQUAL GTEST)
  add_subdirectory(gtest)
endif()

add_subdirectory(pthread)