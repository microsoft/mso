// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/****************************************************************************
Unit tests for classes in the ObjectSwarm.h
****************************************************************************/

#include "precomp.h"
#include <object/fixedSwarm.h>
#include <object/unknownObject.h>
#include <object/refCountedObject.h>
#include <test/skipSEHUT.h>
#include <test/testCheck.h>

//#define TEST_BAD_INHERITANCE1 // Uncomment to confirm VEC, but observe a memory leak. We cannot safely destroy this class.

MSO_STRUCT_GUID(IFixedSwarmSample1, "AA2EB60A-06DD-486F-AC9B-DBF1DDE21408")
struct DECLSPEC_NOVTABLE IFixedSwarmSample1 : IUnknown
{
	virtual int GetValue1() = 0;
};

struct DECLSPEC_NOVTABLE IFixedSwarmSample2 : Mso::IRefCounted
{
	virtual int GetValue2() = 0;
};

struct DECLSPEC_NOVTABLE IFixedSwarmSample3 : Mso::IRefCounted
{
	virtual int GetValue3() = 0;
};

class FixedSwarmMemberSample1 final
	: public Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, IFixedSwarmSample1>
{
public:
	FixedSwarmMemberSample1(bool& created, bool& deleted) noexcept
		: m_deleted(deleted)
	{
		created = true;
	}

	virtual int GetValue1() noexcept override
	{
		return 1;
	}

protected:
	virtual ~FixedSwarmMemberSample1() noexcept
	{
		m_deleted = true;
	}

private:
	bool& m_deleted;
};

// We can add to FixedSwarm IRefCounted objects
class FixedSwarmMemberSample2 final
	: public Mso::RefCountedObject<Mso::RefCountStrategy::WeakRef, IFixedSwarmSample2>
{
public:
	virtual int GetValue2() noexcept override
	{
		return 2;
	}
};

class FixedSwarmMemberSample3 final
	: public Mso::RefCountedObject<Mso::RefCountStrategy::WeakRef, IFixedSwarmSample3>
{
public:
	virtual int GetValue3() noexcept override
	{
		return 3;
	}
};

class FixedSwarmMemberSampleBase
	: public Mso::RefCountedObject<Mso::RefCountStrategy::WeakRef, IFixedSwarmSample2>
{
public:
	FixedSwarmMemberSampleBase() noexcept
	{
	}

	virtual int GetValue2() noexcept override
	{
		return 2;
	}

protected:
	virtual ~FixedSwarmMemberSampleBase() noexcept
	{
	}
};

class FixedSwarmMemberSample4Throw
	: public FixedSwarmMemberSampleBase
{
public:
	using MakePolicy = Mso::MakePolicy::ThrowCtor;

	FixedSwarmMemberSample4Throw()
	{
		throw std::runtime_error("Test");
	}
};

class FixedSwarmMemberSample5InitThrow
	: public FixedSwarmMemberSampleBase
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
	virtual ~SomeVirtualClass() = default;

	int x;
	int y;
};

// !!! Mso::UnknownObject must be always the first one in the inheritance !!!
class BadFixedSwarmMember1 final
	: public SomeVirtualClass
	, public Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, IFixedSwarmSample1>
{
public:
	virtual int GetValue1() override
	{
		return 1;
	}
};
#endif

TEST_CLASS(ObjectFixedSwarmTest)
{
	TEST_METHOD(ObjectFixedSwarm_NoMemberInit)
	{
		using Swarm1 = Mso::FixedSwarm<FixedSwarmMemberSample1>;
		Mso::TCntPtr<Swarm1> swarm1 = Mso::Make<Swarm1>();
		TestAssert::IsFalse(swarm1.IsEmpty());

		using Swarm2 = Mso::FixedSwarm<FixedSwarmMemberSample1, FixedSwarmMemberSample2>;
		Mso::TCntPtr<Swarm2> swarm2 = Mso::Make<Swarm2>();
		TestAssert::IsFalse(swarm2.IsEmpty());

		using Swarm3 = Mso::FixedSwarm<FixedSwarmMemberSample1, FixedSwarmMemberSample2, FixedSwarmMemberSample3>;
		Mso::TCntPtr<Swarm3> swarm3 = Mso::Make<Swarm3>();
		TestAssert::IsFalse(swarm3.IsEmpty());
	}

	TEST_METHOD(ObjectFixedSwarm_NoMemberRefCount)
	{
		using Swarm1 = Mso::FixedSwarm<FixedSwarmMemberSample1>;
		Mso::TCntPtr<Swarm1> swarm1 = Mso::Make<Swarm1>();
		TestAssert::IsFalse(swarm1.IsEmpty());
		
		Debug(TestAssert::AreEqual(1u, swarm1->RefCount()));
		Debug(TestAssert::AreEqual(1u, swarm1->WeakRefCount()));

		Mso::TCntPtr<Swarm1> swarm11 = swarm1;
		Debug(TestAssert::AreEqual(2u, swarm1->RefCount()));
		Debug(TestAssert::AreEqual(1u, swarm1->WeakRefCount()));

		Mso::WeakPtr<Swarm1> weakSwarm1 = swarm1;
		Debug(TestAssert::AreEqual(2u, swarm1->RefCount()));
		Debug(TestAssert::AreEqual(2u, swarm1->WeakRefCount()));

		Mso::WeakPtr<Swarm1> weakSwarm11 = swarm1;
		Debug(TestAssert::AreEqual(2u, swarm1->RefCount()));
		Debug(TestAssert::AreEqual(3u, swarm1->WeakRefCount()));
	}

	TEST_METHOD(ObjectFixedSwarm_GetMember)
	{
		using Swarm3 = Mso::FixedSwarm<FixedSwarmMemberSample1, FixedSwarmMemberSample2, FixedSwarmMemberSample3>;
		Mso::TCntPtr<Swarm3> swarm3 = Mso::Make<Swarm3>();
		TestAssert::IsFalse(swarm3.IsEmpty());

		FixedSwarmMemberSample1* sample1 = swarm3->Get<0>();
		TestAssert::IsNull(sample1);

		FixedSwarmMemberSample2* sample2 = swarm3->Get<1>();
		TestAssert::IsNull(sample2);

		FixedSwarmMemberSample3* sample3 = swarm3->Get<2>();
		TestAssert::IsNull(sample3);
	}

	TEST_METHOD(ObjectFixedSwarm_OneMember)
	{
		using Swarm1 = Mso::FixedSwarm<FixedSwarmMemberSample1>;
		bool created = false;
		bool deleted = false;

		{
			Mso::TCntPtr<Swarm1> swarm1 = Mso::Make<Swarm1>();

			{
				swarm1->MakeMember<0>(created, deleted);
				Mso::TCntPtr<IFixedSwarmSample1> member0 = swarm1->Get<0>();
				TestAssert::IsFalse(member0.IsEmpty());
				TestAssert::IsTrue(created);
				TestAssert::IsFalse(deleted);

				Debug(TestAssert::AreEqual(2u, swarm1->RefCount()));
				Debug(TestAssert::AreEqual(2u, swarm1->WeakRefCount()));
			}

			TestAssert::IsFalse(deleted);
			Debug(TestAssert::AreEqual(1u, swarm1->RefCount()));
			Debug(TestAssert::AreEqual(2u, swarm1->WeakRefCount()));
		}

		TestAssert::IsTrue(deleted);
	}

	TEST_METHOD(ObjectFixedSwarm_ThreeMembers)
	{
		using Swarm3 = Mso::FixedSwarm<FixedSwarmMemberSample1, FixedSwarmMemberSample2, FixedSwarmMemberSample3>;
		bool created = false;
		bool deleted = false;

		{
			Mso::TCntPtr<Swarm3> swarm3 = Mso::Make<Swarm3>();

			{
				swarm3->MakeMember<0>(created, deleted);
				Mso::TCntPtr<IFixedSwarmSample1> member0 = swarm3->Get<0>();
				TestAssert::IsFalse(member0.IsEmpty());
				Debug(TestAssert::AreEqual(2u, swarm3->RefCount()));
				Debug(TestAssert::AreEqual(2u, swarm3->WeakRefCount()));

				swarm3->MakeMember<1>();
				Mso::TCntPtr<IFixedSwarmSample2> member1 = swarm3->Get<1>();
				TestAssert::IsFalse(member1.IsEmpty());
				Debug(TestAssert::AreEqual(3u, swarm3->RefCount()));
				Debug(TestAssert::AreEqual(3u, swarm3->WeakRefCount()));

				swarm3->MakeMember<2>();
				Mso::TCntPtr<FixedSwarmMemberSample3> member2 = swarm3->Get<2>();
				TestAssert::IsFalse(member2.IsEmpty());
				Debug(TestAssert::AreEqual(4u, swarm3->RefCount()));
				Debug(TestAssert::AreEqual(4u, swarm3->WeakRefCount()));
			}

			TestAssert::IsFalse(deleted);
			Debug(TestAssert::AreEqual(1u, swarm3->RefCount()));
			// Each object +1 for weak reference count, +1 weak reference if strong ref count != 0.
			Debug(TestAssert::AreEqual(4u, swarm3->WeakRefCount()));
		}

		TestAssert::IsTrue(deleted);
	}

	TEST_METHOD(ObjectFixedSwarm_WeakPtr)
	{
		using Swarm3 = Mso::FixedSwarm<FixedSwarmMemberSample1, FixedSwarmMemberSample2, FixedSwarmMemberSample3>;
		bool created = false;
		bool deleted = false;

		Mso::WeakPtr<IFixedSwarmSample1> weakMember0;
		Mso::WeakPtr<FixedSwarmMemberSample2> weakMember1;
		Mso::WeakPtr<IFixedSwarmSample3> weakMember2;

		{
			Mso::TCntPtr<Swarm3> swarm3 = Mso::Make<Swarm3>();
			swarm3->MakeMember<0>(created, deleted);
			swarm3->MakeMember<1>();
			swarm3->MakeMember<2>();

			weakMember0 = swarm3->GetWeakPtr<0>();
			weakMember1 = swarm3->GetWeakPtr<1>();
			weakMember2 = swarm3->GetWeakPtr<2>();

			TestAssert::IsFalse(weakMember0.IsExpired());
			TestAssert::IsFalse(weakMember1.IsExpired());
			TestAssert::IsFalse(weakMember2.IsExpired());

			TestAssert::AreEqual(1, weakMember0.GetStrongPtr()->GetValue1());
			TestAssert::AreEqual(2, weakMember1.GetStrongPtr()->GetValue2());
			TestAssert::AreEqual(3, weakMember2.GetStrongPtr()->GetValue3());
		}

		TestAssert::IsTrue(weakMember0.IsExpired());
		TestAssert::IsTrue(weakMember1.IsExpired());
		TestAssert::IsTrue(weakMember2.IsExpired());

		TestAssert::IsNull(weakMember0.GetStrongPtr().Get());
		TestAssert::IsNull(weakMember1.GetStrongPtr().Get());
		TestAssert::IsNull(weakMember2.GetStrongPtr().Get());
	}

	TEST_METHOD(ObjectFixedSwarm_ConstructorThrows)
	{
		using Swarm2 = Mso::FixedSwarm<FixedSwarmMemberSample3, FixedSwarmMemberSample4Throw>;

		Mso::TCntPtr<Swarm2> swarm2 = Mso::Make<Swarm2>();
		swarm2->MakeMember<0>();
		TestAssert::ExpectException<std::runtime_error>([&]()
		{
			swarm2->MakeMember<1>();
		});

		Debug(TestAssert::AreEqual(1u, swarm2->RefCount()));
		Debug(TestAssert::AreEqual(2u, swarm2->WeakRefCount()));
	}

	TEST_METHOD(ObjectFixedSwarm_InitializeThrows)
	{
		using Swarm2 = Mso::FixedSwarm<FixedSwarmMemberSample3, FixedSwarmMemberSample5InitThrow>;

		Mso::TCntPtr<Swarm2> swarm2 = Mso::Make<Swarm2>();
		swarm2->MakeMember<0>();
		TestAssert::ExpectException<std::runtime_error>([&]()
		{
			swarm2->MakeMember<1>();
		});

		Debug(TestAssert::AreEqual(1u, swarm2->RefCount()));
		Debug(TestAssert::AreEqual(2u, swarm2->WeakRefCount()));
	}

#if defined(DEBUG) && defined(TEST_BAD_INHERITANCE1)
	TESTMETHOD_REQUIRES_SEH(ObjectFixedSwarm_BadInheritance1)
	{
		using Swarm2 = Mso::FixedSwarm<BadFixedSwarmMember1, FixedSwarmMemberSample2>;
		Mso::TCntPtr<Swarm2> swarm2 = Mso::Make<Swarm2>();
		TestAssert::ExpectVEC([&]() noexcept
		{
			// You will see a memory leak here because we cannot destroy object correctly.
			swarm2->MakeMember<0>();
		});
	}
#endif

};
