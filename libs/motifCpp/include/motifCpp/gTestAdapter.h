// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#ifndef GTESTADAPTER_H
#define GTESTADAPTER_H

#include "gtest/gtest.h"
#include <motifCpp/testInfo.h>

namespace Mso { namespace UnitTests { namespace GTest {

struct GTestFixture : ::testing::Test
{
  explicit GTestFixture(const TestClassInfo& classInfo, const TestMethodInfo& methodInfo)
      : m_methodInfo{methodInfo}, m_test{classInfo.CreateTest()}
  {
  }

  void TestBody() override
  {
    m_methodInfo.Invoke(*m_test);
  }

  static void SetUpTestSuite() {}

  static void TearDownTestSuite() {}

  void SetUp() override {}

  void TearDown() override {}

private:
  const TestMethodInfo& m_methodInfo;
  std::unique_ptr<TestClass> m_test;
};

inline void RegisterUnitTests()
{
  for (const TestClassInfo* classInfo : TestClassInfo::ClassInfos())
  {
    for (const TestMethodInfo* methodInfo : classInfo->MethodInfos())
    {
      ::testing::RegisterTest(
          /*test_suite_name:*/ classInfo->ClassName(),
          /*test_name:*/ methodInfo->MethodName(),
          /*type_param:*/ nullptr,
          /*value_param:*/ nullptr,
          /*file:*/ classInfo->FileName(),
          /*line:*/ methodInfo->SourceLine(),
          /*factory:*/ [classInfo, methodInfo]() {
            return new GTestFixture{*classInfo, *methodInfo};
          });
    }
  }
}

}}} // namespace Mso::UnitTests::GTest

#endif // GTESTADAPTER_H
