// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#ifndef _CPPEXTENSIONS_THREADLOCALUNIQUEPTR_H_
#define _CPPEXTENSIONS_THREADLOCALUNIQUEPTR_H_
#ifdef __cplusplus
#include <threadlocal/threadlocal.h>
#include <memoryleakscope/memoryleakscope.h>

namespace Mso {

/**
Acts like unique_ptr for a thread-local variable, automatically deletes on thread-exit.
Use as static variable since this object lifetime controls the backing thread-storage.
*/
template<typename T, typename Deleter = std::default_delete<T>>
class threadlocal_unique_ptr : private Mso::ThreadLocal<T*>
{
	using Super = Mso::ThreadLocal<T*>;

public:
	threadlocal_unique_ptr() noexcept : Super(Delete)
	{
	}

	explicit operator bool() const noexcept
	{
		return get() != nullptr;
	}

	T* operator ->() const noexcept
	{
		return get();
	}

	T* get() const noexcept
	{
		return Super::GetValue();
	}

	T& getOrCreate() noexcept
	{
		auto t = get();
		if (!t)
		{
#ifdef DEBUG
			Mso::Memory::AutoIgnoreLeakScope ignoreTlsVar;
#endif
			t = new T();
			reset(t);
		}

		return *t;
	}

	void reset(T* t) noexcept
	{
		Delete(release());
		Super::SetValue(t);
	}

	T* release() noexcept
	{
		auto t = get();
		Super::SetValue(nullptr);
		return t;
	}

private:
	static void __stdcall Delete(T* t) noexcept
	{
		Deleter()(t);
	}
};

} // Mso

#endif // C++
#endif // _CPPEXTENSIONS_THREADLOCALUNIQUEPTR_H_

