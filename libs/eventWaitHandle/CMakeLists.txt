# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

liblet(eventWaitHandle
  DEPENDS
    Mso::object
  DEPENDS_LINUX
    Threads::Threads
)