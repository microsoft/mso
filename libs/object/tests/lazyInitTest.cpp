// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include <array>
#include <thread>

#include <object/lazyInit.h>
#include <object/refCountedObject.h>
#include <object/unknownObject.h>
#include <motifCpp/testCheck.h>

struct ICountedType : Mso::IRefCounted
{
  virtual int GetCallCount() const noexcept = 0;

  virtual int GetArgument0() const noexcept = 0;

  virtual char GetArgument1() const noexcept = 0;
};

class CountedType final : public Mso::RefCountedObject<Mso::RefCountStrategy::WeakRef, ICountedType>
{
  friend MakePolicy;

public:
  MSO_NO_COPY_CTOR_AND_ASSIGNMENT(CountedType);

  int GetCallCount() const noexcept override
  {
    return m_callCount;
  }

  int GetArgument0() const noexcept override
  {
    return m_argument0;
  }

  char GetArgument1() const noexcept override
  {
    return m_argument1;
  }

private:
  explicit CountedType() noexcept : m_callCount{++s_callCount} {}

  explicit CountedType(int argument0) noexcept : m_callCount{++s_callCount}, m_argument0{argument0} {}

  explicit CountedType(int argument0, char argument1) noexcept
      : m_callCount{++s_callCount}, m_argument0{argument0}, m_argument1{argument1}
  {
  }

  ~CountedType() noexcept override = default;

  const int m_callCount{0};

  const int m_argument0{0};

  const char m_argument1{'\0'};

  static std::atomic_int s_callCount;
};

std::atomic_int CountedType::s_callCount{0};

MSO_STRUCT_GUID(IUnknownType, "2F73DD3F-9255-4898-AF44-DE44158FE96E")
struct IUnknownType : IUnknown
{
  virtual int GetArgument0() const noexcept = 0;

  virtual char GetArgument1() const noexcept = 0;
};

class UnknownType final : public Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, IUnknownType>
{
  friend MakePolicy;

public:
  MSO_NO_COPY_CTOR_AND_ASSIGNMENT(UnknownType);

  int GetArgument0() const noexcept override
  {
    return m_argument0;
  }

  char GetArgument1() const noexcept override
  {
    return m_argument1;
  }

private:
  explicit UnknownType() noexcept = default;

  explicit UnknownType(int argument0) noexcept : m_argument0{argument0} {}

  explicit UnknownType(int argument0, char argument1) noexcept : m_argument0{argument0}, m_argument1{argument1} {}

  ~UnknownType() noexcept override = default;

  const int m_argument0{0};

  const char m_argument1{'\0'};
};

TEST_CLASS (LazyInitTests)
{
  TEST_METHOD(Get_CalledMultipleTimesForCountedType_ReturnsSameObject)
  {
    Mso::Object::LazyInit<CountedType> instance;

    const auto pointer{&(instance.Get())};
    TestAssert::AreEqual(
        (void*)&(instance.Get()),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the second time.");
    TestAssert::AreEqual(
        (void*)&(instance.Get()),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the third time.");

    TestAssert::AreEqual(pointer->GetArgument0(), 0, L"Argument 0 should have the default value.");
    TestAssert::AreEqual(pointer->GetArgument1(), '\0', L"Argument 1 should have the default value.");
  }

  TEST_METHOD(GetWith1Parameter_CalledMultipleTimesForCountedType_ReturnsSameObject)
  {
    Mso::Object::LazyInit<CountedType> instance;

    const auto pointer{&(instance.Get(1))};
    TestAssert::AreEqual(
        (void*)&(instance.Get()),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the second time.");
    TestAssert::AreEqual(
        (void*)&(instance.Get(1, 'A')),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the third time.");

    TestAssert::AreEqual(pointer->GetArgument0(), 1, L"Argument 0 should have the specified value.");
    TestAssert::AreEqual(pointer->GetArgument1(), '\0', L"Argument 1 should have the default value.");
  }

  TEST_METHOD(GetWith2Parameters_CalledMultipleTimesForCountedType_ReturnsSameObject)
  {
    Mso::Object::LazyInit<CountedType> instance;

    const auto pointer{&(instance.Get(1, 'A'))};
    TestAssert::AreEqual(
        (void*)&(instance.Get(1)),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the second time.");
    TestAssert::AreEqual(
        (void*)&(instance.Get()),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the third time.");

    TestAssert::AreEqual(pointer->GetArgument0(), 1, L"Argument 0 should have the specified value.");
    TestAssert::AreEqual(pointer->GetArgument1(), 'A', L"Argument 1 should have the specified value.");
  }

  TEST_METHOD(Get_AfterReleaseForCountedType_ReturnsDifferentObject)
  {
    Mso::Object::LazyInit<CountedType> instance;

    const auto callCount0{instance.Get().GetCallCount()};

    instance.Release();
    TestAssert::IsFalse(instance.IsInitialized(), L"Instance should not be initialized after Release().");

    const auto callCount1{instance.Get().GetCallCount()};
    TestAssert::AreEqual(
        callCount0 + 1,
        callCount1,
        L"The call count from before calling Release() should be one less than the one after, thereby ensuring the expected number of objects were created.");
  }

  TEST_METHOD(Get_InParallelForCountedType_ReturnsSameObject)
  {
    Mso::Object::LazyInit<CountedType> instance;
    std::array<CountedType*, 100> results;
    std::array<std::thread, results.size()> threads;

    for (size_t i{0}; i < results.size(); ++i)
    {
      threads[i] = std::thread([&](const size_t index) noexcept -> void { results[index] = &(instance.Get()); }, i);
    }

    for (auto& thread : threads)
    {
      thread.join();
    }

    for (size_t i{0}; i < results.size(); ++i)
    {
      TestAssert::AreEqual(
          (void*)results[i],
          (void*)results[0]); // , L"Call '%zu' to Get() should return the same value as the first call.", i);
    }
  }

  TEST_METHOD(IsInitialized_WithUninitializedObjectForCountedType_ReturnsFalse)
  {
    const Mso::Object::LazyInit<CountedType> instance{};

    TestAssert::IsFalse(instance.IsInitialized(), L"Instance should not be initialized.");
  }

  TEST_METHOD(IsInitialized_WithInitializedObjectForCountedType_ReturnsTru)
  {
    Mso::Object::LazyInit<CountedType> instance;

    instance.Get();
    TestAssert::IsTrue(instance.IsInitialized(), L"Instance should be initialized.");
  }

  TEST_METHOD(IsInitialized_WithReleasedObjectForCountedType_ReturnsFalse)
  {
    Mso::Object::LazyInit<CountedType> instance;

    instance.Get();
    instance.Release();
    TestAssert::IsFalse(instance.IsInitialized(), L"Instance should not be initialized after Release().");
  }

  TEST_METHOD(Release_WithUninitializedObjectForCountedType_SilentlySucceeds)
  {
    Mso::Object::LazyInit<CountedType> instance;

    instance.Release();
    TestAssert::IsFalse(instance.IsInitialized(), L"Instance should not be initialized after Release().");
  }

  TEST_METHOD(Release_InParallelForCountedType_SilentlySucceeds)
  {
    Mso::Object::LazyInit<CountedType> instance;
    instance.Get();

    std::array<std::thread, 100> threads;

    for (size_t i{0}; i < threads.size(); ++i)
    {
      threads[i] = std::thread([&]() noexcept -> void { instance.Release(); });
    }

    for (auto& thread : threads)
    {
      thread.join();
    }

    TestAssert::IsFalse(instance.IsInitialized(), L"Instance should not be initialized after Release().");
  }

  inline static std::wstring ToString(CountedType * /*q*/) noexcept
  {
    return L"CountedType";
  }

  inline static std::wstring ToString(ICountedType * /*q*/) noexcept
  {
    return L"ICountedType";
  }

  inline static std::wstring ToString(UnknownType * /*q*/) noexcept
  {
    return L"UnknownType";
  }

  inline static std::wstring ToString(IUnknownType * /*q*/) noexcept
  {
    return L"IUnknownType";
  }

  TEST_METHOD(Get_CalledMultipleTimesForCountedTypeWithInterface_ReturnsSameObject)
  {
    Mso::Object::LazyInit<CountedType, ICountedType> instance;

    const auto pointer{&(instance.Get())};
    TestAssert::AreEqual(
        (void*)&(instance.Get()),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the second time.");
    TestAssert::AreEqual(
        (void*)&(instance.Get()),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the third time.");

    TestAssert::AreEqual(pointer->GetArgument0(), 0, L"Argument 0 should have the default value.");
    TestAssert::AreEqual(pointer->GetArgument1(), '\0', L"Argument 1 should have the default value.");
  }

  TEST_METHOD(GetWith1Parameter_CalledMultipleTimesForCountedTypeWithInterface_ReturnsSameObject)
  {
    Mso::Object::LazyInit<CountedType, ICountedType> instance;

    const auto pointer{&(instance.Get(1))};
    TestAssert::AreEqual(
        (void*)&(instance.Get()),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the second time.");
    TestAssert::AreEqual(
        (void*)&(instance.Get(1, 'A')),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the third time.");

    TestAssert::AreEqual(pointer->GetArgument0(), 1, L"Argument 0 should have the specified value.");
    TestAssert::AreEqual(pointer->GetArgument1(), '\0', L"Argument 1 should have the default value.");
  }

  TEST_METHOD(GetWith2Parameters_CalledMultipleTimesForCountedTypeWithInterface_ReturnsSameObject)
  {
    Mso::Object::LazyInit<CountedType, ICountedType> instance;

    const auto pointer{&(instance.Get(1, 'A'))};
    TestAssert::AreEqual(
        (void*)&(instance.Get(1)),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the second time.");
    TestAssert::AreEqual(
        (void*)&(instance.Get()),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the third time.");

    TestAssert::AreEqual(pointer->GetArgument0(), 1, L"Argument 0 should have the specified value.");
    TestAssert::AreEqual(pointer->GetArgument1(), 'A', L"Argument 1 should have the specified value.");
  }

  TEST_METHOD(Get_AfterReleaseForCountedTypeWithInterface_ReturnsDifferentObject)
  {
    Mso::Object::LazyInit<CountedType> instance;

    const auto callCount0{instance.Get().GetCallCount()};

    instance.Release();
    TestAssert::IsFalse(instance.IsInitialized(), L"Instance should not be initialized after Release().");

    const auto callCount1{instance.Get().GetCallCount()};
    TestAssert::AreEqual(
        callCount0 + 1,
        callCount1,
        L"The call count from before calling Release() should be one less than the one after, thereby ensuring the expected number of objects were created.");
  }

  TEST_METHOD(Get_InParallelForCountedTypeWithInterface_ReturnsSameObject)
  {
    Mso::Object::LazyInit<CountedType, ICountedType> instance;
    std::array<ICountedType*, 100> results;
    std::array<std::thread, results.size()> threads;

    for (size_t i{0}; i < results.size(); ++i)
    {
      threads[i] = std::thread([&](const size_t index) noexcept -> void { results[index] = &(instance.Get()); }, i);
    }

    for (auto& thread : threads)
    {
      thread.join();
    }

    for (size_t i{0}; i < results.size(); ++i)
    {
      TestAssert::AreEqual(
          (void*)results[i],
          (void*)results[0]); //, L"Call '%zu' to Get() should return the same value as the first call.", i);
    }
  }

  TEST_METHOD(IsInitialized_WithUninitializedObjectForCountedTypeWithInterface_ReturnsFalse)
  {
    const Mso::Object::LazyInit<CountedType, ICountedType> instance{};

    TestAssert::IsFalse(instance.IsInitialized(), L"Instance should not be initialized.");
  }

  TEST_METHOD(IsInitialized_WithInitializedObjectForCountedTypeWithInterface_ReturnsTru)
  {
    Mso::Object::LazyInit<CountedType, ICountedType> instance;

    instance.Get();
    TestAssert::IsTrue(instance.IsInitialized(), L"Instance should be initialized.");
  }

  TEST_METHOD(IsInitialized_WithReleasedObjectForCountedTypeWithInterface_ReturnsFalse)
  {
    Mso::Object::LazyInit<CountedType, ICountedType> instance;

    instance.Get();
    instance.Release();
    TestAssert::IsFalse(instance.IsInitialized(), L"Instance should not be initialized after Release().");
  }

  TEST_METHOD(Release_WithUninitializedObjectForCountedTypeWithInterface_SilentlySucceeds)
  {
    Mso::Object::LazyInit<CountedType, ICountedType> instance;

    instance.Release();
    TestAssert::IsFalse(instance.IsInitialized(), L"Instance should not be initialized after Release().");
  }

  TEST_METHOD(Release_InParallelForCountedTypeWithInterface_SilentlySucceeds)
  {
    Mso::Object::LazyInit<CountedType, ICountedType> instance;
    instance.Get();

    std::array<std::thread, 100> threads;

    for (size_t i{0}; i < threads.size(); ++i)
    {
      threads[i] = std::thread([&]() noexcept -> void { instance.Release(); });
    }

    for (auto& thread : threads)
    {
      thread.join();
    }

    TestAssert::IsFalse(instance.IsInitialized(), L"Instance should not be initialized after Release().");
  }

  TEST_METHOD(Get_CalledMultipleTimesForUnknownType_ReturnsSameObject)
  {
    Mso::Object::LazyInit<UnknownType> instance;

    const auto pointer{&(instance.Get())};
    TestAssert::AreEqual(
        (void*)&(instance.Get()),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the second time.");
    TestAssert::AreEqual(
        (void*)&(instance.Get()),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the third time.");

    TestAssert::AreEqual(pointer->GetArgument0(), 0, L"Argument 0 should have the default value.");
    TestAssert::AreEqual(pointer->GetArgument1(), '\0', L"Argument 1 should have the default value.");
  }

  TEST_METHOD(GetWith1Parameter_CalledMultipleTimesForUnknownType_ReturnsSameObject)
  {
    Mso::Object::LazyInit<UnknownType> instance;

    const auto pointer{&(instance.Get(1))};
    TestAssert::AreEqual(
        (void*)&(instance.Get()),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the second time.");
    TestAssert::AreEqual(
        (void*)&(instance.Get(1, 'A')),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the third time.");

    TestAssert::AreEqual(pointer->GetArgument0(), 1, L"Argument 0 should have the specified value.");
    TestAssert::AreEqual(pointer->GetArgument1(), '\0', L"Argument 1 should have the default value.");
  }

  TEST_METHOD(GetWith2Parameters_CalledMultipleTimesForUnknownType_ReturnsSameObject)
  {
    Mso::Object::LazyInit<UnknownType> instance;

    const auto pointer{&(instance.Get(1, 'A'))};
    TestAssert::AreEqual(
        (void*)&(instance.Get(1)),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the second time.");
    TestAssert::AreEqual(
        (void*)&(instance.Get()),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the third time.");

    TestAssert::AreEqual(pointer->GetArgument0(), 1, L"Argument 0 should have the specified value.");
    TestAssert::AreEqual(pointer->GetArgument1(), 'A', L"Argument 1 should have the specified value.");
  }

  TEST_METHOD(Get_AfterReleaseForUnknownType_ReturnsDifferentObject)
  {
    Mso::Object::LazyInit<CountedType> instance;

    const auto callCount0{instance.Get().GetCallCount()};

    instance.Release();
    TestAssert::IsFalse(instance.IsInitialized(), L"Instance should not be initialized after Release().");

    const auto callCount1{instance.Get().GetCallCount()};
    TestAssert::AreEqual(
        callCount0 + 1,
        callCount1,
        L"The call count from before calling Release() should be one less than the one after, thereby ensuring the expected number of objects were created.");
  }

  TEST_METHOD(Get_InParallelForUnknownType_ReturnsSameObject)
  {
    Mso::Object::LazyInit<UnknownType> instance;
    std::array<UnknownType*, 100> results;
    std::array<std::thread, results.size()> threads;

    for (size_t i{0}; i < results.size(); ++i)
    {
      threads[i] = std::thread([&](const size_t index) noexcept -> void { results[index] = &(instance.Get()); }, i);
    }

    for (auto& thread : threads)
    {
      thread.join();
    }

    for (size_t i{0}; i < results.size(); ++i)
    {
      TestAssert::AreEqual(
          (void*)results[i],
          (void*)results[0]); // , L"Call '%zu' to Get() should return the same value as the first call.", i);
    }
  }

  TEST_METHOD(IsInitialized_WithUninitializedObjectForUnknownType_ReturnsFalse)
  {
    const Mso::Object::LazyInit<UnknownType> instance{};

    TestAssert::IsFalse(instance.IsInitialized(), L"Instance should not be initialized.");
  }

  TEST_METHOD(IsInitialized_WithInitializedObjectForUnknownType_ReturnsTru)
  {
    Mso::Object::LazyInit<UnknownType> instance;

    instance.Get();
    TestAssert::IsTrue(instance.IsInitialized(), L"Instance should be initialized.");
  }

  TEST_METHOD(IsInitialized_WithReleasedObjectForUnknownType_ReturnsFalse)
  {
    Mso::Object::LazyInit<UnknownType> instance;

    instance.Get();
    instance.Release();
    TestAssert::IsFalse(instance.IsInitialized(), L"Instance should not be initialized after Release().");
  }

  TEST_METHOD(Release_WithUninitializedObjectForUnknownType_SilentlySucceeds)
  {
    Mso::Object::LazyInit<UnknownType> instance;

    instance.Release();
    TestAssert::IsFalse(instance.IsInitialized(), L"Instance should not be initialized after Release().");
  }

  TEST_METHOD(Release_InParallelForUnknownType_SilentlySucceeds)
  {
    Mso::Object::LazyInit<UnknownType> instance;
    instance.Get();

    std::array<std::thread, 100> threads;

    for (size_t i{0}; i < threads.size(); ++i)
    {
      threads[i] = std::thread([&]() noexcept -> void { instance.Release(); });
    }

    for (auto& thread : threads)
    {
      thread.join();
    }

    TestAssert::IsFalse(instance.IsInitialized(), L"Instance should not be initialized after Release().");
  }

  TEST_METHOD(Get_CalledMultipleTimesForUnknownTypeWithInterface_ReturnsSameObject)
  {
    Mso::Object::LazyInit<UnknownType, IUnknownType> instance;

    const auto pointer{&(instance.Get())};
    TestAssert::AreEqual(
        (void*)&(instance.Get()),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the second time.");
    TestAssert::AreEqual(
        (void*)&(instance.Get()),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the third time.");

    TestAssert::AreEqual(pointer->GetArgument0(), 0, L"Argument 0 should have the default value.");
    TestAssert::AreEqual(pointer->GetArgument1(), '\0', L"Argument 1 should have the default value.");
  }

  TEST_METHOD(GetWith1Parameter_CalledMultipleTimesForUnknownTypeWithInterface_ReturnsSameObject)
  {
    Mso::Object::LazyInit<UnknownType, IUnknownType> instance;

    const auto pointer{&(instance.Get(1))};
    TestAssert::AreEqual(
        (void*)&(instance.Get()),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the second time.");
    TestAssert::AreEqual(
        (void*)&(instance.Get(1, 'A')),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the third time.");

    TestAssert::AreEqual(pointer->GetArgument0(), 1, L"Argument 0 should have the specified value.");
    TestAssert::AreEqual(pointer->GetArgument1(), '\0', L"Argument 1 should have the default value.");
  }

  TEST_METHOD(GetWith2Parameters_CalledMultipleTimesForUnknownTypeWithInterface_ReturnsSameObject)
  {
    Mso::Object::LazyInit<UnknownType, IUnknownType> instance;

    const auto pointer{&(instance.Get(1, 'A'))};
    TestAssert::AreEqual(
        (void*)&(instance.Get(1)),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the second time.");
    TestAssert::AreEqual(
        (void*)&(instance.Get()),
        (void*)pointer,
        L"Instance should always be the same when calling Get() the third time.");

    TestAssert::AreEqual(pointer->GetArgument0(), 1, L"Argument 0 should have the specified value.");
    TestAssert::AreEqual(pointer->GetArgument1(), 'A', L"Argument 1 should have the specified value.");
  }

  TEST_METHOD(Get_AfterReleaseForUnknownTypeWithInterface_ReturnsDifferentObject)
  {
    Mso::Object::LazyInit<CountedType> instance;

    const auto callCount0{instance.Get().GetCallCount()};

    instance.Release();
    TestAssert::IsFalse(instance.IsInitialized(), L"Instance should not be initialized after Release().");

    const auto callCount1{instance.Get().GetCallCount()};
    TestAssert::AreEqual(
        callCount0 + 1,
        callCount1,
        L"The call count from before calling Release() should be one less than the one after, thereby ensuring the expected number of objects were created.");
  }

  TEST_METHOD(Get_InParallelForUnknownTypeWithInterface_ReturnsSameObject)
  {
    Mso::Object::LazyInit<UnknownType, IUnknownType> instance;
    std::array<IUnknownType*, 100> results;
    std::array<std::thread, results.size()> threads;

    for (size_t i{0}; i < results.size(); ++i)
    {
      threads[i] = std::thread([&](const size_t index) noexcept -> void { results[index] = &(instance.Get()); }, i);
    }

    for (auto& thread : threads)
    {
      thread.join();
    }

    for (size_t i{0}; i < results.size(); ++i)
    {
      TestAssert::AreEqual(
          (void*)results[i],
          (void*)results[0]); // , L"Call '%zu' to Get() should return the same value as the first call.", i);
    }
  }

  TEST_METHOD(IsInitialized_WithUninitializedObjectForUnknownTypeWithInterface_ReturnsFalse)
  {
    const Mso::Object::LazyInit<UnknownType, IUnknownType> instance{};

    TestAssert::IsFalse(instance.IsInitialized(), L"Instance should not be initialized.");
  }

  TEST_METHOD(IsInitialized_WithInitializedObjectForUnknownTypeWithInterface_ReturnsTru)
  {
    Mso::Object::LazyInit<UnknownType, IUnknownType> instance;

    instance.Get();
    TestAssert::IsTrue(instance.IsInitialized(), L"Instance should be initialized.");
  }

  TEST_METHOD(IsInitialized_WithReleasedObjectForUnknownTypeWithInterface_ReturnsFalse)
  {
    Mso::Object::LazyInit<UnknownType, IUnknownType> instance;

    instance.Get();
    instance.Release();
    TestAssert::IsFalse(instance.IsInitialized(), L"Instance should not be initialized after Release().");
  }

  TEST_METHOD(Release_WithUninitializedObjectForUnknownTypeWithInterface_SilentlySucceeds)
  {
    Mso::Object::LazyInit<UnknownType, IUnknownType> instance;

    instance.Release();
    TestAssert::IsFalse(instance.IsInitialized(), L"Instance should not be initialized after Release().");
  }

  TEST_METHOD(Release_InParallelForUnknownTypeWithInterface_SilentlySucceeds)
  {
    Mso::Object::LazyInit<UnknownType, IUnknownType> instance;
    instance.Get();

    std::array<std::thread, 100> threads;

    for (size_t i{0}; i < threads.size(); ++i)
    {
      threads[i] = std::thread([&]() noexcept -> void { instance.Release(); });
    }

    for (auto& thread : threads)
    {
      thread.join();
    }

    TestAssert::IsFalse(instance.IsInitialized(), L"Instance should not be initialized after Release().");
  }
};