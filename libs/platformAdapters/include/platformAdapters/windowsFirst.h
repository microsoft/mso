// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_PLATFORMADAPTERS_WINDOWSFIRST_H
#define MSO_PLATFORMADAPTERS_WINDOWSFIRST_H

/**
  Header to properly include windows.h
*/

#ifdef MS_TARGET_WINDOWS

#ifndef NOMINMAX
#define NOMINMAX
#endif // !NOMINMAX

#include <winsock2.h>
#include <windows.h>

#endif // MS_TARGET_WINDOWS

#endif // MSO_PLATFORMADAPTERS_WINDOWSFIRST_H
