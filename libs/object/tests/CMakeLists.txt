# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

liblet_tests(
  SOURCES
    fixedSwarmTest.cpp
    holderTest.cpp
    lazyInitTest.cpp
    objectRefCountTest.cpp
    objectWithWeakRefTest.cpp
    queryCastTest.cpp
    TCntPtrRefTest.cpp
    TCntPtrTest.cpp
    testAllocators.h
    refCountedObjectTest.cpp
    swarmTest.cpp
    unknownObjectTest.cpp
    weakPtrTest.cpp
  DEPENDS
    Mso::eventWaitHandle
  )