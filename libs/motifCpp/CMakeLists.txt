# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

liblet(motifCpp
  DEPENDS
    Mso::crash
    Mso::oacr
    Mso::platformAdapters
)

if(${MSO_LIBLET_TEST} STREQUAL GTEST)
  target_link_libraries(motifCpp PUBLIC gtest_main)
  target_compile_definitions(motifCpp PUBLIC MSO_USE_GTEST)
elseif(${MSO_LIBLET_TEST} STREQUAL XCTEST)
  arget_compile_definitions(motifCpp PUBLIC MSO_USE_XCTEST)
else()
  message(FATAL_ERROR "Unknown MSO_LIBLET_TEST: ${MSO_LIBLET_TEST}")
endif()