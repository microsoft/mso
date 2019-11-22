// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include <debugAssertApi/debugAssertApi.h>

namespace Mso {
namespace DebugAsserts {

#ifdef DEBUG

/**
	Legacy APIs for disabling asserts
*/
LIBLET_PUBLICAPI void DisableAssertTag(uint32_t tag) noexcept;
LIBLET_PUBLICAPI void EnableAssertTag(uint32_t tag) noexcept;
LIBLET_PUBLICAPI bool IsAssertTagDisabled(uint32_t tag) noexcept;

#else

inline void DisableAssertTag(uint32_t) noexcept {}
inline void EnableAssertTag(uint32_t) noexcept {}
inline bool IsAssertTagDisabled(uint32_t) noexcept { return false; }

#endif

#if 0
// TODO
struct AutoDisableAssertByTag
{
	AutoDisableAssertByTag(uint32_t tag) : 
		m_ignorer([this](const MsoAssertParams& params, const char* /*szMsg*/) noexcept 
		{
			return (params.dwTag == this->m_tag);
		}), 
		m_tag(tag)
	{
	}
	
	~AutoDisableAssertByTag() = default;
	
private:
	AutoRegisterAssertIgnorer m_ignorer;
	uint32_t m_tag;
};

/**
	Disable asserts that contain a given string
*/
struct AutoDisableAssertByString
{
	AutoDisableAssertByString(const char* match) : 
		m_ignorer([this](const MsoAssertParams& /*params*/, const char* szMsg) noexcept 
		{
			return params.dwTag == this->m_tag;
		}),
		m_match(match)
	{
	}
	
	~AutoDisableAssertByString() = default;
	
private:
	AutoRegisterAssertIgnorer m_ignorer;
	std::string m_match;
};

/**
	Ignores all debug asserts
*/
struct AutoIgnoreAllAsserts
{
	AutoIgnoreAllAsserts() noexcept : 
		m_ignorer([](const MsoAssertParams& /*params*/, const char* /*szMsg*/) noexcept 
		{ 
			return true; 
		})
	{
	}
	
	~AutoIgnoreAllAsserts() = default;

private:
	AutoRegisterAssertIgnorer m_ignorer;
};
#endif

} // DebugAsserts
} // Mso
