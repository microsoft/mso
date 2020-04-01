// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/****************************************************************************
Unit tests for the TCntPtrRef class
****************************************************************************/

#include "precomp.h"
#include <atomic>
#include <functional>
#include <object/refCounted.h>
#include <object/unknownObject.h>
#include <test/testCheck.h>
#include <comUtil/qiCast.h>

MSO_STRUCT_GUID(ISimpleUnknown, "C70B7853-1EDB-4AB8-B2D2-F9951A99A847")
struct DECLSPEC_NOVTABLE ISimpleUnknown : public IUnknown
{
  virtual void DoSomething() = 0;
};

struct ISimple : public Mso::IRefCounted
{
  virtual void DoSomething() = 0;
};

class SimpleTestRef : public Mso::TRefCountedImpl<ISimple>
{
  void DoSomething() override {}
};

inline static std::wstring ToString(ISimple* /*q*/) { return L""; }
inline static std::wstring ToString(ISimple** /*q*/) { return L""; }
inline static std::wstring ToString(const ISimple* /*q*/) { return L""; }
inline static std::wstring ToString(ISimple* const* /*q*/) { return L""; }
inline static std::wstring ToString(const Mso::TCntPtr<ISimple>* /*q*/) { return L""; }

TestClassComponent(TCntPtrRefTest, Mso.TCntPtrRef)
TEST_CLASS(TCntPtrRefTest)
{

TEST_METHOD(TCntPtr_TCntPtrRef_GetRaw)
{
	Mso::TCntPtr<ISimple> spObj;
	TestAssert::AreEqual(spObj.GetRaw(), (&spObj).GetRaw());
}

TEST_METHOD(TCntPtr_TCntPtrRef_StarOperator)
{
	Mso::TCntPtr<ISimple> spObj;
	TestAssert::AreEqual(spObj.Get(), *(&spObj));
}

TEST_METHOD(TCntPtr_TCntPtrRef_GetAddressOf)
{
  return L"";
}
inline static std::wstring ToString(const ISimple* q)
{
  return L"";
}
inline static std::wstring ToString(ISimple* const* q)
{
  return L"";
}
inline static std::wstring ToString(const Mso::TCntPtr<ISimple>* q)
{
  return L"";
}

TestClassComponent(TCntPtrRefTest, Mso.TCntPtrRef) TEST_CLASS (TCntPtrRefTest)
{
  TEST_METHOD(TCntPtr_TCntPtrRef_GetRaw)
  {
    Mso::TCntPtr<ISimple> spObj;
    TestAssert::AreEqual(spObj.GetRaw(), (&spObj).GetRaw());
  }

  TEST_METHOD(TCntPtr_TCntPtrRef_StarOperator)
  {
    Mso::TCntPtr<ISimple> spObj;
    TestAssert::AreEqual(spObj.Get(), *(&spObj));
  }

  TEST_METHOD(TCntPtr_TCntPtrRef_GetAddressOf)
  {
    Mso::TCntPtr<ISimple> spObj;
    TestAssert::AreEqual(spObj.GetAddressOf(), const_cast<ISimple**>((&spObj).GetAddressOf()));
  }

  TEST_METHOD(TCntPtr_TCntPtrRef_ClearAndGetAddressOf)
  {
    Mso::TCntPtr<ISimple> spObj = new SimpleTestRef();
    TestAssert::AreEqual(spObj.GetRaw(), (&spObj).ClearAndGetAddressOf());
    TestAssert::IsNull(spObj.Get());
  }

  TEST_METHOD(TCntPtr_TCntPtrRef_IID_PPV_ARGS_Helper)
  {
    Mso::TCntPtr<IUnknown> spObj;
    void** ppObj = IID_PPV_ARGS_Helper(&spObj);
    void** ppDesired = &spObj;
    TestAssert::AreEqual((void*)ppDesired, (void*)ppObj);
  }

  TEST_METHOD(TCntPtr_TCntPtrRef_EqualOperator)
  {
    Mso::TCntPtr<IUnknown> spObj1;
    Mso::TCntPtr<IUnknown> spObj2;
    TestAssert::IsTrue(&spObj1 == &spObj1);
    TestAssert::IsTrue(&spObj2 == &spObj2);
    TestAssert::IsFalse(&spObj1 == &spObj2);
    TestAssert::IsFalse(&spObj2 == &spObj1);
  }

  TEST_METHOD(TCntPtr_TCntPtrRef_NotEqualOperator)
  {
    Mso::TCntPtr<IUnknown> spObj1;
    Mso::TCntPtr<IUnknown> spObj2;
    TestAssert::IsTrue(&spObj1 != &spObj2);
    TestAssert::IsTrue(&spObj2 != &spObj1);
    TestAssert::IsFalse(&spObj2 != &spObj2);
    TestAssert::IsFalse(&spObj1 != &spObj1);
  }

  static void TestFunc1(_In_ _Notnull_ ISimple * pObj, _Out_ ISimple * *ppObj)
  {
    *ppObj = pObj;
  }

  static void TestFunc2(_In_ _Notnull_ ISimple * pObj, _Out_ Mso::TCntPtr<ISimple> * spObj)
  {
    *spObj = Mso::TCntPtr<ISimple>(pObj, false);
  }

  static void TestFunc3(_In_ _Notnull_ ISimple * pObj, _Out_ void** ppT)
  {
    *ppT = pObj;
  }

  static void TestFunc4(_In_ _Notnull_ const ISimple* pObj, _Out_ const void** ppT)
  {
    *ppT = pObj;
  }

  template <typename T, typename U>
  static void TestTCntPtrRef(U && func)
  {
    T spObjOutput;
    Mso::TCntPtr<ISimple> spObjInput = new SimpleTestRef();
    ISimple* pObjInput = spObjInput.Detach();

    func(pObjInput, &spObjOutput);

    TestAssert::AreEqual(pObjInput, spObjOutput.Get());
  }

  TEST_METHOD(TCntPtr_TCntPtrRef_NonConst_ppT)
  {
    TestTCntPtrRef<Mso::TCntPtr<ISimple>>(TestFunc1);
  }

  TEST_METHOD(TCntPtr_TCntPtrRef_NonConst_pTCntPtr)
  {
    TestTCntPtrRef<Mso::TCntPtr<ISimple>>(TestFunc2);
  }

  TEST_METHOD(TCntPtr_TCntPtrRef_NonConst_ppVoid)
  {
    TestTCntPtrRef<Mso::TCntPtr<ISimple>>(TestFunc3);
  }

  TEST_METHOD(TCntPtr_TCntPtrRef_NonConst_ppConstVoid)
  {
    TestTCntPtrRef<Mso::TCntPtr<ISimple>>(TestFunc4);
  }

  TEST_METHOD(TCntPtr_TCntPtrRef_Const_TCntPtr)
  {
    const Mso::TCntPtr<ISimple> spObj;
    TestAssert::AreEqual(spObj.GetThis(), &spObj);
  }

  template <typename T, typename U>
  static void TestTCntPtrRef_Reinterpret_Cast()
  {
    using TNonConst = typename std::remove_const<T>::type;
    T spObj;
    U ppObjDesired = reinterpret_cast<U>(const_cast<TNonConst&>(spObj).GetAddressOf());
    U ppObj = static_cast<U>(&spObj);
    TestAssert::AreEqual((void*)ppObjDesired, (void*)ppObj);
  }

  TEST_METHOD(TCntPtr_TCntPtrRef_NonConst_Reinterpret_Cast_ppVoid)
  {
    TestTCntPtrRef_Reinterpret_Cast<Mso::TCntPtr<ISimple>, void**>();
  }

  class SimpleTestRefUnknown : public Mso::UnknownObject<ISimpleUnknown>
  {
    void DoSomething() override {}
  };

  TEST_METHOD(TCntPtr_TCntPtrRef_NonConst_Reinterpret_Cast_ppIUnknown)
  {
    TestTCntPtrRef_Reinterpret_Cast<Mso::TCntPtr<SimpleTestRefUnknown>, IUnknown**>();
  }

  template <typename T>
  static void TestTCntPtrRef_Cast_pVoid()
  {
    T spObj;
    void* pVoid = &spObj;
    Mso::TCntPtr<ISimple>* pTCntPtr = &spObj;
    TestAssert::AreEqual(pVoid, (void*)pTCntPtr);
  }

  TEST_METHOD(TCntPtr_TCntPtrRef_NonConst_Cast_pVoid)
  {
    TestTCntPtrRef_Cast_pVoid<Mso::TCntPtr<ISimple>>();
  }
};