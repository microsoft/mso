# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

liblet_sources(
  SOURCES
    eventWaitHandleImpl.h
  SOURCES_WIN
    eventWaitHandleImpl_win.cpp
  SOURCES_POSIX
    eventWaitHandleImpl_posix.cpp
)