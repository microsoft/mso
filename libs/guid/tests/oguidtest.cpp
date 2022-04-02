#include "motifcpp/motifcpptest.h"
#include <guid/comparison.h>
#include <guid/OGuid.h>
#include <optional>

// {7714B323-69D2-4B37-B6A1-1B433E6EA023}
const GUID nonNullTestGuid = { 0x7714b323, 0x69d2, 0x4b37, { 0xb6, 0xa1, 0x1b, 0x43, 0x3e, 0x6e, 0xa0, 0x23 } };

TEST_CLASS(Creation)
{
	TEST_METHOD(createstring)
	{
		std::wstring str;
		OGuid::Create(str);
		TestAssert::IsTrue(38 == str.length()); // guid is 38 characters
	}

	TEST_METHOD(createguid)
	{
		GUID guid;
		OGuid::Create(guid);

		// I'm only doing this to verify the GUID value above,
		// tho it'll convert anything.
		const auto str = OGuid::ToString(guid);
		TestAssert::IsTrue(38 == str.length()); // guid is 38 characters
	}
};

TEST_CLASS(Conversions)
{
	TEST_METHOD(Success_FullForm)
	{
		GUID guid;
		std::wstring validGuidString(L"{BA7709E9-A602-495A-BA29-7E2C69317721}");
		OGuid::StringToGUID(validGuidString, guid);
		const auto str = OGuid::ToString(guid);
		TestAssert::IsTrue(str == validGuidString);
	}

	TEST_METHOD(Success_UnformattedToGUID)
	{
		GUID guid;
		std::wstring validUnformattedGuidString(L"7714B32369D24B37B6A11B433E6EA023");
		OGuid::UnformattedStringToGUID(validUnformattedGuidString, guid);
		TestAssert::IsTrue(guid == nonNullTestGuid);
	}

	TEST_METHOD(Success_UnformattedForm_To_FullForm)
	{
		GUID guid;
		std::wstring validGuidString(L"{BA7709E9-A602-495A-BA29-7E2C69317721}");
		std::wstring validShortGuidString(L"BA7709E9A602495ABA297E2C69317721");
		OGuid::UnformattedStringToGUID(validShortGuidString, guid);
		const auto str = OGuid::ToString(guid);
		TestAssert::IsTrue(str == validGuidString);
	}

	TEST_METHOD(Failure_FullForm)
	{
		GUID guid;
		std::wstring invalidGuidString(L"{BA79E9-A602-495A-BA29-7E2C69317721}");
		TestAssert::ExpectException<std::exception>([&]() { OGuid::StringToGUID(invalidGuidString, guid); });
	}

	TEST_METHOD(Failure_FullForm_NoExcept)
	{
		std::wstring invalidGuidString(L"{BA79E9-A602-495A-BA29-7E2C69317721}");
		TestAssert::AreEqual(std::nullopt, OGuid::TryStringToGUID(invalidGuidString));
	}

	TEST_METHOD(Failure_UnformattedForm)
	{
		GUID guid;
		std::wstring invalidGuidString(L"BA79E9A602495ABA297E2C69317721}");
		TestAssert::ExpectException<std::exception>([&]() { OGuid::UnformattedStringToGUID(invalidGuidString, guid); });
	}

	TEST_METHOD(AsciiString)
	{
		TestAssert::AreEqual(L"{7714B323-69D2-4B37-B6A1-1B433E6EA023}", OGuid::ToString(nonNullTestGuid).c_str());
		TestAssert::AreEqual(L"7714B323-69D2-4B37-B6A1-1B433E6EA023", OGuid::ToString(nonNullTestGuid, true).c_str());
	}

	TEST_METHOD(string_view)
	{
		std::string_view s { "{7714B323-69D2-4B37-B6A1-1B433E6EA023}" };
		TestAssert::IsTrue(OGuid::StringToGUID(s) == nonNullTestGuid);
	}

	TEST_METHOD(WstringView)
	{
		std::wstring_view s { L"{7714B323-69D2-4B37-B6A1-1B433E6EA023}" };
		TestAssert::IsTrue(OGuid::StringToGUID(s) == nonNullTestGuid);

	}
};

static void IsSmallerOrEqualTestHelper(const GUID& lhs, const GUID& rhs)
{
	bool fIsSmallerEqual = Mso::Guid::LexicalCompare::LessOrEqual( lhs, rhs );
	wchar_t wzlhs[128]; ::StringFromGUID2(lhs, RgC(wzlhs));
	wchar_t wzrhs[128]; ::StringFromGUID2(rhs, RgC(wzrhs));
	TestAssert::AreEqual(fIsSmallerEqual, wcscmp(wzlhs, wzrhs) <= 0);
}

TEST_CLASS(Comparison)
{
	TEST_METHOD(IsSmallerOrEqual)
	{
		GUID guid1 = GUID();
		IsSmallerOrEqualTestHelper(guid1, guid1);

		// Data1
		GUID guid2 = GUID(); guid2.Data1 = 1;
		IsSmallerOrEqualTestHelper(guid2, guid2);
		IsSmallerOrEqualTestHelper(guid1, guid2);
		IsSmallerOrEqualTestHelper(guid2, guid1);

		// Data2
		guid2 = GUID(); guid2.Data2 = 1;
		IsSmallerOrEqualTestHelper(guid2, guid2);
		IsSmallerOrEqualTestHelper(guid1, guid2);
		IsSmallerOrEqualTestHelper(guid2, guid1);

		// Data3
		guid2 = GUID(); guid2.Data3 = 1;
		IsSmallerOrEqualTestHelper(guid2, guid2);
		IsSmallerOrEqualTestHelper(guid1, guid2);
		IsSmallerOrEqualTestHelper(guid2, guid1);

		// Data4
		guid1 = GUID(); guid1.Data4[0] = 1;
		guid2 = GUID(); guid2.Data4[7] = 1;
		IsSmallerOrEqualTestHelper(guid2, guid2);
		IsSmallerOrEqualTestHelper(guid1, guid2);
		IsSmallerOrEqualTestHelper(guid2, guid1);

		guid1 = GUID(); guid1.Data4[6] = 1;
		guid2 = GUID(); guid2.Data4[7] = 1;
		IsSmallerOrEqualTestHelper(guid2, guid2);
		IsSmallerOrEqualTestHelper(guid1, guid2);
		IsSmallerOrEqualTestHelper(guid2, guid1);

		guid1 = GUID(); guid1.Data4[5] = 1;
		guid2 = GUID(); guid2.Data4[6] = 1;
		IsSmallerOrEqualTestHelper(guid2, guid2);
		IsSmallerOrEqualTestHelper(guid1, guid2);
		IsSmallerOrEqualTestHelper(guid2, guid1);

		OGuid::StringToGUID(std::wstring(L"{314D8A77-3621-4C90-A8AF-DD474202F459}"), guid1);
		TestAssert::IsTrue(Mso::Guid::LexicalCompare::LessOrEqual(guid1, guid1));

		OGuid::StringToGUID(std::wstring(L"{D8F856B9-B319-4B84-AE21-2EC2381DFF81}"), guid2);
		IsSmallerOrEqualTestHelper(guid2, guid2);
		IsSmallerOrEqualTestHelper(guid1, guid2);
		IsSmallerOrEqualTestHelper(guid2, guid1);
		TestAssert::IsTrue( Mso::Guid::LexicalCompare::LessOrEqual(guid1, guid2));

		guid1 = guid2;
		OGuid::StringToGUID(std::wstring(L"{F9C8B96E-84DD-4DBC-9C72-277C6B9CCA0E}"), guid2);
		IsSmallerOrEqualTestHelper(guid2, guid2);
		IsSmallerOrEqualTestHelper(guid1, guid2);
		IsSmallerOrEqualTestHelper(guid2, guid1);
		TestAssert::IsTrue(Mso::Guid::LexicalCompare::LessOrEqual(guid1, guid2));

		guid1 = guid2;
		OGuid::StringToGUID(std::wstring(L"{FCC58B10-9BA4-4872-B77A-600747BF6F17}"), guid2);
		IsSmallerOrEqualTestHelper(guid2, guid2);
		IsSmallerOrEqualTestHelper(guid1, guid2);
		IsSmallerOrEqualTestHelper(guid2, guid1);
		TestAssert::IsTrue(Mso::Guid::LexicalCompare::LessOrEqual(guid1, guid2));
	}

	TEST_METHOD(IsEmpty)
	{
		GUID guid1 = {};
		TestAssert::IsTrue(guid1 == GUID_NULL);
		TestAssert::IsTrue(OGuid::FEmptyGuid(guid1));

		GUID guid2 = GUID(); guid2.Data1 = 1;
		TestAssert::IsFalse(guid2 == GUID_NULL);
		TestAssert::IsFalse(OGuid::FEmptyGuid(guid2));
	}
};