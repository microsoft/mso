// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// This file is added temporary for the purpose of excluding the liblet UT's that used SEH. // 
// Currently there is no support for filtering in Android. Bug # OM: 1706117 Need Filter support for Android Unit tests.
//
//
// Action item post this bug will be fixed ...
// we need to remove this file and all the occurrences of the macro TESTMETHOD_REQUIRES_SEH should be 
// replaced with TESTMETHODEX(${1}, TestCategory(RequriesSEH)).

#pragma once

#ifdef __clang__
#define TESTMETHOD_REQUIRES_SEH(Name) SKIPTESTMETHOD(Name)
#else
#define TESTMETHOD_REQUIRES_SEH(Name) TEST_METHOD(Name)
#endif
