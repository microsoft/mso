# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

liblet_includes(
  INCLUDES
    core/memoryApi.h
    memoryApi/leakDetection.h
    memoryApi/memoryApi.h
    memoryLeakScope/memoryLeakScope.h
    safeAlloct.h
)

#TODO: memoryapi/threadLocalUniquePtr.h