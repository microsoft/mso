# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

liblet(cppExtensions
  DEPENDS
    Mso::compilerAdapters
    Mso::cppType
    Mso::platformAdapters
  DEPENDS_POSIX
    Mso::oacr_posix
)
