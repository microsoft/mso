// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/****************************************************************************
Unit tests for classes in the ObjectSwarm.h
****************************************************************************/

#include "precomp.h"
#include <object/swarm.h>
#include <object/unknownObject.h>
#include <test/skipSEHUT.h>
#include "testAllocators.h"
#include <test/testCheck.h>

//#define TEST_BAD_INHERITANCE1 // Uncomment to confirm VEC, but observe a memory leak. We cannot safely destroy this
//class.

MSO_STRUCT_GUID(ISwarmSample1, "962D2470-7452-43AB-9F74-63545A3E8A58")
struct DECLSPEC_NOVTABLE ISwarmSample1 : public IUnknown
{
  virtual int GetValue1() = 0;
};

MSO_STRUCT_GUID(ISwarmSample2, "B7E82DB0-436F-4B05-950F-FC1FE1ACE651")
struct DECLSPEC_NOVTABLE ISwarmSample2 : public IUnknown
{
  virtual int GetValue2() = 0;
};

class SwarmMemberSample1 final : public Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, ISwarmSample1>
{
public:
  SwarmMemberSample1(bool& deleted) noexcept : m_deleted(deleted) {}

  virtual int GetValue1() noexcept override
  {
    return 1;
  }

protected:
  virtual ~SwarmMemberSample1() noexcept
  {
    m_deleted = true;
  }

private:
  bool& m_deleted;
};

class SwarmMemberSample2 final : public Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, ISwarmSample2>
{
public:
  using MakePolicy = Mso::MakePolicy::NoThrowCtorAndInitializeThis;

  SwarmMemberSample2() noexcept : m_deleted(nullptr), m_other() {}

  bool InitializeThis(bool& deleted) noexcept
  {
    m_deleted = &deleted;
    return true;
  }

  virtual int GetValue2() noexcept override
  {
    return 2;
  }

  Mso::TCntPtr<SwarmMemberSample1> GetOther() noexcept
  {
    return m_other.Get();
  }

  bool IsDifferentSwarm() const noexcept
  {
    return m_other.IsDifferentSwarm();
  }

  void SetOther(SwarmMemberSample1& other) noexcept
  {
    Mso::TCntPtr<Mso::Swarm> swarm = Mso::Swarm::FromObject(this);
    VerifyElseCrashSzTag(!swarm.IsEmpty(), "This object must be part of a swarm.", 0x01003708 /* tag_bad2i */);
    m_other = Mso::SwarmMemberPtr<SwarmMemberSample1>(&other, *swarm);
  }

protected:
  virtual ~SwarmMemberSample2() noexcept
  {
    *m_deleted = true;
  }

private:
  bool* m_deleted;
  Mso::SwarmMemberPtr<SwarmMemberSample1> m_other;
};

class SwarmMemberSample2Base : public Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, ISwarmSample1>
{
public:
  SwarmMemberSample2Base(int index, const std::function<void(int)>& onDelete) : m_index(index), m_onDelete(onDelete) {}

  virtual int GetValue1() noexcept override
  {
    return 1;
  }

protected:
  virtual ~SwarmMemberSample2Base() noexcept
  {
    OACR_ASSUME_NOTHROW_BEGIN
    m_onDelete(m_index);
    OACR_ASSUME_NOTHROW_END
  }

private:
  int m_index;
  std::function<void(int)> m_onDelete;
};

class SwarmMemberSample21 : public SwarmMemberSample2Base
{
public:
  SwarmMemberSample21(int index, const std::function<void(int)>& onDelete) : SwarmMemberSample2Base(index, onDelete) {}
};

class SwarmMemberSample22 : public SwarmMemberSample2Base
{
public:
  SwarmMemberSample22(int index, const std::function<void(int)>& onDelete) : SwarmMemberSample2Base(index, onDelete) {}
};

class SwarmMemberSample3CannotAllocate : public Mso::UnknownObject<BadAllocWeakRefCount, ISwarmSample1>
{
public:
  SwarmMemberSample3CannotAllocate() noexcept {}

  virtual int GetValue1() noexcept override
  {
    return 1;
  }

protected:
  virtual ~SwarmMemberSample3CannotAllocate() noexcept {}
};

class SwarmMemberSampleBase : public Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, ISwarmSample1>
{
public:
  SwarmMemberSampleBase() noexcept {}

  virtual int GetValue1() noexcept override
  {
    return 1;
  }

protected:
  virtual ~SwarmMemberSampleBase() noexcept {}
};

class SwarmMemberSample4Throw : public SwarmMemberSampleBase
{
public:
  using MakePolicy = Mso::MakePolicy::ThrowCtor;

  SwarmMemberSample4Throw()
  {
    throw std::runtime_error("Test");
  }
};

class SwarmMemberSample5InitThrow : public SwarmMemberSampleBase
{
public:
  using MakePolicy = Mso::MakePolicy::ThrowCtorAndInitializeThis;

  void InitializeThis()
  {
    UNREFERENCED_OACR(this);
    throw std::runtime_error("Test");
  }
};

#ifdef TEST_BAD_INHERITANCE1
class SomeVirtualClass
{
public:
  virtual ~SomeVirtualClass() noexcept {}

  int x;
  int y;
};

// !!! Mso::UnknownObject must be always the first one in the inheritance !!!
class BadSwarmMember1 final
    : public SomeVirtualClass
    , public Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, ISwarmSample1>
{
public:
  virtual int GetValue1() override
  {
    return 1;
  }
};
#endif

TEST_CLASS (ObjectSwarmTest)
{
  TEST_METHOD(ObjectSwarm_OneMember)
  {
    bool deleted;
    {
      Mso::TCntPtr<SwarmMemberSample1> member1 = Mso::Swarm::Make<SwarmMemberSample1>(/*ref*/ deleted);
      TestAssert::IsNotNull(member1.Get());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(ObjectSwarm_TwoMembers)
  {
    bool deleted1;
    bool deleted2;
    {
      Mso::TCntPtr<SwarmMemberSample1> member1 = Mso::Swarm::Make<SwarmMemberSample1>(/*ref*/ deleted1);
      Mso::TCntPtr<Mso::Swarm> swarm = Mso::Swarm::FromObject(member1.Get());
      Mso::TCntPtr<SwarmMemberSample2> member2 = swarm->MakeMember<SwarmMemberSample2>(/*ref*/ deleted2);
      TestAssert::IsNotNull(member2.Get());
    }
    TestAssert::IsTrue(deleted1, L"First Swarm member is not deleted.");
    TestAssert::IsTrue(deleted2, L"Second Swarm member is not deleted.");
  }

  TEST_METHOD(ObjectSwarm_ThreeMembers)
  {
    // Test that members are deleted in reverse order to how they were created.
    const int memberCount = 3;
    int deleteOrderIndex = 0;
    int deleteOrder[memberCount];
    std::function<void(int)> onDelete = [&deleteOrderIndex, &deleteOrder](int index) noexcept {
      deleteOrder[--deleteOrderIndex] = index;
    };

    {
      Mso::TCntPtr<SwarmMemberSample21> member1 = Mso::Swarm::Make<SwarmMemberSample21>(deleteOrderIndex++, onDelete);
      Mso::TCntPtr<Mso::Swarm> swarm = Mso::Swarm::FromObject(member1.Get());
      Mso::TCntPtr<SwarmMemberSample22> member2 = swarm->MakeMember<SwarmMemberSample22>(deleteOrderIndex++, onDelete);
      TestAssert::IsNotNull(member2.Get());
      // Members can be of the same type
      Mso::TCntPtr<SwarmMemberSample22> member3 = swarm->MakeMember<SwarmMemberSample22>(deleteOrderIndex++, onDelete);
      TestAssert::IsNotNull(member3.Get());
    }

    TestAssert::AreEqual(0, deleteOrderIndex, L"Not all members are deleted");

    for (int i = 0; i < memberCount; i++)
    {
      TestAssert::AreEqual(i, deleteOrder[i], L"Wrong deletion order");
    }
  }

  TEST_METHOD(ObjectSwarm_FromObject)
  {
    bool deleted1;
    bool deleted2;
    {
      Mso::TCntPtr<SwarmMemberSample1> member1 = Mso::Swarm::Make<SwarmMemberSample1>(/*ref*/ deleted1);
      Mso::TCntPtr<Mso::Swarm> swarm = Mso::Swarm::FromObject(member1.Get());
      TestAssert::IsNotNull(swarm.Get());

      Mso::TCntPtr<SwarmMemberSample2> member2 = swarm->MakeMember<SwarmMemberSample2>(/*ref*/ deleted2);
      TestAssert::IsNotNull(member2.Get());
      swarm = Mso::Swarm::FromObject(member2.Get());
      TestAssert::IsNotNull(swarm.Get());

      Mso::TCntPtr<ISwarmSample1> memberIntf1 = qi_cast<ISwarmSample1>(member1.Get());
      TestAssert::IsNotNull(memberIntf1.Get());
      swarm = Mso::Swarm::FromObject(memberIntf1.Get());
      TestAssert::IsNotNull(swarm.Get());

      // Mso::TCntPtr<ISwarmSample2> memberIntf2 = qi_cast<ISwarmSample2>(member2.Get());
      // TestAssert::IsNotNull(memberIntf2.Get());
      // swarm = Mso::Swarm::FromObject(memberIntf2.Get());
      // TestAssert::IsNotNull(swarm.Get());
    }
    TestAssert::IsTrue(deleted1, L"First Swarm member is not deleted.");
    TestAssert::IsTrue(deleted2, L"Second Swarm member is not deleted.");
  }

  TEST_METHOD(ObjectSwarm_WeakPtr)
  {
    bool deleted1;
    bool deleted2;
    {
      Mso::WeakPtr<SwarmMemberSample1> memberWeak1;
      Mso::WeakPtr<SwarmMemberSample2> memberWeak2;
      Mso::WeakPtr<ISwarmSample1> memberIntfWeak1;
      Mso::WeakPtr<ISwarmSample2> memberIntfWeak2;
      {
        Mso::TCntPtr<SwarmMemberSample1> member1 = Mso::Swarm::Make<SwarmMemberSample1>(/*ref*/ deleted1);
        Mso::TCntPtr<Mso::Swarm> swarm = Mso::Swarm::FromObject(member1.Get());
        Mso::TCntPtr<SwarmMemberSample2> member2 = swarm->MakeMember<SwarmMemberSample2>(/*ref*/ deleted2);

        memberWeak1 = member1;
        memberWeak2 = member2;
        memberIntfWeak1 = qi_cast<ISwarmSample1>(member1.Get());
        memberIntfWeak2 = qi_cast<ISwarmSample2>(member2.Get());

        TestAssert::IsFalse(memberWeak1.IsExpired());
        TestAssert::IsFalse(memberWeak2.IsExpired());
        TestAssert::IsFalse(memberIntfWeak1.IsExpired());
        TestAssert::IsFalse(memberIntfWeak2.IsExpired());

        Debug(TestAssert::AreEqual(/*members*/ 2u + /*weakptr*/ 4u, member1->GetWeakRef().WeakRefCount()));
        Debug(TestAssert::AreEqual(/*members*/ 2u + /*swarm*/ 1u, member1->GetWeakRef().RefCount()));

        Mso::TCntPtr<SwarmMemberSample1> member11 = memberWeak1.GetStrongPtr();
        Mso::TCntPtr<SwarmMemberSample2> member21 = memberWeak2.GetStrongPtr();
        Mso::TCntPtr<ISwarmSample1> memberIntf11 = memberIntfWeak1.GetStrongPtr();
        Mso::TCntPtr<ISwarmSample2> memberIntf21 = memberIntfWeak2.GetStrongPtr();
        TestAssert::IsNotNull(member11.Get());
        TestAssert::IsNotNull(member21.Get());
        TestAssert::IsNotNull(memberIntf11.Get());
        TestAssert::IsNotNull(memberIntf21.Get());
      }
      TestAssert::IsTrue(memberWeak1.IsExpired());
      TestAssert::IsTrue(memberWeak2.IsExpired());
      TestAssert::IsTrue(memberIntfWeak1.IsExpired());
      TestAssert::IsTrue(memberIntfWeak2.IsExpired());

      Mso::TCntPtr<SwarmMemberSample1> member11 = memberWeak1.GetStrongPtr();
      Mso::TCntPtr<SwarmMemberSample2> member21 = memberWeak2.GetStrongPtr();
      Mso::TCntPtr<ISwarmSample1> memberIntf11 = memberIntfWeak1.GetStrongPtr();
      Mso::TCntPtr<ISwarmSample2> memberIntf21 = memberIntfWeak2.GetStrongPtr();
      TestAssert::IsNull(member11.Get());
      TestAssert::IsNull(member21.Get());
      TestAssert::IsNull(memberIntf11.Get());
      TestAssert::IsNull(memberIntf21.Get());
    }
    TestAssert::IsTrue(deleted1, L"First Swarm member is not deleted.");
    TestAssert::IsTrue(deleted2, L"Second Swarm member is not deleted.");
  }

  TEST_METHOD(ObjectSwarm_SwarmMemberPtr_SameSwarm)
  {
    bool deleted1;
    bool deleted2;
    {
      Mso::TCntPtr<SwarmMemberSample2> member2 = Mso::Swarm::Make<SwarmMemberSample2>(/*ref*/ deleted2);
      Mso::TCntPtr<Mso::Swarm> swarm = Mso::Swarm::FromObject(member2.Get());
      Mso::TCntPtr<SwarmMemberSample1> member1 = swarm->MakeMember<SwarmMemberSample1>(/*ref*/ deleted1);

      member2->SetOther(*member1);
      TestAssert::IsTrue(member1.Get() == member2->GetOther().Get());
      TestAssert::IsFalse(member2->IsDifferentSwarm());
    }
    TestAssert::IsTrue(deleted1);
    TestAssert::IsTrue(deleted2);
  }

  TEST_METHOD(ObjectSwarm_SwarmMemberPtr_DifferentSwarm)
  {
    bool deleted1;
    bool deleted2;
    {
      Mso::TCntPtr<SwarmMemberSample1> member1 = Mso::Swarm::Make<SwarmMemberSample1>(/*ref*/ deleted1);
      Mso::TCntPtr<SwarmMemberSample2> member2 = Mso::Swarm::Make<SwarmMemberSample2>(/*ref*/ deleted2);

      member2->SetOther(*member1);
      TestAssert::IsTrue(member1.Get() == member2->GetOther().Get());
      TestAssert::IsTrue(member2->IsDifferentSwarm());
    }
    TestAssert::IsTrue(deleted1);
    TestAssert::IsTrue(deleted2);
  }

  TEST_METHOD(ObjectSwarm_SwarmMemberPtr_KnownSameSwarm)
  {
    bool deleted1;
    bool deleted2;
    {
      Mso::TCntPtr<SwarmMemberSample2> member0 = Mso::Swarm::Make<SwarmMemberSample2>(/*ref*/ deleted2);
      Mso::TCntPtr<Mso::Swarm> swarm = Mso::Swarm::FromObject(member0.Get());

      Mso::SwarmMemberPtr<SwarmMemberSample1, /*KnownSameSwarm*/ true> member1 =
          swarm->MakeMember<SwarmMemberSample1>(/*ref*/ deleted1);
      TestAssert::IsNotNull(member1.Get());

      Mso::SwarmMemberPtr<SwarmMemberSample1, /*KnownSameSwarm*/ true> member2 = std::move(member1);
      TestAssert::IsNull(member1.Get());
      TestAssert::IsNotNull(member2.Get());

      using std::swap;
      swap(member1, member2);
      TestAssert::IsNotNull(member1.Get());
      TestAssert::IsNull(member2.Get());
      TestAssert::IsFalse(member1.IsEmpty());
      TestAssert::IsTrue(member2.IsEmpty());
    }
    TestAssert::IsTrue(deleted1);
    TestAssert::IsTrue(deleted2);
  }

  TEST_METHOD(ObjectSwarm_MakeMember_CannotAllocate)
  {
    bool deleted1;
    {
      Mso::TCntPtr<SwarmMemberSample1> member1 = Mso::Swarm::Make<SwarmMemberSample1>(/*ref*/ deleted1);
      Mso::TCntPtr<Mso::Swarm> swarm = Mso::Swarm::FromObject(member1.Get());

      TestAssert::ExpectVEC([&]() { swarm->MakeMember<SwarmMemberSample3CannotAllocate>(); });

      Debug(TestAssert::AreEqual(2u, swarm->RefCount()));
      Debug(TestAssert::AreEqual(1u, swarm->WeakRefCount()));
    }
    TestAssert::IsTrue(deleted1);
  }

  TEST_METHOD(ObjectSwarm_MakeMember_ConstructorThrows)
  {
    bool deleted1;
    {
      Mso::TCntPtr<SwarmMemberSample1> member1 = Mso::Swarm::Make<SwarmMemberSample1>(/*ref*/ deleted1);
      Mso::TCntPtr<Mso::Swarm> swarm = Mso::Swarm::FromObject(member1.Get());

      TestAssert::ExpectException<std::runtime_error>([&]() { swarm->MakeMember<SwarmMemberSample4Throw>(); });

      Debug(TestAssert::AreEqual(2u, swarm->RefCount()));
      Debug(TestAssert::AreEqual(1u, swarm->WeakRefCount()));
    }
    TestAssert::IsTrue(deleted1);
  }

  TEST_METHOD(ObjectSwarm_MakeMember_InitializeThrows)
  {
    bool deleted1;
    {
      Mso::TCntPtr<SwarmMemberSample1> member1 = Mso::Swarm::Make<SwarmMemberSample1>(/*ref*/ deleted1);
      Mso::TCntPtr<Mso::Swarm> swarm = Mso::Swarm::FromObject(member1.Get());

      TestAssert::ExpectException<std::runtime_error>([&]() { swarm->MakeMember<SwarmMemberSample5InitThrow>(); });

      Debug(TestAssert::AreEqual(2u, swarm->RefCount()));
      Debug(TestAssert::AreEqual(1u, swarm->WeakRefCount()));
    }
    TestAssert::IsTrue(deleted1);
  }

  TEST_METHOD(ObjectSwarm_SwarmMemberPtr_ObjectWeakRef_SameSwarm)
  {
    bool deleted1;
    bool deleted2;
    {
      Mso::TCntPtr<SwarmMemberSample2> member2 = Mso::Swarm::Make<SwarmMemberSample2>(/*ref*/ deleted2);
      Mso::TCntPtr<Mso::Swarm> swarm = Mso::Swarm::FromObject(member2.Get());
      Mso::TCntPtr<Mso::ObjectWeakRef> weakRef2 = &member2->GetWeakRef();

      Mso::TCntPtr<SwarmMemberSample1> member1 = swarm->MakeMember<SwarmMemberSample1>(/*ref*/ deleted1);

      Mso::SwarmMemberPtr<SwarmMemberSample1> swarmPtr =
          Mso::SwarmMemberPtr<SwarmMemberSample1>(member1.Get(), *weakRef2);
      TestAssert::IsTrue(swarmPtr.Get() == member1.Get());
      TestAssert::IsFalse(swarmPtr.IsDifferentSwarm());
    }
    TestAssert::IsTrue(deleted1);
    TestAssert::IsTrue(deleted2);
  }

  TEST_METHOD(ObjectSwarm_SwarmMemberPtr_ObjectWeakRef_DifferentSwarm)
  {
    bool deleted1;
    bool deleted2;
    {
      Mso::TCntPtr<SwarmMemberSample1> member1 = Mso::Swarm::Make<SwarmMemberSample1>(/*ref*/ deleted1);

      Mso::TCntPtr<SwarmMemberSample2> member2 = Mso::Swarm::Make<SwarmMemberSample2>(/*ref*/ deleted2);
      Mso::TCntPtr<Mso::ObjectWeakRef> weakRef2 = &member2->GetWeakRef();

      Mso::SwarmMemberPtr<SwarmMemberSample1> swarmPtr =
          Mso::SwarmMemberPtr<SwarmMemberSample1>(member1.Get(), *weakRef2);
      TestAssert::IsTrue(swarmPtr.Get() == member1.Get());
      TestAssert::IsTrue(swarmPtr.IsDifferentSwarm());
    }
    TestAssert::IsTrue(deleted1);
    TestAssert::IsTrue(deleted2);
  }

  TEST_METHOD(ObjectSwarm_SwarmMemberPtr_EqualityToNull)
  {
    bool deleted = false;
    {
      Mso::TCntPtr<SwarmMemberSample1> member = Mso::Swarm::Make<SwarmMemberSample1>(/*ref*/ deleted);
      Mso::SwarmMemberPtr<SwarmMemberSample1> swarmPtr =
          Mso::SwarmMemberPtr<SwarmMemberSample1>(member.Get(), *Mso::Swarm::FromObject(member.Get()));
      TestAssert::IsFalse(swarmPtr.IsDifferentSwarm());

      TestAssert::IsTrue(swarmPtr != nullptr);
      TestAssert::IsTrue(nullptr != swarmPtr);

      swarmPtr.Reset();

      TestAssert::IsTrue(swarmPtr == nullptr);
      TestAssert::IsTrue(nullptr == swarmPtr);
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(ObjectSwarm_SwarmMemberPtr_KnownSameSwarm_EqualityToNull)
  {
    bool deleted = false;
    {
      Mso::TCntPtr<SwarmMemberSample1> member = Mso::Swarm::Make<SwarmMemberSample1>(/*ref*/ deleted);
      Mso::SwarmMemberPtr<SwarmMemberSample1, /*KnownSameSwarm*/ true> swarmPtr =
          Mso::SwarmMemberPtr<SwarmMemberSample1, /*KnownSameSwarm*/ true>(member.Get());
      TestAssert::IsFalse(swarmPtr.IsDifferentSwarm());

      TestAssert::IsTrue(swarmPtr != nullptr);
      TestAssert::IsTrue(nullptr != swarmPtr);

      swarmPtr.Reset();

      TestAssert::IsTrue(swarmPtr == nullptr);
      TestAssert::IsTrue(nullptr == swarmPtr);
    }
    TestAssert::IsTrue(deleted);
  }

#if defined(DEBUG) && defined(TEST_BAD_INHERITANCE1)
  TESTMETHOD_REQUIRES_SEH(ObjectFixedSwarm_BadInheritance1)
  {
    TestAssert::ExpectVEC([&]() noexcept {
      // You will see a memory leak here because we cannot destroy object correctly.
      Mso::Swarm::Make<BadSwarmMember1>();
    });
  }
#endif
};
