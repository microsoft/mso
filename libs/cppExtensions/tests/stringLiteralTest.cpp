// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
	Basic testing for StringLiteral classes
*/
#include <cppextensions/stringliteral.h>
#include <oacr.h>
#include <motifcpp/motifcpptest.h>
#include <array>
#include <string>
using namespace Mso::StringLiterals;

template<typename char_type>
static void CheckLength(size_t expected, const StringLiteral<char_type>& s)
{
	TestAssert::AreEqual(expected, std::char_traits<char_type>::length(s));
}

TEST_CLASS(StringLiteralTests)
{
	static void ConstLengths() noexcept
	{
		static_assert(""_S.length() == 0, "Invalid compile-time length");
		static_assert("Test"_S.length() == 4, "Invalid compile-time length");
	}

	TEST_METHOD(TestEmpty)
	{
		CheckLength(0, ""_S);
		CheckLength(0, L""_S);
		CheckLength(0, u""_S);
		CheckLength(0, U""_S);
	}

	TEST_METHOD(TestNonEmpty)
	{
		CheckLength(3, "Yes"_S);
		CheckLength(3, L"Yes"_S);
		CheckLength(3, u"Yes"_S);
		CheckLength(3, U"Yes"_S);
	}
};

TEST_CLASS(StringLiteralArrayTests)
{
	struct HoldString
	{
		template<size_t Count>
		constexpr HoldString(const std::array<StringLiteral<char>, Count>& strings) noexcept : 
			m_strings(&strings[0]), 
			m_count(Count)
		{
			static_assert(Count != 0, "Must pass at least 1 string");
			
		}
		
		const char* const* m_strings;
		const size_t m_count;
	};
	
	TEST_METHOD(TestArray)
	{
		static const std::array<StringLiteral<char>, 3> strings = 
		{
			"Hi"_S, "This"_S, "Works"_S,
		};
		
		const HoldString hold(strings);
		
		//static_assert(hold.m_count == 3, "Incorrect count");  Look at this later
		TestAssert::AreEqual("Hi", hold.m_strings[0]);
		TestAssert::AreEqual("This", hold.m_strings[1]);
		TestAssert::AreEqual("Works", hold.m_strings[2]);
	}
};