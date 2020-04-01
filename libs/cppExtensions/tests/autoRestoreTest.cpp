// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
Basic testing for Mso::TPropertyRestorer<T, PfnGetter, PfnSetter>
*/
#include <compileradapters/functiondecorations.h>
#include <cppextensions/autoRestore.h>
#include <motifcpp/motifcpptest.h>

template <typename T>
struct TestProperty
{
  static T Get() noexcept
  {
    static typename std::decay<T>::type s_value;
    return std::forward<T>(s_value); // Return any value that compiles.
  }

  static void Set(T /*value*/) noexcept {}

  using Restorer = typename Mso::TPropertyRestorer<T, Get, Set>;

  template <bool expectingMoveAssignmentToCompile = true>
  static void VerifyRestorerCompiles() noexcept
  {
    Restorer rst(Get()); // Ensure class template instantiation compiles.
    Restorer rst2(std::move(rst)); // Ensure move semantics compile too.

    rst = std::move(rst2);
  }

  template <>
  static void VerifyRestorerCompiles<false /*expectingMoveAssignmentToCompile*/>() noexcept
  {
    Restorer rst(Get()); // Ensure class template instantiation compiles.
    Restorer rst2(std::move(rst)); // Ensure move semantics compile too.

    static_assert(
        !std::is_move_assignable<T>::value,
        "If T has a valid move assignment operator, then why can't we move assign it?");
  }
};

static const int c_nDefaultValue = 123;
static const int c_nNonDefaultValue = 456;
static_assert(c_nDefaultValue != c_nNonDefaultValue, "If you break this, then these tests are pretty useless");

static int s_nGlobal = c_nDefaultValue;

void SetTestGlobal(int value) noexcept
{
  s_nGlobal = value;
}

int GetTestGlobal() noexcept
{
  return s_nGlobal;
}

// Hungarian: tr
using TestRestorer = Mso::TPropertyRestorer<int, GetTestGlobal, SetTestGlobal>;

TEST_CLASS (TPropertyRestorerTest)
{
  TEST_METHOD_INITIALIZE(Setup)
  {
    SetTestGlobal(c_nDefaultValue); // Ensure every test starts with default global value
  }

  TEST_METHOD(DefaultConstructor_GlobalIsReverted)
  {
    // Arrange & Act
    {
      TestRestorer tr;
      TestAssert::AreEqual(c_nDefaultValue, GetTestGlobal(), L"Global should not be updated");

      SetTestGlobal(c_nNonDefaultValue);
      TestAssert::AreEqual(c_nNonDefaultValue, GetTestGlobal(), L"Global should be updated");
    }

    // Assert
    TestAssert::AreEqual(c_nDefaultValue, GetTestGlobal(), L"Global should be reverted");
  }

  TEST_METHOD(ConstructorDestructor_Nested_GlobalIsUpdated)
  {
    // Arrange & Act
    {
      TestRestorer tr(c_nNonDefaultValue);
      TestAssert::AreEqual(c_nNonDefaultValue, GetTestGlobal(), L"Global should be updated");

      {
        TestRestorer tr2(123456789);
        TestAssert::AreEqual(123456789, GetTestGlobal(), L"Global should be updated");
      }

      TestAssert::AreEqual(c_nNonDefaultValue, GetTestGlobal(), L"Global should be reverted");
    }

    // Assert
    TestAssert::AreEqual(c_nDefaultValue, GetTestGlobal(), L"Global should be reverted");
  }

  TEST_METHOD(Disable_OriginallyEnabled_NotReverted)
  {
    // Arrange & Act
    {
      TestRestorer tr(c_nNonDefaultValue);

      TestAssert::AreEqual(c_nNonDefaultValue, GetTestGlobal(), L"Global should be updated");

      TestAssert::IsTrue(tr.IsEnabled());
      tr.Disable();
      TestAssert::IsFalse(tr.IsEnabled());
    }

    // Assert
    TestAssert::AreEqual(c_nNonDefaultValue, GetTestGlobal(), L"Global should not be reverted");
  }

  TEST_METHOD(MoveOperator_OriginallyEnabled_OriginalIsDisabled)
  {
    // Arrange & Act
    {
      TestRestorer tr(c_nNonDefaultValue);
      TestAssert::AreEqual(c_nNonDefaultValue, GetTestGlobal(), L"Global should be updated");

      {
        TestRestorer tr2(123456789);
        TestAssert::AreEqual(123456789, GetTestGlobal(), L"Global should be updated");

        TestAssert::IsTrue(tr2.IsEnabled());
        tr = std::move(tr2);
        TestAssert::IsFalse(tr2.IsEnabled());
      }

      TestAssert::AreEqual(123456789, GetTestGlobal(), L"Global should not be reverted");
    }

    // Assert
    TestAssert::AreEqual(c_nNonDefaultValue, GetTestGlobal(), L"Global should be reverted to tr2's previous value");
  }

  TEST_METHOD(MoveConstructor_OriginallyEnabled_OriginalIsDisabled)
  {
    // Arrange & Act
    {
      TestRestorer tr(std::move(TrSetGlobalToNonDefault()));
      TestAssert::AreEqual(c_nNonDefaultValue, GetTestGlobal(), L"Global should be updated");
    }

    // Assert
    TestAssert::AreEqual(c_nDefaultValue, GetTestGlobal(), L"Global should be reverted");
  }

  TEST_METHOD(MoveConstructor_SetRestoreTo123456789_RestoreTo123456789)
  {
    // Arrange & Act
    {
      TestRestorer tr(std::move(TrSetGlobalToNonDefaultAndRestoreTo123456789()));
      TestAssert::AreEqual(c_nNonDefaultValue, GetTestGlobal(), L"Global should be updated");
    }

    // Assert
    TestAssert::AreEqual(123456789, GetTestGlobal(), L"Global should be reverted");
  }

  TEST_METHOD(SetValueToRestore_SetRestoreTo123456789_RestoreTo123456789)
  {
    // Arrange & Act
    {
      TestRestorer tr(c_nNonDefaultValue);
      TestAssert::AreEqual(c_nNonDefaultValue, GetTestGlobal(), L"Global should be updated");

      TestAssert::AreEqual(c_nDefaultValue, tr.ValueToRestore());
      tr.ValueToRestore() = 123456789;
      TestAssert::AreEqual(123456789, tr.ValueToRestore());

      TestAssert::AreEqual(c_nNonDefaultValue, GetTestGlobal(), L"Global should not be reverted yet");
    }

    // Assert
    TestAssert::AreEqual(123456789, GetTestGlobal(), L"Global should be reverted");
  }

  TEST_METHOD(ComplexTemplatedTypes_Compiles)
  {
    struct Unmovable final
    {
      Unmovable() = default;

    private:
      Unmovable(Unmovable&&) = delete;
      Unmovable& operator=(Unmovable&&) = delete;
      DECLARE_COPYCONSTR_AND_ASSIGNMENT(Unmovable); // No copying!
    };

    // Ensure PropertyRestorer doesn't cause any extra constructor calls.
    TestProperty<Unmovable&>::VerifyRestorerCompiles<false /*expectingMoveAssignmentToCompile*/>();
    TestProperty<Unmovable&&>::VerifyRestorerCompiles<false /*expectingMoveAssignmentToCompile*/>();
    TestProperty<const Unmovable&>::VerifyRestorerCompiles<false /*expectingMoveAssignmentToCompile*/>();

    TestProperty<int>::VerifyRestorerCompiles();
    TestProperty<int*>::VerifyRestorerCompiles();
    TestProperty<int&>::VerifyRestorerCompiles();
    TestProperty<const int>::VerifyRestorerCompiles<false /*expectingMoveAssignmentToCompile*/>();
    TestProperty<const int*>::VerifyRestorerCompiles();
    TestProperty<const int&>::VerifyRestorerCompiles<false /*expectingMoveAssignmentToCompile*/>();

    TestProperty<std::unique_ptr<int>>::VerifyRestorerCompiles();
    TestProperty<std::unique_ptr<int>&>::VerifyRestorerCompiles();
    TestProperty<std::unique_ptr<int>&&>::VerifyRestorerCompiles();
    TestProperty<const std::unique_ptr<int>&>::VerifyRestorerCompiles<false /*expectingMoveAssignmentToCompile*/>();
  }

private:
  // any helper type methods

  static TestRestorer TrSetGlobalToNonDefault()
  {
    TestAssert::AreNotEqual(
        c_nNonDefaultValue, GetTestGlobal(), L"This test is less useful if the global already is c_nNonDefaultValue");

    auto verifyPostCondition = Mso::TCleanup::Make(
        []() { TestAssert::AreEqual(c_nNonDefaultValue, GetTestGlobal(), L"Global should not be reverted"); });

    TestRestorer tr(c_nNonDefaultValue);
    TestAssert::AreEqual(c_nNonDefaultValue, GetTestGlobal(), L"Global should be updated");

    return tr;
  }

  static TestRestorer TrSetGlobalToNonDefaultAndRestoreTo123456789()
  {
    TestAssert::AreNotEqual(
        c_nNonDefaultValue, GetTestGlobal(), L"This test is less useful if the global already is c_nNonDefaultValue");
    TestAssert::AreNotEqual(123456789, GetTestGlobal(), L"This test is less useful if the global already is 123456789");

    SetTestGlobal(123456789);

    auto verifyPostCondition = Mso::TCleanup::Make(
        []() { TestAssert::AreEqual(c_nNonDefaultValue, GetTestGlobal(), L"Global should not be reverted"); });

    return TrSetGlobalToNonDefault();
  }
};
