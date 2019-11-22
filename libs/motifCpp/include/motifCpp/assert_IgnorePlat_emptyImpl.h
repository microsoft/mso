// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/*----------------------------------------------------------------------------
    MotifCpp assert APIs
----------------------------------------------------------------------------*/
#pragma once
#ifndef MOTIFCPP_ASSERT_IGNOREPLAT_EMPTYIMPL_H
#define MOTIFCPP_ASSERT_IGNOREPLAT_EMPTYIMPL_H

#include <cstdint>

namespace Mso {

    class IgnoreAllAssertsPlatformImpl {};

    class IgnoreAssertPlatformImpl
    {
    public:
        IgnoreAssertPlatformImpl(uint32_t) {}
    };

} // Mso

#endif // MOTIFCPP_ASSERT_IGNOREPLAT_EMPTYIMPL_H
