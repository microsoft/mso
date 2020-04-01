// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"

#include "tagutils/tagutils.h"

OACR_WARNING_DISABLE(CAST_CAN_BE_CONST, "The CppUnitTest framework runs afoul of default OACR rules for liblets")
OACR_WARNING_DISABLE(FUNC_COULD_BE_NOTHROW, "The CppUnitTest framework runs afoul of default OACR rules for liblets")
OACR_WARNING_DISABLE(STATIC_FUNCTION, "The CppUnitTest framework runs afoul of default OACR rules for liblets")

// ConvertStringToTag has a "bug" in it where it doesn't handle critical tags.  This is just to pin behavior
TEST_CLASS (CriticalCharacterToTag)
{
  TEST_METHOD(ToTagThreeChar)
  {
    const wchar_t* wzCritTag = L"123";
    const uint32_t expected = 0;
    uint32_t tag = TaggingUtilities::ConvertStringToTag(wzCritTag, wcslen(wzCritTag));
    TestAssert::AreEqual(expected, tag); // should expect 123
  }

  TEST_METHOD(ToTagFourChar)
  {
    const wchar_t* wzCritTag = L"1234";
    const uint32_t expected = 825373492;
    uint32_t tag = TaggingUtilities::ConvertStringToTag(wzCritTag, wcslen(wzCritTag));
    TestAssert::AreEqual(expected, tag); // should expect 1234
  }

  TEST_METHOD(ToTagFiveChar)
  {
    const wchar_t* wzCritTag = L"13000";
    const uint32_t expected = 460695194;
    uint32_t tag = TaggingUtilities::ConvertStringToTag(wzCritTag, wcslen(wzCritTag));
    TestAssert::AreEqual(expected, tag); // should expect 13000
  }
};

const wchar_t c_wzFourTag[] = L"yyzz";
const char c_szFourTag[] = "yyzz";
const uint32_t c_dwFourTag = 2038004346;
TEST_CLASS (FourCharacterToTag)
{
  TEST_METHOD(ToTagWz)
  {
    uint32_t tag = TaggingUtilities::ConvertStringToTag(c_wzFourTag, wcslen(c_wzFourTag));
    TestAssert::AreEqual(tag, c_dwFourTag);
  }

  TEST_METHOD(ToTagSz)
  {
    uint32_t tag = TaggingUtilities::ConvertStringToTag(c_szFourTag, strlen(c_szFourTag));
    TestAssert::AreEqual(tag, c_dwFourTag);
  }
};

const uint32_t c_dwFiveTag = 0x001c0797;
const wchar_t c_wzFiveTag[] = L"aha4x";
const char c_szFiveTag[] = "aha4x";
TEST_CLASS (FiveCharacterToTag)
{
  TEST_METHOD(ToTagU16)
  {
    OACR_WARNING_SUPPRESS(LOCAL_ARRAY_SHOULD_BE_PTR, "test code needs array for template size deduction");
    const char16_t sz[] = u"aha4x";
    uint32_t tag = TaggingUtilities::ConvertStringToTag(sz);
    TestAssert::AreEqual(c_dwFiveTag, tag);
  }

  TEST_METHOD(ToTagU32)
  {
    OACR_WARNING_SUPPRESS(LOCAL_ARRAY_SHOULD_BE_PTR, "test code needs array for template size deduction");
    const char32_t sz[] = U"aha4x";
    uint32_t tag = TaggingUtilities::ConvertStringToTag(sz);
    TestAssert::AreEqual(c_dwFiveTag, tag);
  }

  TEST_METHOD(ToTagWz)
  {
    uint32_t tag = TaggingUtilities::ConvertStringToTag(c_wzFiveTag);
    TestAssert::AreEqual(c_dwFiveTag, tag);
  }

  TEST_METHOD(ToTagSz)
  {
    uint32_t tag = TaggingUtilities::ConvertStringToTag(c_szFiveTag);
    TestAssert::AreEqual(c_dwFiveTag, tag);
  }

  TEST_METHOD(UnexpectedCharacterWz)
  {
    OACR_WARNING_SUPPRESS(LOCAL_ARRAY_SHOULD_BE_PTR, "test code needs array for template size deduction");
    const wchar_t wzFiveTag[] = L"ab&cd";
    const uint32_t expected = 417923;
    uint32_t tag = TaggingUtilities::ConvertStringToTag(wzFiveTag);
    TestAssert::AreEqual(expected, tag);
  }

  TEST_METHOD(UnexpectedCharacterSz)
  {
    OACR_WARNING_SUPPRESS(LOCAL_ARRAY_SHOULD_BE_PTR, "test code needs array for template size deduction");
    const char szTag[] = "ab&cd";
    const uint32_t expected = 417923;
    uint32_t tag = TaggingUtilities::ConvertStringToTag(szTag);
    TestAssert::AreEqual(expected, tag);
  }
};
