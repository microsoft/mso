// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/****************************************************************************
  Holder.cpp

  Unit tests for the smart pointers

****************************************************************************/

#include <motifCpp/testCheck.h>
#include <compilerAdapters/functionDecorations.h>
#include <smartPtr/smartPointerBase.h>
#include <smartPtr/smartPointers.h>
#include <smartPtr/cntPtr.h>
#include <comUtil/qiCast.h>
#include <cppExtensions/autoRestore.h>

/*------------------------------------------------------------------------------
  %%Id: e308ef40-4b24-4cd2-98cb-a269c6345d81
------------------------------------------------------------------------------*/
class CUnknown : public IUnknown
{
public:
  /*---------------------------------------------------------------------------
    %%Id: 8cf178db-8f93-40d9-beb1-c2cbe04feb60
  ---------------------------------------------------------------------------*/
  CUnknown() noexcept : m_cRef(0) {}
  /*---------------------------------------------------------------------------
    %%Id: b3d41d41-7d5f-4190-a215-abbdf1149345
  ---------------------------------------------------------------------------*/
  virtual ~CUnknown() noexcept {}
  /*---------------------------------------------------------------------------
    %%Id: eae6ae85-b193-4bdc-9e77-032456318605
  ---------------------------------------------------------------------------*/
  STDMETHOD(QueryInterface)(REFIID riid, void** ppv) noexcept override
  {
    if (riid == __uuidof(IUnknown))
    {
      *ppv = this;
      AddRef();
      return S_OK;
    }
    return E_NOTIMPL;
  }
  /*---------------------------------------------------------------------------
    %%Id: 997c38a7-d5fe-456f-aaff-f3061bd07693
  ---------------------------------------------------------------------------*/
  STDMETHOD_(ULONG, AddRef)() noexcept override
  {
    return ++m_cRef;
  }
  /*---------------------------------------------------------------------------
    %%Id: 997c38a7-d5fe-456f-aaff-f3061bd07693
  ---------------------------------------------------------------------------*/
  OACR_WARNING_SUPPRESS(
      NOTHROW_FUNC_THROWS,
      "This is test code and we want to use throwing TestAssert::IsTrue. It does not affect ship code.")
  STDMETHOD_(ULONG, Release)() noexcept override
  {
    ULONG cRef = --m_cRef;
    TestAssert::IsTrue(static_cast<LONG>(cRef) >= 0, L"");
    if (cRef == 0)
      delete this;
    return cRef;
  }
  ULONG m_cRef;
};

MSO_STRUCT_GUID(ICustom, "5c58ced6-4f43-4b56-8d47-2236d00f7ccd")
struct ICustom : IUnknown
{
  virtual void SomeMethod() = 0;
};

TEST_CLASS (HolderTests)
{
  /*------------------------------------------------------------------------------
  ------------------------------------------------------------------------------*/
  TEST_METHOD(Ctor)
  {
    Mso::THolder<BYTE*, Mso::TDeleteHelper<BYTE*>> pb1;
    Mso::THolder<BYTE*, Mso::TDeleteHelper<BYTE*>> pb2(NULL);
    Mso::THolder<BYTE*, Mso::TDeleteHelper<BYTE*>> pb3 = NULL;
    Mso::THolder<BYTE*, Mso::TDeleteHelper<BYTE*>> pb4 = new BYTE;
  }

  /*------------------------------------------------------------------------------
  ------------------------------------------------------------------------------*/
  TEST_METHOD(Assignment)
  {
    Mso::THolder<BYTE*, Mso::TDeleteHelper<BYTE*>> pb1;
    Mso::THolder<BYTE*, Mso::TDeleteHelper<BYTE*>> pb2;
    Mso::THolder<BYTE*, Mso::TDeleteHelper<BYTE*>> pb3 = new BYTE;

    TestAssert::IsTrue(pb1 == nullptr, L"");
    TestAssert::IsTrue(pb1 == pb2, L"");
    TestAssert::IsTrue(pb1 != pb3, L"");
    TestAssert::IsTrue(pb3 != nullptr, L"");

    const BYTE* pbRaw = pb1.Get();
    TestAssert::IsTrue(pbRaw == nullptr, L"");
    TestAssert::IsTrue(pbRaw == pb1.Get(), L"");
    TestAssert::IsTrue(pbRaw != pb3.Get(), L"");

    pbRaw = pb3.Get();
    TestAssert::IsTrue(pbRaw != nullptr, L"");
    TestAssert::IsTrue(pbRaw != pb1.Get(), L"");
    TestAssert::IsTrue(pbRaw == pb3.Get(), L"");
  }

  /*------------------------------------------------------------------------------
  ------------------------------------------------------------------------------*/
  TEST_METHOD(Swap)
  {
    Mso::THolder<BYTE*, Mso::TDeleteHelper<BYTE*>> pb1;
    Mso::THolder<BYTE*, Mso::TDeleteHelper<BYTE*>> pb2 = new BYTE;
    Mso::THolder<BYTE*, Mso::TDeleteHelper<BYTE*>> pb3 = new BYTE;
    const BYTE* pb1Raw = pb1.Get();
    const BYTE* pb2Raw = pb2.Get();
    const BYTE* pb3Raw = pb3.Get();
    TestAssert::IsTrue(pb1Raw == nullptr, L"");
    TestAssert::IsTrue(pb2Raw != nullptr, L"");
    TestAssert::IsTrue(pb3Raw != nullptr, L"");
    TestAssert::IsTrue(pb2Raw != pb3Raw, L"");

    pb2.Swap(pb3);
    TestAssert::IsTrue(pb2Raw != pb2.Get(), L"");
    TestAssert::IsTrue(pb2Raw == pb3.Get(), L"");
    TestAssert::IsTrue(pb3Raw != pb3.Get(), L"");
    TestAssert::IsTrue(pb3Raw == pb2.Get(), L"");

    pb3.Swap(pb2);
    TestAssert::IsTrue(pb2Raw == pb2.Get(), L"");
    TestAssert::IsTrue(pb2Raw != pb3.Get(), L"");
    TestAssert::IsTrue(pb3Raw == pb3.Get(), L"");
    TestAssert::IsTrue(pb3Raw != pb2.Get(), L"");

    pb1.Swap(pb2);
    TestAssert::IsTrue(pb2Raw != pb2.Get(), L"");
    TestAssert::IsTrue(pb2Raw == pb1.Get(), L"");
    TestAssert::IsTrue(pb1Raw != pb1.Get(), L"");
    TestAssert::IsTrue(pb1Raw == pb2.Get(), L"");
    pb2.Swap(pb1);
  }

  /*------------------------------------------------------------------------------
    %%Id: 4a8bb941-2f5c-488a-8bc9-9f5b8c0897be
  ------------------------------------------------------------------------------*/
  TEST_METHOD(Transfer)
  {
    Mso::THolder<BYTE*, Mso::TDeleteHelper<BYTE*>> pb1;
    Mso::THolder<BYTE*, Mso::TDeleteHelper<BYTE*>> pb2 = new BYTE;
    TestAssert::IsTrue(pb1 == nullptr, L"");
    TestAssert::IsTrue(pb2 != nullptr, L"");

    pb1 = std::move(pb2);
    TestAssert::IsTrue(pb1 != nullptr, L"");
    TestAssert::IsTrue(pb2 == nullptr, L"");
  }

  /*------------------------------------------------------------------------------
  ------------------------------------------------------------------------------*/
  TEST_METHOD(Place)
  {
    Mso::THolder<BYTE*, Mso::TDeleteHelper<BYTE*>> pb1;
    Mso::THolder<BYTE*, Mso::TDeleteHelper<BYTE*>> pb2 = new BYTE;
    TestAssert::IsTrue(pb1 == nullptr, L"");
    TestAssert::IsTrue(pb2 != nullptr, L"");

    pb1.Place(pb2.Detach());
    TestAssert::IsTrue(pb1 != nullptr, L"");
    TestAssert::IsTrue(pb2 == nullptr, L"");
  }

  /*------------------------------------------------------------------------------
  ------------------------------------------------------------------------------*/
  TEST_METHOD(Attach)
  {
    Mso::THolder<BYTE*, Mso::TDeleteHelper<BYTE*>> pb1 = new BYTE;
    Mso::THolder<BYTE*, Mso::TDeleteHelper<BYTE*>> pb2 = new BYTE;
    TestAssert::IsTrue(pb1 != nullptr, L"");
    TestAssert::IsTrue(pb2 != nullptr, L"");

    pb1.Attach(pb2.Detach());
    TestAssert::IsTrue(pb1 != nullptr, L"");
    TestAssert::IsTrue(pb2 == nullptr, L"");
  }

  /*------------------------------------------------------------------------------
    %%Id: 040b6350-5468-48ff-b8cf-defc6ef412ad
  ------------------------------------------------------------------------------*/
  TEST_METHOD(Address)
  {
    Mso::THolder<BYTE*, Mso::TDeleteHelper<BYTE*>> pb1;
    Mso::THolder<BYTE*, Mso::TDeleteHelper<BYTE*>> pb2 = new BYTE;
    TestAssert::IsTrue(pb1 == nullptr, L"");
    TestAssert::IsTrue(pb2 != nullptr, L"");

    BYTE** ppb = &pb1;
    TestAssert::IsTrue(ppb == pb1.GetAddressOf(), L"");
    TestAssert::IsTrue(ppb == pb1.GetRaw(), L"");

    *ppb = pb2.Detach();
    TestAssert::IsTrue(pb1 != nullptr, L"");
    TestAssert::IsTrue(pb2 == nullptr, L"");
  }

  /*------------------------------------------------------------------------------
    %%Id: afe52e28-6314-446a-bb29-d2d07ce4a845
  ------------------------------------------------------------------------------*/
  TEST_METHOD(CntPtrCore)
  {
    Mso::CntPtr<CUnknown> punk1;
    Mso::CntPtr<CUnknown> punk2(new CUnknown());
    Mso::CntPtr<CUnknown> punk3(new CUnknown(), Mso::AttachTag);
    punk3->AddRef();
    Mso::CntPtr<CUnknown> punk4(punk3);
    Mso::CntPtr<CUnknown> punk5;
    punk5 = new CUnknown();

    TestAssert::IsTrue(punk1 == nullptr, L"");
    TestAssert::IsTrue(punk2 != nullptr, L"");
    TestAssert::IsTrue(punk3 != nullptr, L"");
    TestAssert::IsTrue(punk4 != nullptr, L"");
    TestAssert::IsTrue(punk5 != nullptr, L"");

    punk1 = punk2;
    TestAssert::IsTrue(punk1->m_cRef == 2, L"");
    TestAssert::IsTrue(punk2->m_cRef == 2, L"");
    punk1 = punk4;
    TestAssert::IsTrue(punk1->m_cRef == 3, L"");
    TestAssert::IsTrue(punk2->m_cRef == 1, L"");
    TestAssert::IsTrue(punk4->m_cRef == 3, L"");
    punk1 = nullptr;

    std::swap(punk1, punk3);
    std::swap(punk3, punk5);
    std::swap(punk5, punk1);
    std::swap(punk3, punk5);

    punk1 = std::move(punk2);
    punk2 = std::move(punk1);
    punk1.Attach(punk2.Detach());
    punk2.Attach(punk1.Detach());
    punk1.Attach(punk2.Detach());
    punk2.Attach(punk1.Detach());

    TestAssert::IsTrue(punk1 == nullptr, L"");
    TestAssert::IsTrue(punk2 != nullptr, L"");
    TestAssert::IsTrue(punk2->m_cRef == 1, L"");
    TestAssert::IsTrue(punk3 != nullptr, L"");
    TestAssert::IsTrue(punk3->m_cRef == 2, L"");
    TestAssert::IsTrue(punk4 != nullptr, L"");
    TestAssert::IsTrue(punk4->m_cRef == 2, L"");
    TestAssert::IsTrue(punk3 == punk4, L"");
    TestAssert::IsTrue(punk5 != nullptr, L"");
    TestAssert::IsTrue(punk5->m_cRef == 1, L"");
  }

  /*------------------------------------------------------------------------------
    %%Id: 5938c5b2-5d47-40a6-bd0a-903a7e126059
  ------------------------------------------------------------------------------*/
  TEST_METHOD(CntPtrMixed)
  {
    // TODO: [vmorozov] fix it.
    // Mso::CntPtr<IUnknown> punk;
    // Mso::CntPtr<ICustom> pistm;
    // TestAssert::IsTrue(pistm == nullptr, L"");
    // TestAssert::IsTrue(punk == nullptr, L"");
    // punk = pistm;
    // punk = &*pistm;
    // punk = *pistm.GetRaw();
    // punk = pistm.Copy();
    //*punk.Address() = pistm.Copy();
    //*punk.GetRaw() = pistm.Copy();
    // pistm = static_cast<ICustom*>(static_cast<IUnknown*>(punk));
    // pistm = static_cast<ICustom*>(&*punk);
    // pistm = static_cast<ICustom*>(*punk.GetRaw());
  }

/*------------------------------------------------------------------------------
  %%Id: b32deb10-ad98-452e-a307-8eb5d212429e
------------------------------------------------------------------------------*/
#ifdef MS_TARGET_WINDOWS
  TEST_METHOD(CntQIPtrCore)
  {
    Mso::CntPtr<IUnknown> punk1(new CUnknown());
    Mso::CntPtr<IUnknown> punk2(new CUnknown());
    Mso::CntPtr<IStream> pistmQI;

    TestAssert::IsTrue(FAILED(Mso::ComUtil::HrQueryFrom(pistmQI, punk1)), L"");
    TestAssert::IsTrue(SUCCEEDED(CreateStreamOnHGlobal(NULL, TRUE, &pistmQI)), L"");
    punk1 = pistmQI;
    TestAssert::IsTrue(Mso::ComUtil::AreEqualObjects(pistmQI, punk1), L"");
    pistmQI.Clear();
    TestAssert::AreEqual(S_OK, Mso::ComUtil::HrQueryFrom(pistmQI, punk1));
    TestAssert::IsTrue(Mso::ComUtil::AreEqualObjects(pistmQI, punk1), L"");
  }
#endif

  TEST_METHOD(TCleanup)
  // Use TCleanup to increment the variable.
  {
    int i = 0;
    {
      auto cuIncrement = Mso::TCleanup::Make([&]() noexcept { ++i; });
      TestAssert::AreEqual(0, i, L"");
    }
    TestAssert::AreEqual(1, i, L"");
  }
};
