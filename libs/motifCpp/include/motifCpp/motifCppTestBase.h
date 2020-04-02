// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

typedef wchar_t WCHAR;

class MotifCppTestBase
{
public:
  MotifCppTestBase() {}

  virtual ~MotifCppTestBase()
  {
    TestClassTeardown();
  }

  virtual void TestClassSetup() {}

  virtual void TestClassTeardown() {}

  virtual void Setup() {}

  virtual void Teardown() {}
};