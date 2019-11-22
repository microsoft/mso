// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"

#include "tagutils/tagutils.h"

OACR_WARNING_DISABLE(CAST_CAN_BE_CONST, "The CppUnitTest framework runs afoul of default OACR rules for liblets")
OACR_WARNING_DISABLE(FUNC_COULD_BE_NOTHROW, "The CppUnitTest framework runs afoul of default OACR rules for liblets")
OACR_WARNING_DISABLE(STATIC_FUNCTION, "The CppUnitTest framework runs afoul of default OACR rules for liblets")

TEST_CLASS(TagHolderTesting)
{
	TEST_METHOD(CharType)
	{
		TaggingUtilities::SzTag tag(0);
		TestAssert::AreEqual("00000", tag.GetPCWSTR());
	}

	TEST_METHOD(CharType_ImplicitCast)
	{
		TaggingUtilities::SzTag tag(0);
		TestAssert::AreEqual("00000", tag);
	}

	TEST_METHOD(CharType_ZeroIsEmpty)
	{
		TaggingUtilities::SzTag tag(0, false);
		TestAssert::AreEqual("", tag);
	}

	TEST_METHOD(CharType_UntaggedIsEmpty)
	{
		TaggingUtilities::SzTag tag(UNTAGGED, false /* formatZero */);
		TestAssert::AreEqual("", tag);
	}

	TEST_METHOD(CharType_NoValueIsEmpty)
	{
		const uint32_t c_dwFiveTag = 0x001c0797;
		OACR_WARNING_SUPPRESS(LOCAL_ARRAY_SHOULD_BE_PTR, "test code needs array for template size deduction");
		const char c_szFiveTag[] = "aha4x";
		
		TaggingUtilities::SzTag tag(c_dwFiveTag, false  /* formatZero */);
		TestAssert::AreEqual(c_szFiveTag, tag);
	}


	TEST_METHOD(WCharType)
	{
		TaggingUtilities::WzTag tag(0);
		TestAssert::AreEqual(L"00000", tag.GetPCWSTR());
	}

	TEST_METHOD(WCharType_ImplicitCast)
	{
		TaggingUtilities::WzTag tag(0);
		TestAssert::AreEqual(L"00000", tag);
	}

	TEST_METHOD(WCharType_ZeroIsEmpty)
	{
		TaggingUtilities::WzTag tag(0, false  /* formatZero */);
		TestAssert::AreEqual(L"", tag);
	}
};
