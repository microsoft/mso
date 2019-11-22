// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"
#include <compileradapters/functiondecorations.h>
#include <compileradapters/functiondecorations_deprecated.h>
#include <compileradapters/cppmacros.h>
#include "tagutils/tagutils.h"

OACR_WARNING_DISABLE(CAST_CAN_BE_CONST, "The CppUnitTest framework runs afoul of default OACR rules for liblets")
OACR_WARNING_DISABLE(FUNC_COULD_BE_NOEXCEPT, "The CppUnitTest framework runs afoul of default OACR rules for liblets")
OACR_WARNING_DISABLE(FUNC_COULD_BE_NOTHROW, "The CppUnitTest framework runs afoul of default OACR rules for liblets")
OACR_WARNING_DISABLE(STATIC_FUNCTION, "The CppUnitTest framework runs afoul of default OACR rules for liblets")

template<typename T>
static void TestSuccessfulConvertToString(uint32_t iTag, const T* szExpected) noexcept
{
	T szActual[MAX_TAG_CCH] = { 0 };
	TaggingUtilities::ConvertTagToString(iTag, szActual);
	TestAssert::AreEqual(szExpected, szActual);
}

TEST_CLASS(CriticalTagToString)
{
	TEST_METHOD(MaxCriticalToString)
	{
		const uint32_t value = 65535;
		const char* string = "65535";
		TestSuccessfulConvertToString(value, string);
	}

	TEST_METHOD(MinCriticalToString)
	{
		const uint32_t value = 1;
		const char* string = "1";
		TestSuccessfulConvertToString(value, string);
	}

	TEST_METHOD(TensPlaceString)
	{
		const uint32_t value = 25;
		const char* string = "25";
		TestSuccessfulConvertToString(value, string);
	}

	TEST_METHOD(HundredsPlaceString)
	{
		const uint32_t value = 512;
		const char* string = "512";
		TestSuccessfulConvertToString(value, string);
	}

	TEST_METHOD(ThousandsPlaceString)
	{
		const uint32_t value = 4192;
		const char* string = "4192";
		TestSuccessfulConvertToString(value, string);
	}

	TEST_METHOD(CriticalTagToStringBufferTermination)
	{
		char szActual[] = "abcdfeghi";
		const char* szExpected = "5192";
		TaggingUtilities::ConvertTagToString(5192, szActual);
		TestAssert::AreEqual(szExpected, szActual);
	}
	
	TEST_METHOD(TheWzVersionWorksToo)
	{
		const uint32_t c_dwCritTag = 13000;
		const wchar_t* c_wzCritTag = L"13000";
		TestSuccessfulConvertToString(c_dwCritTag, c_wzCritTag);
	}
};

static const uint32_t c_dwFourTag = 2038004346;
static const char c_szFourTag[] = "yyzz";
static const wchar_t c_wzFourTag[] = L"yyzz";
TEST_CLASS(FourCharacterToString)
{
	TEST_METHOD(SuccessSz)
	{
		TestSuccessfulConvertToString(c_dwFourTag, c_szFourTag);
	}

	TEST_METHOD(SuccessWz)
	{
		TestSuccessfulConvertToString(c_dwFourTag, c_wzFourTag);
	}
}; // TESTCLASS

static const uint32_t c_dwFiveTag = 0x001c0797;
static const char c_szFiveTag[] = "aha4x";
static const wchar_t c_wzFiveTag[] = L"aha4x";
TEST_CLASS(FiveCharacterToString)
{
	// 0 is a special case tag since we always pad it out to five characters
	TEST_METHOD(TagZeroSz)
	{
		TestSuccessfulConvertToString(0, "00000");
	}

	TEST_METHOD(TagZeroWz)
	{
		TestSuccessfulConvertToString(0, L"00000");
	}

	TEST_METHOD(SuccessSz)
	{
		TestSuccessfulConvertToString(c_dwFiveTag, c_szFiveTag);
	}

	TEST_METHOD(SuccessWz)
	{
		TestSuccessfulConvertToString(c_dwFiveTag, c_wzFiveTag);
	}

// Motif doesn't yet implement u16 and u32 C++ character types/strings to UnitTestString
#if 0
	TEST_METHOD(SuccessU16)
	{
		static const char16_t sz[] = u"aha4x";
		TestSuccessfulConvertToString(c_dwFiveTag, sz);
	}

	TEST_METHOD(SuccessU32)
	{
		static const char32_t sz[] = U"aha4x";
		TestSuccessfulConvertToString(c_dwFiveTag, sz);
	}
#endif

	TEST_METHOD(UnexpectedCharacter)
	{
		const wchar_t* wzInvalidTag = L"ab&cd"; // '&' is not in the allowed namespace a-z0-9
		const wchar_t* wzExpectedConversion = L"ab*cd";
		uint32_t tag = TaggingUtilities::ConvertStringToTag(wzInvalidTag, wcslen(wzInvalidTag));

		TestSuccessfulConvertToString(tag, wzExpectedConversion);
	}
}; // TESTCLASS
