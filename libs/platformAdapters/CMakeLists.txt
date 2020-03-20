# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

liblet(platformAdapters
  DEPENDS
    Mso::compilerAdapters
    Mso::debugAssertApi
    Mso::oacr
    Mso::tagUtils
  DEPENDS_POSIX
    Mso::platform_posix
)
