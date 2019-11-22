# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

liblet(memoryApi
  DEPENDS
    Mso::compilerAdapters
    Mso::cppType
    Mso::crash
    Mso::debugAssertApi
    Mso::oacr
    Mso::smartPtr
    Mso::tagUtils
  DEPENDS_POSIX
    Mso::platform_posix
)
