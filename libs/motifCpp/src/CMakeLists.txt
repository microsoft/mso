# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

if(${MSO_LIBLET_TEST} STREQUAL GTEST)
  liblet_sources(
    SOURCES
      gTestMain.cpp
  )
elseif(${MSO_LIBLET_TEST} STREQUAL XCTEST)
#  liblet_sources(
#    SOURCES
#      xcTestMain.mm
#  )
else()
  message(FATAL_ERROR "Unknown MSO_LIBLET_TEST: ${MSO_LIBLET_TEST}")
endif()