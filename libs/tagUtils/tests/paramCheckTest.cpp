// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"

#include "tagUtils/tagUtils.h"

OACR_WARNING_DISABLE(CAST_CAN_BE_CONST, "The CppUnitTest framework runs afoul of default OACR rules for liblets")
OACR_WARNING_DISABLE(FUNC_COULD_BE_NOTHROW, "The CppUnitTest framework runs afoul of default OACR rules for liblets")
OACR_WARNING_DISABLE(STATIC_FUNCTION, "The CppUnitTest framework runs afoul of default OACR rules for liblets")

TEST_CLASS (ParamChecking)
{
  TEST_METHOD(NullBuffSz)
  {
    OACR_WARNING_SUPPRESS(
        INVALID_PARAM_VALUE_1, "Intentionally passing invalid parameter to test unexpected caller functionality.");
    TestAssert::IsFalse(TaggingUtilities::ConvertiTagToSzTag(1, nullptr, 10));
  }

  TEST_METHOD(NullBuffWz)
  {
    OACR_WARNING_SUPPRESS(
        INVALID_PARAM_VALUE_1, "Intentionally passing invalid parameter to test unexpected caller functionality.");
    TestAssert::IsFalse(TaggingUtilities::ConvertiTagToWzTag(1, nullptr, 10));
  }

  TEST_METHOD(BuffTooSmallSz)
  {
    char szActual[MAX_TAG_CCH - 1] = {0};
    TestAssert::IsFalse(TaggingUtilities::ConvertiTagToSzTag(1, szActual, _countof(szActual)));
  }

  TEST_METHOD(BuffTooSmallWz)
  {
    wchar_t wzActual[MAX_TAG_CCH - 1] = {0};
    TestAssert::IsFalse(TaggingUtilities::ConvertiTagToWzTag(1, wzActual, _countof(wzActual)));
  }
};
