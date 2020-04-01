// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"

#include "tagUtils/tagTypes.h"

OACR_WARNING_DISABLE(CAST_CAN_BE_CONST, "The CppUnitTest framework runs afoul of default OACR rules for liblets")
OACR_WARNING_DISABLE(FUNC_COULD_BE_NOEXCEPT, "The CppUnitTest framework runs afoul of default OACR rules for liblets")
OACR_WARNING_DISABLE(FUNC_COULD_BE_NOTHROW, "The CppUnitTest framework runs afoul of default OACR rules for liblets")
OACR_WARNING_DISABLE(STATIC_FUNCTION, "The CppUnitTest framework runs afoul of default OACR rules for liblets")

TEST_CLASS (MsoReserveTagTests)
{
  static void SyntaxTestsForTool()
  {
    MsoReserveTag a(0); // not tagged because the tagger doesn't know about ctor's on objects
    MsoReserveTag b{0}; // same
    auto c = MsoReserveTag(0x0201658c /* tag_cawwm */); // tagged because looks like c-style function call
    auto d = MsoReserveTag{0}; // not tagged because tagger doesn't know anything about uniform initialization

    DoSomethingWithTag(MsoReserveTag(0x02015415 /* tag_cavqv */));
    DoSomethingWithTag(MsoReserveTag{0});
  }

  static void DoSomethingWithTag(MsoReserveTag) noexcept {}

  static void SyntaxTestsForToolWithVar(uint32_t value) noexcept
  {
    MsoReserveTag a(value); // not tagged because the tagger doesn't know about ctor's on objects
    MsoReserveTag b{value}; // same
    auto c = MsoReserveTag(value); // tagged because looks like c-style function call
    auto d = MsoReserveTag{value}; // not tagged because tagger doesn't know anything about uniform initialization

    DoSomethingWithTag(MsoReserveTag(value));
    DoSomethingWithTag(MsoReserveTag{value});
  }

  // Standard AssertTag #define isn't available here because
  // asserts rely on Tag's
  static void AssertTag(bool, uint32_t) noexcept {}

  static void FailedSyntaxTestsForToolWithEmbeddedParens() noexcept
  {
    struct MyTagUnderlyingValueHoldingType
    {
      uint32_t value{0};
    };

    MyTagUnderlyingValueHoldingType t;

    // Intentionally ignoring as these do not parse due with the existing tagger regexes
    // probably because they're more than just a simple variable to detect
    // ASSERTTAG_IGNORE_START
    DoSomethingWithTag(MsoReserveTag(t.value));
    DoSomethingWithTag(MsoReserveTag{t.value});

    DoSomethingWithTag(MsoReserveTag(MyTagUnderlyingValueHoldingType().value));
    DoSomethingWithTag(MsoReserveTag{MyTagUnderlyingValueHoldingType().value});

    // This is an example using AssertTag to demonstrate the replacement issue
    // doesn't exist exist with MsoReserveTag
    AssertTag(true, t.value);
    AssertTag(true, MyTagUnderlyingValueHoldingType().value);
    // ASSERTTAG_IGNORE_FINISH
  }

  TEST_METHOD(ObjHasStrongType)
  {
    auto value = MsoReserveTag(0x0201658d /* tag_cawwn */);
    auto expected = UNTAGGED;

    bool isSameType = std::is_same<decltype(value), decltype(expected)>::value;
    TestAssert::IsFalse(isSameType);

    // Intentionally unreferenced params
    value;
    expected;
  }

  TEST_METHOD(UntaggedReserve)
  {
    // Intentionally ignoring to prove that MsoReserve is pinned to the UNTAGGED value
    // ASSERTTAG_IGNORE_START
    auto value = MsoReserve();
    // ASSERTTAG_IGNORE_FINISH
    auto expected = UNTAGGED;

    TestAssert::IsTrue(value == expected);
  }

  TEST_METHOD(TaggedReserve)
  {
    const uint32_t expected = 0x001c0797;
    auto value = MsoReserveTag(expected);

    TestAssert::IsTrue(value == expected);
  }

  TEST_METHOD(TagEnumAssignment)
  {
    const uint32_t expected = 10;
    // Intentionally ignoring to prove assignment to a specific value
    // ASSERTTAG_IGNORE_START
    enum class MyEnum : uint32_t
    {
      value = MsoReserveTag(10)
    };
    // ASSERTTAG_IGNORE_FINISH
    static_assert(expected == static_cast<uint32_t>(MyEnum::value), "verify constexpr assignment");
    TestAssert::IsTrue(true, L"Compilation is success");
  }

  TEST_METHOD(TagConstAssignment)
  {
    const uint32_t expected = 10;
    // Intentionally ignoring to prove assignment to a specific value
    // ASSERTTAG_IGNORE_START
    static const uint32_t value = MsoReserveTag(10);
    // ASSERTTAG_IGNORE_FINISH
    static_assert(expected == value, "verify constexpr assignment");
    TestAssert::IsTrue(true, L"Compilation is success");
  }
};
