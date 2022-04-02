/*************************************************************************
Simple namespace to wrap GUID comparisons
Copyright (C) 2004 Microsoft Corporation
*************************************************************************/
#pragma once
#ifndef MSO_PLATFORMADAPTERS_COMPARISON_H
#define MSO_PLATFORMADAPTERS_COMPARISON_H

#include <compileradapters/cppmacros.h>
#include <compileradapters/functiondecorations.h>
#include <guiddef.h>
#include <memoryApi/memoryComparison.h>
#include <utility>

namespace Mso::Guid::MemCompare {

	inline int Compare(const GUID& lhs, const GUID& rhs) noexcept
	{
		return Mso::Memory::Compare(lhs, rhs);
	}

	inline bool Exact(const GUID& lhs, const GUID& rhs) noexcept
	{
		return Mso::Memory::Exact(lhs, rhs);
	}

	inline bool Less(const GUID& lhs, const GUID& rhs) noexcept
	{
		return Mso::Memory::Less(lhs, rhs);
	}

	inline bool LessOrEqual(const GUID& lhs, const GUID& rhs) noexcept
	{
		return Mso::Memory::LessOrEqual(lhs, rhs);
	}

	inline bool More(const GUID& lhs, const GUID& rhs) noexcept
	{
		return Mso::Memory::More(lhs, rhs);
	}

	inline bool MoreOrEqual(const GUID& lhs, const GUID& rhs) noexcept
	{
		return Mso::Memory::MoreOrEqual(lhs, rhs);
	}
	
	struct LessFunctor
	{
		bool operator()(const GUID& lhs, const GUID& rhs) const noexcept { return Less(lhs, rhs); }
	};
} // Mso::Guid::MemCompare

namespace Mso::Guid::LexicalCompare {

	inline int Compare(const GUID& lhs, const GUID& rhs) noexcept
	{
		if (lhs.Data1 < rhs.Data1) 
			return -1; 
		else if (rhs.Data1 < lhs.Data1) 
			return 1;
		if (lhs.Data2 < rhs.Data2) 
			return -1; 
		else if (rhs.Data2 < lhs.Data2)
			return 1;
		if (lhs.Data3 < rhs.Data3) 
			return -1; 
		else if (rhs.Data3 < lhs.Data3) 
			return 1;
		for (int i = 0; i < 8; ++i)
			if (lhs.Data4[i] < rhs.Data4[i]) return -1; else if (rhs.Data4[i] < lhs.Data4[i]) return 1;
		return 0;
	}

	inline bool Exact(const GUID& lhs, const GUID& rhs) noexcept
	{
		return Compare(lhs, rhs) == 0;
	}

	inline bool Less(const GUID& lhs, const GUID& rhs) noexcept
	{
		return Compare(lhs, rhs) < 0;
	}

	inline bool LessOrEqual(const GUID& lhs, const GUID& rhs) noexcept
	{
		return Compare(lhs, rhs) <= 0;
	}

	inline bool More(const GUID& lhs, const GUID& rhs) noexcept
	{
		return Compare(lhs, rhs) > 0;
	}

	inline bool MoreOrEqual(const GUID& lhs, const GUID& rhs) noexcept
	{
		return Compare(lhs, rhs) >= 0;
	}
	
	struct LessFunctor
	{
		bool operator()(const GUID& lhs, const GUID& rhs) const noexcept { return Less(lhs, rhs); }
	};
} //Mso::Guid::LexicalCompare

#endif // MSO_PLATFORMADAPTERS_COMPARISON_H