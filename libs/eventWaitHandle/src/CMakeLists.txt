# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

liblet_sources(
  SOURCES
    eventWaitHandle.h
  SOURCES_WIN
    eventWaitHandle_win.cpp
  SOURCES_POSIX
    eventWaitHandle_posix.cpp
)