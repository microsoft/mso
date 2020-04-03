# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

liblet_includes(
  INCLUDES
    motifCpp/assert_IgnorePlat_emptyImpl.h
    motifCpp/assert_motifApi.h
    motifCpp/libletAwareMemLeakDetection.h
    motifCpp/motifCppTest.h
    motifCpp/motifCppTestBase.h
    motifCpp/testCheck.h
    motifCpp/testInfo.h
    test/skipSEHUT.h
  INCLUDES_APPLE
    motifCpp/assert_IgnorePlat_apple.h
)

if(${MSO_LIBLET_TEST} STREQUAL GTEST)
  liblet_includes(
    INCLUDES
      motifCpp/gTestAdapter.h
  )
elseif(${MSO_LIBLET_TEST} STREQUAL XCTEST)
#  liblet_includes(
#    INCLUDES
#      motifcpp/xcTestAdapter.h
#  )
else()
  message(FATAL_ERROR "Unsupported MSO_LIBLET_TEST: ${MSO_LIBLET_TEST}")
endif()