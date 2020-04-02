// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/****************************************************************************
Unit tests for the TCntPtr smart pointer
****************************************************************************/

#include "precomp.h"
#include <atomic>
#include <functional>

#include <compilerAdapters/compilerWarnings.h>
#include <object/refCounted.h>
#include <object/unknownObject.h>
#include <test/testCheck.h>

MSO_STRUCT_GUID(IUnkSimple, "9FEAB33F-E5D0-4A52-9216-6BA8BA9990A4")
struct IUnkSimple : public IUnknown
{
  virtual void DoSomething() = 0;
};

MSO_STRUCT_GUID(IUnkSample, "8A2560F5-E28D-4342-8716-1BBD3A4603B3")
struct IUnkSample : public IUnknown
{
  virtual void DoAnything() = 0;
};

struct ISimple : public Mso::IRefCounted
{
  virtual void DoSomething() = 0;
};

typedef std::function<void(bool inc)> RefCountChangedCallback;
class SimpleClass : public ISimple
{
public:
  SimpleClass(RefCountChangedCallback&& onRefCountChanged)
      : m_refCount(0), m_onRefCountChanged(std::move(onRefCountChanged))
  {
  }

  virtual ~SimpleClass() = default;

  virtual void AddRef() const noexcept override
  {
    OACR_ASSUME_NOTHROW_BEGIN

    m_onRefCountChanged(/*incremented*/ true);

    OACR_ASSUME_NOTHROW_END

    ++m_refCount;
  }

  virtual void Release() const noexcept override
  {
    OACR_ASSUME_NOTHROW_BEGIN

    m_onRefCountChanged(/*incremented*/ false);

    OACR_ASSUME_NOTHROW_END

    if (--m_refCount == 0)
    {
      delete this;
    }
  }

  virtual void DoSomething() noexcept override {}

  void ClassDoSomething() noexcept
  {
    OACR_USE_PTR(this); // simulates access to 'this' for OACR build
  }

private:
  mutable std::atomic<uint32_t> m_refCount;
  RefCountChangedCallback m_onRefCountChanged;
};

class UnkSimpleClass : public IUnkSimple
{
public:
  UnkSimpleClass(RefCountChangedCallback&& onRefCountChanged)
      : m_refCount(0), m_onRefCountChanged(std::move(onRefCountChanged))
  {
  }

  virtual ~UnkSimpleClass() = default;

  _Success_(return == S_OK)
      STDMETHOD(QueryInterface)(const GUID& /*riid*/, _Outptr_ void** /*ppvObject*/) noexcept override
  {
    return E_NOINTERFACE;
  }

  STDMETHOD_(ULONG, AddRef)() noexcept override
  {
    OACR_ASSUME_NOTHROW_BEGIN
    m_onRefCountChanged(/*incremented*/ true);
    OACR_ASSUME_NOTHROW_END
    ++m_refCount;
    return 1;
  }

  STDMETHOD_(ULONG, Release)() noexcept override
  {
    OACR_ASSUME_NOTHROW_BEGIN
    m_onRefCountChanged(/*incremented*/ false);
    OACR_ASSUME_NOTHROW_END
    if (--m_refCount == 0)
    {
      delete this;
    }

    return 1;
  }

  virtual void DoSomething() noexcept override {}

  void ClassDoSomething() noexcept
  {
    OACR_USE_PTR(this); // simulates access to 'this' for OACR build
  }

private:
  mutable std::atomic<uint32_t> m_refCount;
  RefCountChangedCallback m_onRefCountChanged;
};

class AggregatedObject : public Mso::UnknownObject<IUnkSimple, IUnkSample>
{
public:
  virtual void DoSomething() noexcept override {}

  virtual void DoAnything() noexcept override {}
};

template <typename TAction>
static void ValidateRefCount(uint32_t expectedIncRefCountCallCount, TAction action)
{
  uint32_t actualIncRefCountCallCount = 0;
  uint32_t actualDecRefCountCallCount = 0;
  auto callback = [&actualIncRefCountCallCount, &actualDecRefCountCallCount](bool incremented) noexcept {
    if (incremented)
    {
      ++actualIncRefCountCallCount;
    }
    else
    {
      ++actualDecRefCountCallCount;
    }
  };

  action(RefCountChangedCallback(callback));

  TestAssert::AreEqual(actualIncRefCountCallCount, actualDecRefCountCallCount, L"IncCount != DecCount.");
  TestAssert::AreEqual(expectedIncRefCountCallCount, actualIncRefCountCallCount, L"Unexpected IncCount.");
}

TestClassComponent(TCntPtrTest, Mso.TCntPtr) TEST_CLASS (TCntPtrTest){

    TEST_METHOD(TCntPtr_EmptyCtor){Mso::TCntPtr<SimpleClass> spObj;

TestAssert::IsNull(spObj.Get(), L"Expected null");
}

TEST_METHOD(TCntPtr_NullCtor)
{
  Mso::TCntPtr<SimpleClass> spObj = nullptr;

  TestAssert::IsNull(spObj.Get(), L"Expected null");
}

TEST_METHOD(TCntPtr_DeprecatedNullCtor)
{
  // TODO: Remove when we stop using NULL
  Mso::TCntPtr<SimpleClass> spObj;

  TestAssert::IsNull(spObj.Get(), L"Expected null");
}

TEST_METHOD(TCntPtr_Create)
{
  ValidateRefCount(1, [](RefCountChangedCallback&& onRefCountChanged) {
    SimpleClass* ptr = new SimpleClass(std::move(onRefCountChanged));
    Mso::TCntPtr<SimpleClass> spObj(ptr);

    TestAssert::AreEqual((void*)ptr, (void*)spObj.Get(), L"Expected ptr");
  });
}

TEST_METHOD(TCntPtr_CreateInterface)
{
  ValidateRefCount(1, [](RefCountChangedCallback&& onRefCountChanged) {
    SimpleClass* ptr = new SimpleClass(std::move(onRefCountChanged));
    Mso::TCntPtr<ISimple> spIntf(ptr);

    TestAssert::AreEqual((void*)ptr, (void*)spIntf.Get(), L"Expected ptr");
  });
}

TEST_METHOD(TCntPtr_CopyConstructor)
{
  ValidateRefCount(2, [](RefCountChangedCallback&& onRefCountChanged) {
    SimpleClass* ptr = new SimpleClass(std::move(onRefCountChanged));
    Mso::TCntPtr<SimpleClass> spObj(ptr);
    Mso::TCntPtr<SimpleClass> spSameObj(spObj);

    TestAssert::AreEqual((void*)ptr, (void*)spObj.Get(), L"Expected ptr");
    TestAssert::AreEqual((void*)ptr, (void*)spSameObj.Get(), L"Expected ptr");
  });
}

TEST_METHOD(TCntPtr_CopyConstructorInterface)
{
  ValidateRefCount(2, [](RefCountChangedCallback&& onRefCountChanged) {
    SimpleClass* ptr = new SimpleClass(std::move(onRefCountChanged));
    Mso::TCntPtr<SimpleClass> spObj(ptr);
    Mso::TCntPtr<ISimple> spIntf(spObj);

    TestAssert::AreEqual((void*)ptr, (void*)spObj.Get(), L"Expected ptr");
    TestAssert::AreEqual((void*)ptr, (void*)spIntf.Get(), L"Expected ptr");
  });
}

TEST_METHOD(TCntPtr_MoveConstructor)
{
  ValidateRefCount(1, [](RefCountChangedCallback&& onRefCountChanged) {
    SimpleClass* ptr = new SimpleClass(std::move(onRefCountChanged));
    Mso::TCntPtr<SimpleClass> spObj(ptr);
    Mso::TCntPtr<SimpleClass> spSameObj(std::move(spObj));

    TestAssert::IsNull(spObj.Get(), L"Expected null");
    TestAssert::AreEqual((void*)ptr, (void*)spSameObj.Get(), L"Expected ptr");
  });
}

TEST_METHOD(TCntPtr_MoveConstructorInterface)
{
  ValidateRefCount(1, [](RefCountChangedCallback&& onRefCountChanged) {
    SimpleClass* ptr = new SimpleClass(std::move(onRefCountChanged));
    Mso::TCntPtr<SimpleClass> spObj(ptr);
    Mso::TCntPtr<ISimple> spIntf(std::move(spObj));

    TestAssert::IsNull(spObj.Get(), L"Expected null");
    TestAssert::AreEqual((void*)ptr, (void*)spIntf.Get(), L"Expected ptr");
  });
}

// Factory method to get benefits from using the move constructor
static Mso::TCntPtr<SimpleClass> CreateSimpleClass(RefCountChangedCallback&& onRefCountChanged)
{
  Mso::TCntPtr<SimpleClass> spObj = new SimpleClass(std::move(onRefCountChanged));
  spObj->ClassDoSomething();
  return spObj; // std::move() not needed because the same type allows the named return value optimization.
}

TEST_METHOD(TCntPtr_CallCreateSimpleClass)
{
  ValidateRefCount(1, [](RefCountChangedCallback&& onRefCountChanged) {
    Mso::TCntPtr<ISimple> spObj = CreateSimpleClass(std::move(onRefCountChanged));

    TestAssert::IsNotNull(spObj.Get(), L"Expected not a null value");
  });
}

// Factory method to get benefits from using the move constructor
static Mso::TCntPtr<ISimple> CreateISimple(RefCountChangedCallback&& onRefCountChanged)
{
  Mso::TCntPtr<SimpleClass> spObj = new SimpleClass(std::move(onRefCountChanged));
  spObj->ClassDoSomething();
  return std::move(spObj); // We should use std::move() here to avoid use of copy constructor.
                           // Named value return optimization will not work because we have different types.
}

TEST_METHOD(TCntPtr_CallCreateISimple)
{
  ValidateRefCount(1, [](RefCountChangedCallback&& onRefCountChanged) {
    Mso::TCntPtr<ISimple> spIntf = CreateISimple(std::move(onRefCountChanged));

    TestAssert::IsNotNull(spIntf.Get(), L"Expected not a null value");
  });
}

TEST_METHOD(TCntPtr_CopyAssignment)
{
  ValidateRefCount(3, [](RefCountChangedCallback&& onRefCountChanged) {
    Mso::TCntPtr<SimpleClass> spObj1(new SimpleClass(RefCountChangedCallback(onRefCountChanged)));
    SimpleClass* ptr = new SimpleClass(std::move(onRefCountChanged));
    Mso::TCntPtr<SimpleClass> spObj2(ptr);
    spObj1 = spObj2;

    TestAssert::AreEqual((void*)ptr, (void*)spObj1.Get(), L"Expected ptr");
    TestAssert::AreEqual((void*)ptr, (void*)spObj2.Get(), L"Expected ptr");
  });
}

TEST_METHOD(TCntPtr_CopyAssignmentInterface)
{
  ValidateRefCount(3, [](RefCountChangedCallback&& onRefCountChanged) {
    SimpleClass* ptr = new SimpleClass(RefCountChangedCallback(onRefCountChanged));
    Mso::TCntPtr<SimpleClass> spObj(ptr);
    Mso::TCntPtr<ISimple> spIntf = new SimpleClass(std::move(onRefCountChanged));
    spIntf = spObj;

    TestAssert::AreEqual((void*)ptr, (void*)spObj.Get(), L"Expected ptr");
    TestAssert::AreEqual((void*)ptr, (void*)spIntf.Get(), L"Expected ptr");
  });
}

TEST_METHOD(TCntPtr_CopyAssignmentSameObject)
{
  // See what happens when we assign TCntPtr to itself.
  ValidateRefCount(1, [](RefCountChangedCallback&& onRefCountChanged) {
    SimpleClass* ptr = new SimpleClass(std::move(onRefCountChanged));
    Mso::TCntPtr<SimpleClass> spObj(ptr);

    OACR_WARNING_SUPPRESS(IDENTITY_ASSIGNMENT, "We want to test our code that nothing bad happens in this case");
    BEGIN_DISABLE_WARNING_SELF_ASSIGN_OVERLOADED()
    spObj = spObj;
    END_DISABLE_WARNING_SELF_ASSIGN_OVERLOADED()

    TestAssert::AreEqual((void*)ptr, (void*)spObj.Get(), L"Expected ptr");
  });
}

TEST_METHOD(TCntPtr_CopyAssignmentConst)
{
  // Test that TCntPtr can accept a const variable and AddRef/Release methods are not const.
  ValidateRefCount(3, [](RefCountChangedCallback&& onRefCountChanged) {
    Mso::TCntPtr<const UnkSimpleClass> spObj1(new UnkSimpleClass(RefCountChangedCallback(onRefCountChanged)));
    const UnkSimpleClass* ptr = new UnkSimpleClass(std::move(onRefCountChanged));
    Mso::TCntPtr<const UnkSimpleClass> spObj2(ptr);
    spObj1 = spObj2;

    TestAssert::AreEqual((void*)ptr, (void*)spObj1.Get(), L"Expected ptr");
    TestAssert::AreEqual((void*)ptr, (void*)spObj2.Get(), L"Expected ptr");
  });
}

TEST_METHOD(TCntPtr_MoveAssignment)
{
  ValidateRefCount(2, [](RefCountChangedCallback&& onRefCountChanged) {
    Mso::TCntPtr<SimpleClass> spObj1 = new SimpleClass(RefCountChangedCallback(onRefCountChanged));
    SimpleClass* ptr = new SimpleClass(std::move(onRefCountChanged));
    Mso::TCntPtr<SimpleClass> spObj2(ptr);
    spObj1 = std::move(spObj2);

    TestAssert::AreEqual((void*)ptr, (void*)spObj1.Get(), L"Expected ptr");
    TestAssert::IsNull(spObj2.Get(), L"Expected null");
  });
}

TEST_METHOD(TCntPtr_MoveAssignmentInterface)
{
  ValidateRefCount(2, [](RefCountChangedCallback&& onRefCountChanged) {
    SimpleClass* ptr = new SimpleClass(RefCountChangedCallback(onRefCountChanged));
    Mso::TCntPtr<SimpleClass> spObj(ptr);
    Mso::TCntPtr<ISimple> spIntf = new SimpleClass(std::move(onRefCountChanged));
    spIntf = std::move(spObj);

    TestAssert::IsNull(spObj.Get(), L"Expected null");
    TestAssert::AreEqual((void*)ptr, (void*)spIntf.Get(), L"Expected ptr");
  });
}

TEST_METHOD(TCntPtr_MoveAssignmentSameObject)
{
  // Our copy assignment does not check if we use the same object. This test is to see that nothing bad happens.
  ValidateRefCount(1, [](RefCountChangedCallback&& onRefCountChanged) {
    SimpleClass* ptr = new SimpleClass(std::move(onRefCountChanged));
    Mso::TCntPtr<SimpleClass> spObj(ptr);

    BEGIN_DISABLE_WARNING_SELF_MOVE()
    spObj = std::move(spObj);
    END_DISABLE_WARNING_SELF_MOVE()

    TestAssert::AreEqual((void*)ptr, (void*)spObj.Get(), L"Expected ptr");
  });
}

TEST_METHOD(TCntPtr_NullAssignment)
{
  ValidateRefCount(1, [](RefCountChangedCallback&& onRefCountChanged) {
    Mso::TCntPtr<SimpleClass> spObj = new SimpleClass(RefCountChangedCallback(onRefCountChanged));
    spObj = nullptr;

    TestAssert::IsNull(spObj.Get(), L"Expected null");
  });
}

TEST_METHOD(TCntPtr_IsEqualObject_BothISimpleClass_AreEqual)
{
  Mso::TCntPtr<IUnkSimple> spObj = Mso::Make<AggregatedObject>();
  Mso::TCntPtr<IUnkSimple> spObjTwo = spObj;

  TestAssert::IsTrue(Mso::ComUtil::AreEqualObjects(spObj, spObjTwo));
}

TEST_METHOD(TCntPtr_IsEqualObject_DifferentInterfaceTypes_AreEqual)
{
  Mso::TCntPtr<IUnkSimple> spObj = Mso::Make<AggregatedObject>();
  Mso::TCntPtr<IUnkSample> spSample;
  TestAssert::HrSucceeded(Mso::ComUtil::HrQueryFrom(spSample, spObj));

  TestAssert::IsTrue(Mso::ComUtil::AreEqualObjects(spObj, spSample));
}

TEST_METHOD(TCntPtr_IsEqualObject_DifferentObject_AreNotEqual)
{
  Mso::TCntPtr<IUnkSimple> spObj = Mso::Make<AggregatedObject>();
  Mso::TCntPtr<IUnkSimple> spObjTwo = Mso::Make<AggregatedObject>();

  TestAssert::IsFalse(Mso::ComUtil::AreEqualObjects(spObj, spObjTwo));
}
}
;
