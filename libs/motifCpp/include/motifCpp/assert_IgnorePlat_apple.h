// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/*----------------------------------------------------------------------------
    MotifCpp assert APIs
    Apple platforms route motif asserts through NL (netlib misnomer, actually
    mbuinstrument), so direct calls to NL must also be explicitly disabled
----------------------------------------------------------------------------*/

#pragma once
#ifndef MOTIFCPP_ASSERT_IGNOREPLAT_APPLE_H
#define MOTIFCPP_ASSERT_IGNOREPLAT_APPLE_H

#include <platformAdapters/types.h>

#if DEBUG
// Forward decl, this is because including mbuassert.h brings in macro definitions such as
// UNTAGGED that can conflict with tagutils.h.
extern "C"
{
  typedef UInt32 nl_assert_tag_t;

  Boolean NLFAssertsEnabled();
  void NLEnableAllAsserts(Boolean inEnabled);

  Boolean NLIsAssertEnabledByTag(nl_assert_tag_t inTag);
  void NLDisableAssertByTag(nl_assert_tag_t inTag);
  void NLEnableAssertByTag(nl_assert_tag_t inTag);
}
#endif

namespace Mso {

#if DEBUG
class IgnoreAllAssertsPlatformImpl
{
public:
  IgnoreAllAssertsPlatformImpl() : m_nlEnabled(NLFAssertsEnabled())
  {
    NLEnableAllAsserts(false);
  }

  ~IgnoreAllAssertsPlatformImpl()
  {
    NLEnableAllAsserts(m_nlEnabled);
  }

private:
  const bool m_nlEnabled;
};

class IgnoreAssertPlatformImpl
{
public:
  IgnoreAssertPlatformImpl(DWORD tag) : m_tagEnabled(NLIsAssertEnabledByTag(tag)), m_tag(tag)
  {
    if (m_tagEnabled)
    {
      NLDisableAssertByTag(m_tag);
    }
  }

  ~IgnoreAssertPlatformImpl()
  {
    if (m_tagEnabled)
    {
      NLEnableAssertByTag(m_tag);
    }
  }

private:
  const bool m_tagEnabled;
  const nl_assert_tag_t m_tag;
};
#else // DEBUG
class IgnoreAllAssertsPlatformImpl
{
};
class IgnoreAssertPlatformImpl
{
public:
  IgnoreAssertPlatformImpl(uint32_t) {}
};
#endif

} // namespace Mso

#endif // MOTIFCPP_ASSERT_IGNOREPLAT_APPLE_H
