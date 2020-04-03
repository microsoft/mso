# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

liblet(cppExtensions
  DEPENDS
    Mso::compilerAdapters
    Mso::platformAdapters
    Mso::typeTraits
  DEPENDS_POSIX
    Mso::oacr_posix
)
