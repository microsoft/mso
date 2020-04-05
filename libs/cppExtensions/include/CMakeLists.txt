# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

liblet_includes(
  INCLUDES
    cppExtensions/array.h
    cppExtensions/autoRestore.h
    cppExtensions/stringLiteral.h
  INCLUDES_WIN
    cppExtensions/threadLocal_fls.h
  INCLUDES_POSIX
    cppExtensions/threadLocal_pthread.h
)
