// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
  Header to properly include windows.h
*/
#pragma once
#ifndef LIBLET_PLATFORMADAPTERS_WINDOWSFIRST_H
#define LIBLET_PLATFORMADAPTERS_WINDOWSFIRST_H

#ifdef MS_TARGET_WINDOWS

#ifndef NOMINMAX
#define NOMINMAX
#endif // !NOMINMAX

#include <winsock2.h>
#include <windows.h>

#endif // MS_TARGET_WINDOWS

#endif // LIBLET_PLATFORMADAPTERS_WINDOWSFIRST_H
